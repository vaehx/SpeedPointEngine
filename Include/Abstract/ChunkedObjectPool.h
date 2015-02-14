//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2015, Pascal R.
// ------------------------------------------------------------------------------
// Filename:	ChunkedObjectPool.h
// Created:	1/30/2015 by Pascal R.
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>

// Summary:
// 	Object pool that is able to recycle allocated objects and
// 	never invalidates pointers. Thus, this storage is suitable for
// 	high-frequency reads and (slightly slower) writes.
// Description:
// 	As chunk_size, use a predeterminated value that is optimal for
// 	the usage of this storage. A higher value may consume more
// 	unused memory, but avoids frequent resize and thereby the chunks
// 	lying distributed over the memory. However, with a lower value,
// 	consumption of unused memory might be much lower.
template<typename T, const unsigned int chunk_size = 20>
class ChunkedObjectPool
{
private:
	struct Object {
		typename T instance;
		bool used;
		Object() : used(false) {}
	};
	struct Chunk {
		typename ChunkedObjectPool<T, chunk_size>::Object* objects;
		unsigned int num_used_objects;
		unsigned int first_used_object, last_used_object;
		unsigned int* frees;	// stack
		unsigned int num_frees;

		Chunk()
			: objects(0),
			num_used_objects(0),
			first_used_object(0),
			last_used_object(0),
			frees(0),
			num_frees(0) {}

		~Chunk()
		{
			if (objects)
				delete[] objects;

			if (frees)
				delete[] frees;

			objects = 0;
			frees = 0;
			num_used_objects = 0;
			first_used_object = 0;
			last_used_object = 0;
			num_frees = 0;
		}

		// sizeof objects buffer in bytes
		unsigned int GetObjectsByteSize() const {
			return sizeof(Object) * (num_used_objects + num_frees);
		}
	}** chunks;
	unsigned int num_chunks;
	unsigned int num_used_objects;
public:
	ChunkedObjectPool()
		: chunks(0), num_chunks(0), num_used_objects(0) {}

	unsigned int GetUsedObjectCount() const { return num_used_objects; }
	unsigned int GetFreeCount() const { return num_chunks * chunk_size - num_used_objects; }

	// Summary:
	//	Returns a used object with object index > objindex. 0 if no more used object
	T* GetNextUsedObject(unsigned int& objindex)
	{
		if (!chunks || num_chunks == 0 || num_used_objects == 0)
			return 0;

		if (objindex >= num_chunks * chunk_size - 1)
			return 0; // end of pool

		unsigned int chunkobjindex = objindex % chunk_size; // objindex relative to chunk start
		unsigned int ic = (unsigned int)((objindex - chunkobjindex) / chunk_size);

		for (; ic < num_chunks; ++ic)
		{
			if (!chunks[ic])
			{
				// chunk array damage. try to skip.
				chunkobjindex = 0;
				continue;
			}

			Chunk& chunk = *chunks[ic];
			if (chunk.num_used_objects == 0 || chunkobjindex >= chunk.last_used_object)
			{
				chunkobjindex = 0;
				continue;
			}

			// skip free objects at chunk start
			if (chunkobjindex < chunk.first_used_object)
			{
				chunkobjindex = chunk.first_used_object;
				objindex = ic * chunk_size + chunkobjindex;
				return &chunk.objects[chunkobjindex].instance;
			}

			for (unsigned int i = chunkobjindex + 1; i <= chunk.last_used_object; ++i)
			{
				if (!chunk.objects[i].used)
					continue;

				objindex = ic * chunk_size + i;
				return &chunk.objects[i].instance;
			}

			// not found, next chunks start search at chunk start
			chunkobjindex = 0;
		}

		// nothing more found
		return 0;
	}

	// Summary:
	//	Finds unused slot, flags it used and returns pointer to instance
	T* Get()
	{
		Object* freeobj = 0;
		unsigned int freeobjindex = 0;
		Chunk* freeobjchunk = 0;

		if (GetFreeCount() == 0)
		{
			// add new chunk
			Chunk* newChunk = new Chunk();
			newChunk->objects = new Object[chunk_size];
			newChunk->num_used_objects = 0;

			newChunk->frees = new unsigned int[chunk_size];
			newChunk->num_frees = chunk_size;
			for (unsigned int i = 0; i < chunk_size; ++i)
				newChunk->frees[i] = i;

			newChunk->first_used_object = 0;
			newChunk->last_used_object = 0;

			Chunk** oldChunks = chunks;
			chunks = new Chunk*[++num_chunks];
			chunks[num_chunks - 1] = newChunk;
			if (oldChunks)
			{
				memcpy(chunks, oldChunks, sizeof(Chunk*) * (num_chunks - 1));
				delete[] oldChunks;
			}
			
			freeobjindex = newChunk->frees[chunk_size - 1];
			freeobj = &newChunk->objects[freeobjindex];
			freeobjchunk = newChunk;
		}
		else
		{
			for (unsigned int ic = 0; ic < num_chunks; ++ic)
			{
				Chunk& chunk = *chunks[ic];
				if (chunk.num_frees == 0)
					continue;

				freeobjindex = chunk.frees[chunk.num_frees - 1];
				freeobj = &chunk.objects[freeobjindex];
				freeobjchunk = chunks[ic];
				break;
			}
		}

		if (!freeobj || !freeobjchunk)
			return 0; // unexpected

		freeobj->used = true;
		freeobjchunk->num_used_objects++;
		freeobjchunk->num_frees--;

		if (freeobjindex < freeobjchunk->first_used_object || freeobjchunk->num_used_objects == 1)
			freeobjchunk->first_used_object = freeobjindex;

		if (freeobjindex > freeobjchunk->last_used_object || freeobjchunk->num_used_objects == 1)
			freeobjchunk->last_used_object = freeobjindex;


		num_used_objects++;

		return &freeobj->instance;
	}

	// Summary:
	//	Releases this object und flags it unused. Instance is not destructed.
	void Release(T** pInstance)
	{
		T* instance = 0;
		if (num_chunks == 0 || !pInstance || !(instance = *pInstance))
			return;

		for (unsigned int ic = 0; ic < num_chunks; ++ic)
		{
			Chunk& chunk = *chunks[ic];
			if ((unsigned long)instance < (unsigned long)chunk.objects || (unsigned long)instance >(unsigned long)chunk.objects + (unsigned long)chunk.GetObjectsByteSize())
				continue;

			unsigned int ptrOffs = (unsigned int)instance - (unsigned int)chunk.objects;
			unsigned int iObj = (ptrOffs - (ptrOffs % sizeof(Object))) / sizeof(Object);
			Object& obj = chunk.objects[iObj];
			if (!obj.used)
				return;

			obj.used = false;
			*pInstance = 0;
			instance = 0;

			num_used_objects--;
			chunk.num_used_objects--;
			chunk.frees[chunk.num_frees] = iObj;
			chunk.num_frees++;

			if (chunk.num_used_objects == 0)
			{
				chunk.first_used_object = 0;
				chunk.last_used_object = 0;
			}
			else if (iObj == chunk.first_used_object || iObj == chunk.last_used_object)
			{
				bool objIsFirstUsedObj = (iObj == chunk.first_used_object);
				unsigned int& checkLimit = (objIsFirstUsedObj) ? chunk.first_used_object : chunk.last_used_object;
				unsigned int& oppositeCheckLimit = (objIsFirstUsedObj) ? chunk.last_used_object : chunk.first_used_object;
				unsigned int diff = (objIsFirstUsedObj) ? 1 : -1;

				for (unsigned int i = checkLimit + 1;; i += diff)
				{
					if (!chunk.objects[i].used)
					{
						if ((i + diff) == oppositeCheckLimit)
							break;

						continue;
					}

					checkLimit = i;
					break;
				}

				if (checkLimit == iObj)
					return; // unexpected
			}

			break; // found according obj in chunk
		}
	}


	void Clear()
	{
		for (unsigned int ic = 0; ic < num_chunks; ++ic)
			delete chunks[ic];

		delete[] chunks;
		chunks = 0;

		num_chunks = 0;
		num_used_objects = 0;
	}

	~ChunkedObjectPool()
	{
		Clear();
	}
};
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

	// Returns the object with the given index from the pool or 0 if not found
	T* GetAt(unsigned int idx) const
	{
		unsigned int ic = 0;
		for (; ic < num_chunks; ++ic)
		{
			if (idx >= chunks[ic]->num_used_objects)
				idx -= chunks[ic]->num_used_objects;
			else
				break;
		}

		if (ic == num_chunks)
			return 0;

		Chunk& chunk = *chunks[ic];
		for (unsigned int i = chunk.first_used_object; i < chunk.num_used_objects; ++i)
		{
			if (!chunk.objects[i].used)
				continue;
			
			if (idx == 0)
				return &chunk.objects[i].instance;
			
			idx--;
		}

		return 0;
	}

	// objindex is set to the index of the first object, regardless of its previous value
	// Returns:
	//	0 if no object is in the pool, pointer to the instance of the first used object otherwise
	T* GetFirstUsedObject(unsigned int& objindex) const
	{
		if (!chunks || num_chunks == 0 || num_used_objects == 0)
			return 0;

		for (unsigned int ic = 0; ic < num_chunks; ++ic)
		{
			Chunk& chunk = *chunks[ic];
			if (chunk.num_used_objects == 0)
				continue;

			objindex = ic * chunk_size + chunk.first_used_object;
			return &chunk.objects[chunk.first_used_object].instance;
		}

		return 0;
	}

	// Summary:
	//	Returns a used object with object index > objindex. 0 if no more used object
	T* GetNextUsedObject(unsigned int& objindex) const
	{
		if (!chunks || num_chunks == 0 || num_used_objects == 0)
			return 0;

		if (objindex >= num_chunks * chunk_size - 1)
			return 0; // end of pool

		++objindex;

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
			if (chunk.num_used_objects == 0 || chunkobjindex > chunk.last_used_object)
			{
				chunkobjindex = 0;
				continue; // go to next chunk
			}

			// skip free objects at chunk start
			if (chunkobjindex < chunk.first_used_object)
			{
				chunkobjindex = chunk.first_used_object;
				objindex = ic * chunk_size + chunkobjindex;
				return &chunk.objects[chunkobjindex].instance;
			}

			for (unsigned int i = chunkobjindex; i <= chunk.last_used_object; ++i)
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
				newChunk->frees[i] = (chunk_size - 1 - i);

			newChunk->first_used_object = 0;
			newChunk->last_used_object = 0;

			Chunk** oldChunks = chunks;
			chunks = new Chunk*[++num_chunks];
			chunks[num_chunks - 1] = newChunk;
			if (oldChunks)
			{
				// Copy array of chunk-pointers
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

	// Returns true if this pointer points to a used object in any chunk of this pool.
	bool IsValidPtr(const T* ptr)
	{
		if (ptr)
		{
			long ptrOffs;
			for (unsigned int ic = 0; ic < num_chunks; ++ic)
			{
				const Chunk& chunk = *chunks[ic];
				ptrOffs = (long)ptr - (long)chunk.objects;
				if (ptrOffs >= 0 && ptrOffs < (long)chunk_size)
					return chunk.objects[ptrOffs].used;
			}
		}

		return false;
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
			if ((unsigned long)instance < (unsigned long)chunk.objects || (unsigned long)instance > (unsigned long)chunk.objects + (unsigned long)chunk.GetObjectsByteSize())
				continue;

			unsigned int ptrOffs = (unsigned int)((unsigned long)instance - (unsigned long)chunk.objects);
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
			else if (iObj == chunk.first_used_object)
			{
				// find new first used object
				for (; chunk.first_used_object < chunk.last_used_object; ++chunk.first_used_object)
				{
					if (chunk.objects[chunk.first_used_object].used)
						break;
				}
			}
			else if (iObj == chunk.last_used_object)
			{
				// find new last used object
				for (; chunk.last_used_object > chunk.first_used_object; --chunk.last_used_object)
				{
					if (chunk.objects[chunk.last_used_object].used)
						break;
				}
			}

			break; // found according obj in chunk
		}
	}

	// Releases all objects in the pool.
	// WARNING: All pointers to any object in the pool are invalidated, but still point to a correct address!
	void ReleaseAll()
	{		
		for (unsigned int ic = 0; ic < num_chunks; ++ic)
		{
			// Repair missing chunk
			if (!chunks[ic])
				chunks[ic] = new Chunk();

			// Reset chunk
			Chunk& chunk = *chunks[ic];
			chunk.num_used_objects = 0;
			chunk.num_frees = chunk_size;
			for (unsigned int i = 0; i < chunk_size; ++i)
			{
				chunk.frees[i] = i;
				chunk.objects[i].used = false;
			}

			chunk.first_used_object = 0;
			chunk.last_used_object = 0;
		}

		num_used_objects = 0;
	}

	// Deletes the chunk memory.
	// All pointers are invalidated.
	// To avoid deallocating all already allocated memory, use ReleaseAll()
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


public:
	class Iterator
	{
	private:
		ChunkedObjectPool<T, chunk_size>* pool;
		T* ptr;
		unsigned int i;
	public:
		Iterator(ChunkedObjectPool<T, chunk_size>* _pool)
			: pool(_pool)
		{
			ptr = 0;
			if (pool)
				ptr = pool->GetFirstUsedObject(i = 0);
			if (!ptr)
				i = UINT_MAX;
		}

		Iterator(ChunkedObjectPool<T, chunk_size>* _pool, T* _ptr, unsigned int _i)
			: pool(_pool), ptr(_ptr), i(_i)
		{
			if (!pool)
				ptr = 0;
			else if (!pool->IsValidPtr(ptr))
				ptr = 0;
			if (!ptr)
				i = UINT_MAX;
		}

		T* operator ->() const { return ptr; }
		T* operator *() const { return ptr; }
		operator bool() const { return pool && ptr && i < UINT_MAX; }
		bool operator ==(const Iterator& it) const { return (pool == it.pool) && (ptr == it.ptr) && (i == it.i); }
		bool operator !=(const Iterator& it) const
		{
			return (pool != it.pool) || (ptr != it.ptr) || (i != it.i);
		}
		Iterator& operator ++()
		{
			if (pool)
				ptr = pool->GetNextUsedObject(i);
			if (!ptr)
				i = UINT_MAX;
			return *this;
		}
	};

	inline Iterator begin() { return Iterator(this); }
	inline Iterator end() { return Iterator(this, 0, UINT_MAX); }
};

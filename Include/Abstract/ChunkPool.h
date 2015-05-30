


// Chunk based Pool

#pragma once
#include <SPrerequisites.h>

SP_NMSPACE_BEG

template<typename T>
struct S_API ChunkSlot
{
	T instance;
	char* specification;
	bool bUsed;

	ChunkSlot()
		: bUsed(false),
		specification(0)
	{
	}

	~ChunkSlot()
	{
		if (IS_VALID_PTR(specification))
			delete[] specification;

		specification = 0;
	}
};


template<typename T, int slots = 10>
struct S_API ChunkPoolChunk
{
	ChunkSlot<T> pSlots[slots];
	unsigned int nSlots;
	unsigned int nUsedSlots;

	ChunkPoolChunk()
	{
		nSlots = slots;
		nUsedSlots = 0;
	}
};



// Notes:
//	The T type should override the default constructor and assignment operator (=). Otherwise
//	the newly created objects are not filled / constructed properly.
template<typename T, unsigned int chunkSz = 10>
class S_API ChunkPool
{
public:
	typedef ChunkPoolChunk<T, chunkSz> Chunk;
	typedef T* InstancePtr;

private:
	Chunk** pChunks;

	unsigned int nChunks;

public:
	ChunkPool()
		: pChunks(nullptr),
		nChunks(0)
	{
	}

	~ChunkPool()
	{
		Clear();
	}



	// Arguments:
	//	createSz - When the Chunk Ptr is not valid (not inited) this size specifies the creation size
	ILINE SResult CheckChunksPtr(unsigned int createSz = 1)
	{
		SP_ASSERTR(createSz > 0, S_INVALIDPARAM);

		if (!IS_VALID_PTR(pChunks))
		{
			pChunks = new Chunk*[createSz];
			for (unsigned int i = 0; i < createSz; ++i)
				pChunks[i] = new ChunkPoolChunk<T, chunkSz>();

			nChunks = createSz;
		}

		return S_SUCCESS;
	}







	// Arguments:
	//	pNewChnksBegIndex - If not 0 (default), the first index of newly created chunks are assigned
	//			    If no new chunks have been created and is a valid ptr, MAX_UINT32 is asigned to it
	ILINE SResult CheckMinChunkCount(unsigned int n, unsigned int* pNewChnksBegIndex = 0)
	{
		SP_ASSERTR(n > 0, S_INVALIDPARAM);

		CheckChunksPtr(n);
		if (nChunks >= n)
		{
			if (IS_VALID_PTR(pNewChnksBegIndex))
				*pNewChnksBegIndex = MAX_UINT32;

			return S_SUCCESS;
		}

		// add missing new chunks
		Chunk** pOldChunks = pChunks;
		pChunks = new Chunk*[n];
		memcpy(pChunks, pOldChunks, sizeof(Chunk*) * nChunks);
		for (unsigned int iNewChnk = nChunks; iNewChnk < n; ++iNewChnk)
			pChunks[iNewChnk] = new Chunk();

		if (IS_VALID_PTR(pNewChnksBegIndex))
			*pNewChnksBegIndex = nChunks;

		nChunks = n;

		delete[] pOldChunks;
		return S_SUCCESS;
	}







	// Arguments:
	//	pPtrOut - Ptr of a Ptr-To-Object to access outside of this pool. If add failed, 0 is assigned to it.
	//	specification - the specification, the new object should have
	ILINE SResult AddItem(T** pPtrOut, const char* specification = 0, void** pChnkPtr = 0)
	{
		CheckChunksPtr();

		// find chunk with empty slot
		Chunk* pGoodChnk = 0;
		for (unsigned int iChunk = 0; iChunk < nChunks; ++iChunk)
		{
			Chunk* pChnk = pChunks[iChunk];
			if (pChnk->nSlots - pChnk->nUsedSlots > 0)
			{
				pGoodChnk = pChnk;
				break;
			}
		}

		if (!IS_VALID_PTR(pGoodChnk))
		{
			// add a new chunk if necessary
			unsigned int iGoodChnkIndex = MAX_UINT32;
			CheckMinChunkCount(nChunks + 1, &iGoodChnkIndex);
			SP_ASSERTR(iGoodChnkIndex < nChunks, S_ERROR);

			pGoodChnk = pChunks[iGoodChnkIndex];
		}

		// now find and fill the empty slot
		SP_ASSERTR(pGoodChnk, S_ERROR);

		bool bFilled = false;
		for (unsigned int iSlot = 0; iSlot < pGoodChnk->nSlots; ++iSlot)
		{
			ChunkSlot<T>* pChunkSlot = &pGoodChnk->pSlots[iSlot];
			if (!pChunkSlot->bUsed)
			{
				// found, now fill
				pChunkSlot->instance = T();
				pChunkSlot->bUsed = true;
				pChunkSlot->specification = 0;
				if (IS_VALID_PTR(specification))
				{
					unsigned int specLen = strlen(specification);
					pChunkSlot->specification = new char[specLen + 1];	// +1 for terminating zero
					memcpy((void*)pChunkSlot->specification, (void*)specification, sizeof(char) * specLen);
					pChunkSlot->specification[specLen] = 0;
				}

				if (IS_VALID_PTR(pPtrOut))
					*pPtrOut = &pChunkSlot->instance;

				pGoodChnk->nUsedSlots++;

				bFilled = true;
				break;
			}
		}

		if (!bFilled && IS_VALID_PTR(pPtrOut))
		{
			*pPtrOut = 0;
			return S_ERROR;
		}

		return S_SUCCESS;
	}







	// Return chunk index where the instance, t points to, is stored in. MAX_UINT32 when not found or not initialized
	ILINE unsigned int GetPtrChunk(const T* t) const
	{
		if (nChunks == 0 || !IS_VALID_PTR(pChunks))
			return MAX_UINT32;

		for (unsigned int i = 0; i < nChunks; ++i)
		{
			if ((unsigned int)t >= (unsigned int)pChunks[i]
				&& (unsigned int)t <= ((unsigned int)pChunks[i] + sizeof(Chunk)))
			{
				return i;
			}
		}

		return MAX_UINT32;
	}




	// returns nullptr if not found.
	ILINE ChunkSlot<T>* GetSlot(const T* t) const
	{
		if (!IS_VALID_PTR(t))
			return nullptr;

		unsigned int iChnk = GetPtrChunk(t);
		if (iChnk == MAX_UINT32)
			return nullptr;

		Chunk* pChnk = pChunks[iChnk];
		if (pChnk->nUsedSlots == 0)
			return nullptr;

		for (unsigned int i = 0; i < pChnk->nSlots; ++i)
		{
			ChunkSlot<T>* pSlot = &pChnk->pSlots[i];
			if (!pSlot->bUsed)
				continue;

			if (&pSlot->instance == t)
				return pSlot;
		}

		return nullptr;
	}






	// Warning: This function is not performant for realtime usage
	// If multiple instances share same specification, then the first occurence is taken. To get
	// all instances with given specification use GetBySpecificationArr instead.
	// If nothing matched given specification, nullptr is returned.
	ILINE T* GetBySpecification(const SString& spec, bool bCaseSensitive = true)
	{
		if (nChunks == 0 || !IS_VALID_PTR(pChunks))
			return nullptr;

		for (unsigned int iChnk = 0; iChnk < nChunks; ++iChnk)
		{
			Chunk* pChnk = pChunks[iChnk];
			if (pChnk->nUsedSlots == 0)
				continue;

			for (unsigned int iSlot = 0; iSlot < pChnk->nSlots; ++iSlot)
			{
				if (!pChnk->pSlots[iSlot].bUsed)
					continue;

				if (IS_VALID_PTR(pChnk->pSlots[iSlot].specification))
				{
					bool cmp = (bCaseSensitive ? StrCmp(spec, pChnk->pSlots[iSlot].specification)
						: StrCmpCI(spec, pChnk->pSlots[iSlot].specification));

					if (cmp)
						return &pChnk->pSlots[iSlot].instance;
				}
			}
		}

		return nullptr;
	}

	// If nothing was found, *pResult is set to nullptr and 0 is returned.
	// Remember to delete the result array which is created by this function.
	ILINE unsigned int GetBySpecificationArr(const SString& spec, InstancePtr** pResults, bool bCaseSensitive = true)
	{
		if (nChunks == 0 || !IS_VALID_PTR(pChunks))
			return nullptr;

		assert(IS_VALID_PTR(pResults));

		InstancePtr* pTmpRes = new InstancePtr[nChunks * chunkSz];
		unsigned int tmpSlot = 0;

		for (unsigned int iChnk = 0; iChnk < nChunks; ++iChnk)
		{
			Chunk* pChnk = pChunks[iChnk];
			if (pChnk->nUsedSlots == 0)
				continue;

			for (unsigned int iSlot = 0; iSlot < pChnk->nSlots; ++iSlot)
			{
				if (!pChnk->pSlots[iSlot].bUsed)
					continue;

				if (IS_VALID_PTR(pChnk->pSlots[iSlot].specification))
				{
					bool cmp = (bCaseSensitive ? StrCmp(spec, pChnk->pSlots[iSlot].specification)
						: StrCmpCI(spec, pChnk->pSlots[iSlot].specification));

					if (cmp)
					{
						pTmpRes[tmpSlot] = &pChnk->pSlots[iSlot].instance;
						++tmpSlot;
					}
				}
			}
		}

		if (tmpSlot == 0)
		{
			*pResults = nullptr;
			return 0;
		}

		*pResults = new InstancePtr[tmpSlot];
		memcpy(*pResults, pTmpRes, sizeof(InstancePtr) * tmpSlot);
		delete[] pTmpRes;

		return tmpSlot;
	}






	// The pointer t gets set to nullptr when deletion succeeded
	ILINE SResult Delete(T** t)
	{
		SP_ASSERTR(IS_VALID_PTR(t), S_INVALIDPARAM);
		if (!IS_VALID_PTR(t) || !IS_VALID_PTR(*t))
			return S_ERROR; // not found

		unsigned int iTChunk = GetPtrChunk(*t);
		if (iTChunk < nChunks)
			return S_ERROR; // not found		

		Chunk* pChnk = pChunks[iTChunk];
		for (unsigned int iSlot = 0; iSlot < pChnk->nSlots; ++iSlot)
		{
			ChunkSlot<T>* pSlot = &pChnk->pSlots[iSlot];
			if (pSlot->bUsed && &pSlot->instance == *t)
			{
				pSlot->instance = T();	// reset to defaults
				if (IS_VALID_PTR(pSlot->specification))
					delete[] pSlot->specification;

				pSlot->specification = 0;
				pSlot->bUsed = false;
				pChnk->nUsedSlots--;
				*t = nullptr;
				return S_SUCCESS;
			}
		}

		return S_ERROR;
	}

	ILINE static unsigned int SizeOfSlotStruct()
	{
		return sizeof(ChunkSlot<T>);
	}


	ILINE void Clear()
	{
		if (nChunks == 0 || !IS_VALID_PTR(pChunks))
			return;

		for (unsigned int i = 0; i < nChunks; ++i)
		{
			if (IS_VALID_PTR(pChunks[i]))
				delete pChunks[i];
		}

		delete[] pChunks;
		pChunks = 0;
		nChunks = 0;
	}

	ILINE Chunk** GetChunks() const
	{
		return pChunks;
	}

	ILINE unsigned int GetChunkCount() const
	{
		return nChunks;
	}

	ILINE unsigned int GetAvailSlotCount() const
	{
		assert(IS_VALID_PTR(pChunks));
		if (nChunks == 0)
			return 0;

		unsigned int res = 0;
		for (unsigned int iChnk = 0; iChnk < nChunks; ++iChnk)
			res += pChunks[iChnk]->nSlots;

		return res;
	}

	ILINE unsigned int GetFreeSlotCount() const
	{
		assert(IS_VALID_PTR(pChunks));
		if (nChunks == 0)
			return 0;

		unsigned int res = 0;
		for (unsigned int iChnk = 0; iChnk < nChunks; ++iChnk)
			res += pChunks[iChnk]->nSlots - pChunks[iChnk]->nUsedSlots;

		return res;
	}

	// This function is a bit more performant than running GetAvailSlotCount() - GetFreeSlotCount()
	// as it avoid two loops.
	ILINE unsigned int GetUsedSlotCount() const
	{
		assert(IS_VALID_PTR(pChunks));
		if (nChunks == 0)
			return 0;

		unsigned int res = 0;
		for (unsigned int iChnk = 0; iChnk < nChunks; ++iChnk)
			res += pChunks[iChnk]->nUsedSlots;

		return res;
	}

	// Returns true if this pool has at least one used slot. This functiion is slightly more performant
	// than using GetUsedSlotCount() as it breaks immediately when a used slot is found.
	ILINE bool HasElements() const
	{
		assert(IS_VALID_PTR(pChunks));
		if (nChunks == 0)
			return false;

		for (unsigned int iChnk = 0; iChnk < nChunks; ++iChnk)
			if (pChunks[iChnk]->nUsedSlots > 0) return true;

		return false;
	}


	ILINE SResult ForEach(IForEachHandler<T>* pHandler)
	{
		if (!IS_VALID_PTR(pHandler))
			return S_INVALIDPARAM;

		assert(IS_VALID_PTR(pChunks));

		if (nChunks == 0)
			return S_SUCCESS;

		for (unsigned int iChnk = 0; iChnk < nChunks; ++iChnk)
		{
			Chunk* pChnk = pChunks[iChnk];
			if (pChnk->nUsedSlots == 0) continue;

			for (unsigned int iSlot = 0; iSlot < pChnk->nSlots; ++iSlot)
			{
				ChunkSlot<T>* pSlot = pChnk->pSlots[iSlot];
				if (!pSlot->bUsed) continue;

				EForEachHandlerState state = pHandler->Handle(&pSlot->instance);
				if (state == FOREACH_BREAK)
					return S_SUCCESS;
				else if (state == FOREACH_ABORT_ERR)
					return S_ERROR;
			}
		}

		return S_SUCCESS;
	}
};



SP_NMSPACE_END
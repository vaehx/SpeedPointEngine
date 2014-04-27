// ********************************************************************************************

//	SpeedPoint indirection based Pool

//	This file is part of the SpeedPoint Game Engine!

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include "SPoolIndex.h"
#include <new>

namespace SpeedPoint
{

#define __NULL(x) (x == NULL)
#define __ISSET(x) (x != NULL)
#define __TRIVIAL -1

	// The SpeedPoint Pool / Indirection-Array
	template <class SPoolType_>
	class S_API SPool
	{
	private:
		int*		pIndirections;	// Buffer of indirections
		int*		pFree;		// Buffer of free indices
		int		nSize;		// Count of call Slots
		int		nFrees;		// Count of free Slots

	public:
		// The Object structure
		struct SPoolObject
		{
			int		iVersion;
			int		iIndirectionIndex;
			SPoolType_	tInstance;

			SPoolObject()
				: iIndirectionIndex(__TRIVIAL),
				iVersion(0),
				tInstance(SPoolType_()) {};

			SPoolObject(int uid, int version)
				: iIndirectionIndex(uid),
				iVersion(version),
				tInstance(SPoolType_()) {};

			SPoolObject(const SPoolType_& obj, int uid, int version)
				: iIndirectionIndex(uid),
				iVersion(version),
				tInstance(obj) {};

			~SPoolObject()
			{
				iVersion = 0;
				iIndirectionIndex = __TRIVIAL;
				tInstance.~SPoolType_();
			}
		} *pMainList;

		// Default constructor
		SPool()
			: nSize(0),
			nFrees(0),
			pFree(NULL),
			pIndirections(NULL),
			pMainList(NULL) {};

		// Constructor with given initialization size
		SPool(int nSz);

		// Destructor
		~SPool()
		{			
			Clear(); // Make sure the pool is cleared
		}

		// Pre-Allocate Memory with given size.
		// Returns pointer to first item
		SPoolType_* Allocate(int Sz);

		// Resize Memory with given size
		// Returns pointer to first item
		SPoolType_* Resize(int nNewSize);

		// Get the pointer to an item with given ArrayIndex
		SPoolType_* GetItemByUID(SPoolIndex iIndex);

		// Get the pointer to an item with given direct index
		SPoolType_* GetItemByIndirectionIndex(UINT iIndex);

		// Get the actual Pool object by given direct index
		SPoolObject* GetPoolObjectByIndirectionIndex(UINT iIndex);

		// Adds a new item to the array
		// Returns pointer to this item
		SPoolType_* AddItem(const SPoolType_& tItem, SPoolIndex* pIndex);

		// Deletes an item from the array
		// Returns pointer to first item
		SPoolType_* DeleteItem(SPoolIndex iIndex);

		// Returns count of whole pool (including free items)
		// To get actual count use GetUsageSize()
		int GetSize(void);

		// Return the count of used slots
		unsigned int GetUsageSize();

		// Returns the count of free items
		int GetFreeCount(void);

		// Clears all items and sets size to 0
		void Clear(void);

		// Resets sizes without de-allocation
		// Warning! May damage your data!
		void ResetCounts(void);

	};

	// ******************************************************************************************

	// Constructor with given initial size
	template<class T> SPool<T>::SPool(int nSz)
	{
		if (nSz > 0)
		{
			pMainList = (SPoolObject*)malloc(nSz * sizeof(SPoolObject));
			pIndirections = (int*)malloc(nSz * sizeof(int));
			pFree = (int*)malloc(nSz * sizeof(int));

			// fill new memory
			for (int i = 0; i < nSize; i++)
			{
				//pMainList[i]		= SPoolObject();	// call default constructor				
				pIndirections[i] = __TRIVIAL;		// Set as dead
				pFree[i] = nSize - i - 1;		// 
			}
		}
		else
		{
			pMainList = NULL;
			pIndirections = NULL;
			pFree = NULL;
		}
		nSize = nSz;
		nFrees = nSize;
	}

	// ******************************************************************************************
	
	// Reset sizes
	template <class T> void SPool<T>::ResetCounts(void)
	{
		nFrees = 0;
		for (int i = 0; i < nSize; i++)
			pFree[i] = nSize - i - 1;
	}

	// ******************************************************************************************
	
	// Allocate memory now.			
	template <class T> T* SPool<T>::Allocate(int nSz)
	{
		if (__ISSET(pMainList))
			free(pMainList);

		if (__ISSET(pIndirections))
			free(pIndirections);

		if (__ISSET(pFree))
			free(pFree);

		if (nSz > 0)
		{
			pMainList = (SPoolObject*)malloc(nSz * sizeof(SPoolObject));
			pIndirections = (int*)malloc(nSz * sizeof(int));
			pFree = (int*)malloc(nSz * sizeof(int));
			nSize = nSz;
			nFrees = nSz;
			// fill new memory
			for (int i = 0; i < nSize; i++)
			{
				//pMainList[i]		= SPoolObject();	// call default constructor
				pIndirections[i] = __TRIVIAL;		// Set as dead
				pFree[i] = nSize - i - 1;		// 
			}
			return &pMainList[0].tInstance;
		}
		else
		{
			nSize = 0;
			nFrees = 0;
			pMainList = NULL;
			pIndirections = NULL;
			pFree = NULL;
		}
		return NULL;
	}

	// ******************************************************************************************
	
	// Resize memory
	template <class T> T* SPool<T>::Resize(int nNewSize)
	{
		// stays
		if (nNewSize == nSize)
			return &pMainList[0].tInstance;

		// call destructors before shrink
		if (nNewSize < nSize)
		{
			for (int i = nNewSize; i < nSize; i++)
			{
				if (pMainList[i].iIndirectionIndex > __TRIVIAL)
					pMainList[i].~SPoolObject();
			}
		}

		// Resize Main List
		SPoolObject* pTmp = (SPoolObject*)realloc(pMainList, nNewSize * sizeof(SPoolObject));
		if (__NULL(pTmp))
			return NULL;

		pMainList = pTmp;

		// Resize Indirection list
		int* pTmpIndirections = (int*)realloc((void*)pIndirections, nNewSize * sizeof(int));
		if (__NULL(pTmpIndirections)) return NULL;
		pIndirections = pTmpIndirections;

		// Resize free list
		int* pTmpFree = (int*)realloc((void*)pFree, nNewSize * sizeof(int));
		if (__NULL(pTmpFree)) return NULL;
		pFree = pTmpFree;

		if (nNewSize > nSize)	// enlarge
		{
			// fill new memory
			for (int i = nSize; i < nNewSize; i++)
			{
				//pMainList[i]		= SPoolObject();
				pIndirections[i] = __TRIVIAL;
				pFree[nFrees] = i;
				nFrees++;
			}
		}
		else if (nNewSize < nSize)  // shrink
		{
			// we need to update frees here
			nFrees = 0;
			for (int i = 0; i < nNewSize; i++)
			{
				if (pIndirections[i] == __TRIVIAL)
				{
					pFree[nFrees] = i;
					nFrees++;
				}
			}
		}
		nSize = nNewSize;
		return &pMainList[0].tInstance;
	}

	// ******************************************************************************************
	
	// Get an item by given id
	template <class T> T* SPool<T>::GetItemByUID(SPoolIndex iIndex)
	{
		if (__ISSET(pMainList) && __ISSET(pIndirections) && iIndex.iIndex < nSize)
		{
			int i = pIndirections[iIndex.iIndex];
			if (i != __TRIVIAL && pMainList[i].iVersion == iIndex.iVersion)
				return &pMainList[i].tInstance;
			else
				return NULL;
		}

		return NULL;
	}

	// ******************************************************************************************
	
	// Get an item by given direct id
	template <class T> T* SPool<T>::GetItemByIndirectionIndex(UINT iIndex)
	{
		if (__ISSET(pMainList) && __ISSET(pIndirections) && nSize > 0 && (int)iIndex < nSize
			&& pIndirections[(int)iIndex] != __TRIVIAL)
		{
			return &pMainList[pIndirections[iIndex]].tInstance;
		}

		return NULL;
	}
	
	// ******************************************************************************************

	// Get an actual Pool Object by given direct id
	template <class T> typename SPool<T>::SPoolObject* SPool<T>::GetPoolObjectByIndirectionIndex(UINT iIndex)
	{
		if (__ISSET(pMainList) && __ISSET(pIndirections) && nSize > 0 && (int)iIndex < nSize
			&& pIndirections[(int)iIndex] != __TRIVIAL)
		{
			return &pMainList[pIndirections[iIndex]];
		}

		return NULL;
	}

	// ******************************************************************************************
	
	// Add an item to list (push_back) and return pointer to added item
	template <class T> T* SPool<T>::AddItem(const T& tItem, SPoolIndex* pIndex)
	{
		if (!(__ISSET(pMainList) && __ISSET(pIndirections) && __ISSET(pFree) && nFrees > 0 && nSize > 0))
		{
			Resize(nSize + 1);
		}

		int i = pFree[nFrees - 1];
		UINT nReservedSize = nSize - nFrees;
		new (&pMainList[nReservedSize]) SPoolObject();
		pMainList[nReservedSize].iVersion++;
		pMainList[nReservedSize].tInstance = tItem;
		pMainList[nReservedSize].iIndirectionIndex = i;

		pIndirections[i] = nReservedSize;

		if (__ISSET(pIndex))
		{
			pIndex->iIndex = i;
			pIndex->iVersion = pMainList[nReservedSize].iVersion;
		}

		// Set free dead
		pFree[nFrees - 1] = __TRIVIAL;
		--nFrees;
		return &pMainList[nReservedSize].tInstance;
	}

	// ******************************************************************************************
	
	// Delete an item from array
	template <class T> T* SPool<T>::DeleteItem(SPoolIndex iIndex)
	{
		if (__ISSET(pMainList) && __ISSET(pIndirections) && __ISSET(pFree))
		{
			int i = pIndirections[iIndex.iIndex];
			if (pMainList[i].iVersion == iIndex.iVersion)
			{
				// now swap
				pIndirections[pMainList[(nSize - nFrees) - 1].iIndirectionIndex] = i;
				pMainList[i] = pMainList[(nSize - nFrees) - 1];

				// set Indirection dead
				pIndirections[iIndex.iIndex] = __TRIVIAL;

				// ... and pop
				pFree[nFrees] = iIndex.iIndex;
				nFrees++;
				return &pMainList[0].tInstance;
			}
		}
		return NULL;
	}

	// ******************************************************************************************

	// Get the size the whole memory (including reserved and free cells)
	// To get the actual size of reserved slots, subtract by GetFreeCount()
	template <class T> int SPool<T>::GetSize(void)
	{
		return nSize;
	}

	// ******************************************************************************************

	// Get the count of free cells
	template <class T> int SPool<T>::GetFreeCount(void)
	{
		return nFrees;
	}
	
	// ******************************************************************************************

	// Get count of used items
	template <class T> unsigned int SPool<T>::GetUsageSize()
	{
		return (nSize - nFrees);
	}

	// ******************************************************************************************

	// Clear Pool ( allocate with size 0 )	
	template <class T> void SPool<T>::Clear(void)
	{
		// destruct everything
		if (nSize > 0)
		{
			for (int i = 0; i < nSize; i++)
			{
				if (pMainList[i].iIndirectionIndex > __TRIVIAL)
					pMainList[i].~SPoolObject();
			}
		}
		Allocate(0);
	}
}
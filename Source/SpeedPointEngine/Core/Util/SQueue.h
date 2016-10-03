//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine Source File
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2014, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "SAPI.h"
#include <SPrerequisites.h>

namespace SpeedPoint
{
	//////////////////////////////////////////////////////////////////////////////////////////////////

	// Lock type
	enum S_API Lock
	{
		eLOCK_NONE = 0x0,
		eLOCK_PRESERVE = 0x01
	};

	// SQueue template
	template<class T>
	class S_API SQueue
	{
	public:
		// Slot item
		struct Slot
		{
			T*	pInstance;
			bool	bIsCopy;

			// default constructor
			Slot()
				: pInstance(0),
				bIsCopy(false)
			{
			}
		};

		// --

		SQueue()
			: m_pSlots(0),
			m_Ref(0),
			m_Length(0),
			m_bLocked(false),
			m_LockingFlags(0),
			m_FirstSlot(0),
			m_LastSlot(0)
		{
		}

		// constructor with approximated size
		SQueue(unsigned int approxSize);

		// default destructor
		~SQueue();

		// pop slot from the current front	
		bool PopSlot(Slot** pSlot);

		// pop ptr to the instance from current front slot
		bool Pop(T** pOut);

		// Summary:
		//	push copy of instance and return Slot ptr to slot holding it
		// Description:
		//	SQueue will handle construction and destruction of instance
		Slot* Push(const T& obj);

		// Summary:
		//	push ptr of instance and return slot ptr to slot holding it
		// Description:
		//	SQueue will NOT handle construction or destruction of instance
		//	SQueue has to be locked, if not assertion exception will be thrown
		// Arguments:
		//	bIsCopy - set to false, if SQueue must not handle destruction, set to true to let it handle resp.
		// Return Value:
		//	Returns 0 if ptr to instance is 0	
		Slot* Push(T* pInstance, bool bIsCopy = false);

		// Summary:
		//	push a new instance of the template type
		// Description:
		//	SQueue will handle construction and destruction of instance
		// Return value:
		//	
		Slot* Push();

		// Recalc length of the slot array
		unsigned int& RecalcLength() const;

		// Summary:
		//	Tries to lock the SQueue
		// Return value:
		//	Returns true if succeeded, false if SQueue already locked
		bool Lock(unsigned int lockFlags = eLOCK_PRESERVE);

		// Summary:
		//	Unlocks the SQueue
		// Return value:
		//	false if not locked
		bool Unlock();

		// Summary:
		//	Resets the iteration reference index
		// Description:
		//	If using eLOCK_PRESERVE for lock, the current index will not be resetted
		//	automatically until using unlock.
		bool ResetIterator();

		// Summary:
		//	Clears the buffer
		// Description:
		//	Will also destruct copied and created instances
		// Return Value:
		//	Returns false if SQueue is locked
		bool Clear();

		// Summary:
		//	Checks if popping reached its end
		// Return Value:
		//	Returns true if there is still something in the queue to be popped or
		//	reference is not equal index of last slot yet
		bool ReachedEnd();

		// Summary:
		//	Calculate and return the amount of used slots
		// Return Value:
		//	Returns amount of used slots
		unsigned int GetUsageCount();

	private:
		// Summary:
		//	Resize SQueue slot buffer
		// Description:
		//	Resize is declared private because it is only meant to be called by internal functions
		//	in order to bypass the lock
		bool Resize(unsigned int nNewSize);

		unsigned int	m_Ref;		// the reference pointing to the current front item if buffer locked with eLOCK_PRESERVE
		unsigned int	m_Length;	// (max) current length of the slot array

		// Index of the first slot
		// This index starts at 1!
		// If the first slot index equals the last slot index and both are 0, then there is nothing in the SQueue
		// If the first slot index equals the last slot index and both are not 0, then there is only one used slot in the SQueue
		unsigned int	m_FirstSlot;

		// Index of the last slot
		// This index starts at 1!
		// If the first slot index equals the last slot index and both are 0, then there is nothing in the SQueue
		// If the first slot index equals the last slot index and both are not 0, then there is only one used slot in the SQueue
		unsigned int	m_LastSlot;

		unsigned char*	m_pRealBuf;	// unaligned buffer start ptr
		Slot*		m_pSlots;	// slot array
		bool		m_bLocked;	// locking state
		unsigned int	m_LockingFlags;	// locking flags
	}; // SQueue

	//////////////////////////////////////////////////////////////////////////////////////////////////

	template<class T> SQueue<T>::SQueue(unsigned int approxSize)
	{
		assert(approxSize > 1, "Approximated size has to be at least 2!");

		unsigned int nAllocSize = approxSize * sizeof(Slot)+4; // +4 for 32bit alignment
		m_pRealBuf = new unsigned char[nAllocSize];

		// now align
		unsigned char* pAlign = m_pRealBuf;

		// Disabled manual alignment due to modern hardware doing this on its own
		/*
		while ((unsigned int)pAlign & 0x3)
		++pAlign;
		*/

		m_pSlots = (Slot*)pAlign;

		m_Ref = 0;

		// set both limit indices to 0, which indicates that there is nothing in the SQueue
		m_FirstSlot = 0;
		m_LastSlot = 0;

		m_Length = approxSize;
		m_bLocked = false;
		m_LockingFlags = 0;
	}

	template<class T> SQueue<T>::~SQueue()
	{
		assert_trace(!m_bLocked, "Attempted destruction of locked SQueue!");

		Clear();
	}

	template<class T> inline bool SQueue<T>::PopSlot(typename SQueue<T>::Slot** pSlot)
	{
		assert(m_pSlots);
		assert(m_FirstSlot > 0 && m_Ref > 0 && m_LastSlot > 0, "Attempted Pop from empty SQueue!");
		assert_trace((m_Ref - 1) < m_Length, "ref=%d, len=%d", m_Ref, m_Length);

		// check output slot
		if (pSlot && !(*pSlot))
			(*pSlot) = new Slot();

		// destroy if necessary
		if (!m_bLocked || (m_bLocked && !(m_LockingFlags & eLOCK_PRESERVE)))
		{
			Slot* ppSlot = &m_pSlots[m_FirstSlot - 1];
			if (pSlot)
			{
				(*pSlot)->pInstance = ppSlot->pInstance;
				(*pSlot)->bIsCopy = ppSlot->bIsCopy;
			}

			// destroy slot instance and reset indices
			if (ppSlot->pInstance && ppSlot->bIsCopy)
			{
				delete ppSlot->pInstance;
				ppSlot->pInstance = 0;
			}

			// [1, 2, 3, 4, x, x],1 -> [x, 1, 2, 3, x, x],2
			// [2, 3, 4, x, x, 1],6 -> [1, 2, 3, x, x, x],1
			if (m_FirstSlot != m_LastSlot)
			{
				m_FirstSlot = (m_FirstSlot % m_Length) + 1;
			}
			else
			{
				// popped all slots
				m_FirstSlot = 0;
				m_LastSlot = 0;
			}

			m_Ref = m_FirstSlot;
		}
		else if (m_bLocked)	// optional condition
		{
			++m_Ref;
		}

		return true;
	}

	template<class T> inline bool SQueue<T>::Pop(T** pOut)
	{
		assert(m_pSlots);
		assert(m_FirstSlot > 0 && m_Ref > 0 && m_LastSlot > 0, "Attempted Pop from empty SQueue!");
		assert_trace((m_Ref - 1) < m_Length, "ref=%d, len=%d", m_Ref, m_Length);

		// destroy if necessary	
		if (!m_bLocked || (m_bLocked && !(m_LockingFlags & eLOCK_PRESERVE)))
		{
			Slot* ppSlot = &m_pSlots[m_FirstSlot - 1];

			// destroy slot instance and reset indices	
			if (!pOut && ppSlot->bIsCopy)
				delete ppSlot->pInstance;
			else
				*pOut = ppSlot->pInstance;

			// reset slot instance ptr, just to be sure
			ppSlot->pInstance = 0;

			if (m_FirstSlot != m_LastSlot)
			{
				m_FirstSlot = (m_FirstSlot % m_Length) + 1;
			}
			else
			{
				// popped all slots
				m_FirstSlot = 0;
				m_LastSlot = 0;
			}

			m_Ref = m_FirstSlot;
		}
		else if (m_bLocked)	// optional condition
		{
			++m_Ref;
		}

		return true;
	}

	// T needs to have a copy constructor!
	template<class T> inline typename SQueue<T>::Slot* SQueue<T>::Push(const T& obj)
	{
		assert(m_pSlots);

		T* pInstance = new T(obj);
		return Push(pInstance, true);

		// remember not to remove the instance here, as it is stored in the slot
	}

	template<class T> inline typename SQueue<T>::Slot* SQueue<T>::Push(T* pInstance, bool bIsCopy)
	{
		assert(m_pSlots);
		assert(pInstance, "Tried to push zero ptr as instance to SQueue!");

		// check size
		unsigned int usageSize = 0;
		if (m_FirstSlot > m_LastSlot)
			usageSize = (m_FirstSlot + 1) + m_LastSlot;
		else
			usageSize = (m_LastSlot + 1) - m_FirstSlot; // 0 if equal

		assert_trace(usageSize <= m_Length, "firstSlot=%d len=%d usageSize=%d", m_FirstSlot, m_Length, usageSize); // check for possible error to avoid memory leaks

		// resize if necessary
		if (usageSize == m_Length)
		{
			// add 10 slots, we don't have to worry about those 100 bytes, as we use ptrs for instances in Slots
			unsigned int prevLength = m_Length;
			bool bResizeRes = Resize(m_Length + 10);
			assert_trace(bResizeRes, "nNewLength=%d", prevLength + 10);
		}

		// we do have a free slot now
		// [4, 5, x, x, 1, 2, 3] or [1, 2, 3, 4, 5, x] or [x, 1, 2, 3, 4] or [x, x, x, x, x]
		m_LastSlot = (m_LastSlot % m_Length) + 1;
		Slot* pSlot = &m_pSlots[m_LastSlot - 1];
		pSlot->bIsCopy = bIsCopy;
		pSlot->pInstance = pInstance;

		// Fix first slot index
		// before: [x, x, x, x, x] -> FirstSlot = 0, LastSlot = 0 
		// after: [1, x, x, x, x] -> FirstSlot = 1, LastSlot = 1	
		if (m_FirstSlot == 0 && m_LastSlot == 1)
			m_FirstSlot = 1;

		// fix reference index
		if (!m_Ref) m_Ref = m_FirstSlot;

		return pSlot;
	}

	template<class T> inline typename SQueue<T>::Slot* SQueue<T>::Push()
	{
		assert(m_pSlots);
		assert_trace(!m_bLocked, "locked=%d", m_bLocked);

		T* pInstance = new T();
		return Push(pInstance, true);
	}

	template<class T> inline unsigned int& SQueue<T>::RecalcLength() const
	{
		assert(m_pSlots);
		return (m_Length = (sizeof(m_pSlots) // warning: maybe this is not correct!!!!!
			/ sizeof(Slot)));
	}

	template<class T> inline bool SQueue<T>::Lock(unsigned int lockFlags)
	{
		if (m_bLocked)
			return false;

		// lock with specified flags
		m_bLocked = true;
		m_LockingFlags = lockFlags;
		return true;
	}

	template<class T> inline bool SQueue<T>::Unlock()
	{
		if (!m_bLocked)
			return false;

		m_bLocked = false;
		m_LockingFlags &= 0;
		m_Ref = m_FirstSlot;
		return true;
	}

	template<class T> inline bool SQueue<T>::Resize(unsigned int nNewSize)
	{
		assert(m_pSlots);
		assert_trace(nNewSize > 1, "Size of SQueue has to be at least 2! newSize=%d", nNewSize);
		assert_trace(nNewSize >= m_Length, "You cannot shrink SQueues! curLen=%d newLen=%d", m_Length, nNewSize);
		assert_trace(!(m_FirstSlot == 0 && m_FirstSlot != m_LastSlot), "SQueue leak detected! firstSlot=%d, lastSlot=%d", m_FirstSlot, m_LastSlot);
		// bypass lock here

		if (nNewSize == m_Length)
			return true;

		// first, create new aligned buffer of specified size
		// + 4 bytes for 32bit alignment
		unsigned int nAllocSize = (nNewSize * sizeof(Slot)) + 4;
		unsigned char* pNewBuf = new unsigned char[nAllocSize];
		unsigned char* pNewOutBuf = pNewBuf;

		// Disabled manual alignment due to modern hardware doing this on its own
		/*
		while ((unsigned int)pNewOutBuf & 0x3)
		++pNewOutBuf;
		*/

		// now copy the memory to the brand new buffer
		if (m_FirstSlot != 0) // m_FirstSlot = 0 -> [x, x, x]
		{
			// boundaries slot indices are +1, that's why turn them back
			--m_FirstSlot;
			--m_LastSlot;

			if (m_FirstSlot <= m_LastSlot)
			{
				// [1, 2, 3, 4, x, x, x] -> [1, 2, 3, 4, x, x, x, x, x, x]
				// [x, 1, 2, 3, 4, x, x] -> [x, 1, 2, 3, 4, x, x, x, x, x]
				memcpy((unsigned char*)pNewOutBuf + m_FirstSlot,
					(unsigned char*)(m_pSlots + m_FirstSlot),
					((m_LastSlot - m_FirstSlot) + 1) * sizeof(Slot));
			}
			else if (m_FirstSlot > m_LastSlot)
			{
				// [4, 5, 6, x, 1, 2, 3] -> [4, 5, 6, x, x, x, x, 1, 2, 3]
				memcpy(pNewOutBuf,
					(unsigned char*)m_pSlots,
					(m_LastSlot + 1) * sizeof(Slot));	// +1 due to m_LastSlot being an index

				memcpy(pNewOutBuf + (nNewSize - (m_Length - m_FirstSlot)),
					(unsigned char*)m_pSlots + m_FirstSlot,
					(m_Length - m_FirstSlot) * sizeof(Slot));

				// we have to reset the first index in this case
				m_FirstSlot = (nNewSize - (m_Length - m_FirstSlot));
			}

			++m_FirstSlot;
			++m_LastSlot;
		}

		// delete old stuff
		delete[] m_pRealBuf;

		// and set new vars
		m_pRealBuf = pNewBuf;
		m_pSlots = (Slot*)pNewOutBuf;
		m_Length = nNewSize;

		return true;
	}

	template<class T> inline bool SQueue<T>::Clear()
	{
		if (m_bLocked) return false;

		if (m_pRealBuf) delete[] m_pRealBuf;

		m_pRealBuf = 0;
		m_pSlots = 0;
		m_Ref = 0;
		m_FirstSlot = 0;
		m_LastSlot = 0;
		m_Length = 0;
		m_LockingFlags = 0;
		return true;
	}

	template<class T> inline bool SQueue<T>::ReachedEnd()
	{
		if (m_bLocked && (m_LockingFlags & eLOCK_PRESERVE))
			return (m_Ref == m_LastSlot);

		return (GetUsageCount() == 0);
	}

	template<class T> inline unsigned int SQueue<T>::GetUsageCount()
	{
		unsigned int usageSize = 0;
		if (m_FirstSlot > m_LastSlot)
			usageSize = (m_FirstSlot + 1) + m_LastSlot;
		else
			usageSize = (m_LastSlot + 1) - m_FirstSlot; // 0 if equal

		return usageSize;
	}
}
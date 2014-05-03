//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine Source File
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2014, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <SPrerequisites.h>
#include <Util\SVector3.h>
#include <Util\SQueue.h>

namespace SpeedPoint
{
	//////////////////////////////////////////////////////////////////////////////////////////////////

	// Description of a command buffer entry (command)
	struct S_API SCommandDescription
	{
		S_RENDER_COMMAND_TYPE	commandType;
		void*			pData;

		SCommandDescription()
			: commandType(eSRCMD_NONE),
			pData(0)
		{
		}

		SCommandDescription(S_RENDER_COMMAND_TYPE type, void* data)
			: commandType(type),
			pData(data)
		{
		}

		~SCommandDescription()
		{
			// make sure to clear the data structure instance
			if (pData)
				delete pData;

			pData = 0;
		}
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////

	// Data description structur for a DRAWSOLID command buffer entry
	struct S_API SDrawSolidData
	{
		enum { cmdType = eSRCMD_DRAWSOLID };

		// Id of the solid
		SP_ID		id;

		// Geometry instance attributes:
		SVector3	vPosition;
		SVector3	vRotation;		
	};

	// Data description structur for a DRAWPRIMITIVE commmand buffer entry
	struct S_API SDrawPrimitiveData
	{
		enum { cmdType = eSRCMD_DRAWPRIMITIVE };

		// Id of the containting solid
		SP_ID		iSolid;

		// id of the primitive
		unsigned int	iPrimitive;
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////

	// Wrapper for Queue => CommandQueue
	class S_API SCommandQueue
	{
	private:
		SQueue<SCommandDescription>*	m_pQueue;

	public:		
		SCommandQueue()
			: m_pQueue(0)
		{
		}
		
		~SCommandQueue()
		{
			Clear();
		}

		// Summary:
		//	Initializes the queue with given approximated size
		// Arguments:
		//	nApproxSize - (default 100) approximated maximum queue size
		//		The queue automatically scales if no space left, but nApproxSize
		//		should avoid too many memcpy's in advance.
		// Return Value:
		//	Returns S_ABORTED if already initialized.
		SResult Init(unsigned int nApproxSize = 100)
		{
			if (m_pQueue) return S_ABORTED;
			
			m_pQueue = new SQueue<SCommandDescription>(nApproxSize);
			return S_SUCCESS;
		}

		// Summary:
		//	Pop front item from the queue
		// Arguments:
		//	pDesc - ptr to ptr to output description. is instanciated by function!		
		SResult Pop(SCommandDescription** pDesc)
		{
			if (!m_pQueue) return S_NOTINIT;

			if (m_pQueue->Pop(pDesc))
				return S_SUCCESS;
			else
				return S_ERROR;
		}

		// Summary:
		//	Locks the buffer
		// Arguments:
		//	bPrevent - (default false) set to true if you want to preserve the slot and increase ref instead when popping
		SResult Lock(bool bPrevent = false)
		{
			if (!m_pQueue) return S_NOTINIT;

			unsigned int nLockFlags = (bPrevent) ? eLOCK_PRESERVE : 0;
			if (m_pQueue->Lock(nLockFlags))
				return S_SUCCESS;
			else
				return S_ERROR;
		}

		// Summary:
		//	Unlock the buffer
		SResult Unlock()
		{
			if (!m_pQueue) return S_NOTINIT;

			if (m_pQueue->Unlock())
				return S_SUCCESS;
			else
				return S_ERROR;
		}

		// Summary:
		//	Push a new item by copy
		SResult Push(const SCommandDescription& desc)
		{
			if (!m_pQueue) return S_NOTINIT;

			if (m_pQueue->Push(desc))
				return S_SUCCESS;
			else
				return S_ERROR;
		}

		// Summary:
		//	Push a new SCommandDescription instance				
		// Arguments:
		//	pOutDesc - (default 0)sets pointer to the command description instance.
		//		Warning: Do not keep this ptr as it might get invalid after resize of the queue buffer!
		SResult Push(SCommandDescription** pOutDesc = 0)
		{
			if (!m_pQueue) return S_NOTINIT;

			SQueue<SCommandDescription>::Slot* pSlot = m_pQueue->Push();
			if (pOutDesc)			
				*pOutDesc = pSlot->pInstance;		

			return (pOutDesc) ? S_SUCCESS : S_ERROR;
		}

		// Summary:
		//	Push a new SCommandDescription instance
		// Description:
		//	same as Push(SCommandDescription**), but with direct write of the parameters
		// Arguments:
		//	pData - must be a ptr to one of the Data structs
		SResult Push(S_RENDER_COMMAND_TYPE cmdType, void* pData, SCommandDescription** pOutDesc = 0)
		{
			if (!m_pQueue) return S_NOTINIT;

			SCommandDescription* pDesc;
			if (Failure(Push(&pDesc)) || !pDesc)
				return S_ERROR;

			pDesc->commandType = cmdType;
			pDesc->pData = pData;

			if (pOutDesc)
				*pOutDesc = pDesc;

			return S_SUCCESS;
		}

		// Summary:
		//	Clears the queue and its buffer
		SResult Clear()
		{
			if (m_pQueue)
			{
				m_pQueue->Clear();
				delete m_pQueue;
				m_pQueue = 0;
			}

			return S_SUCCESS;
		}
	};
}
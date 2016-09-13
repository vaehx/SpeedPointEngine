#include <Physics\CPhysics.h>

SP_NMSPACE_BEG

S_API CPhysics::CPhysics()
	: m_pObjects(0)
{
}

S_API CPhysics::~CPhysics()
{
}

S_API void CPhysics::SetPhysObjectPool(IComponentPool<CPhysObject>* pPool)
{
	assert(pPool);
	m_pObjects = pPool;
}

S_API CPhysObject* CPhysics::CreatePhysObject(const SPhysObjectParams& params)
{
	if (IS_VALID_PTR(m_pObjects))
	{
		CPhysObject* pPhysObject = m_pObjects->Get();
		pPhysObject->Init(params);
		return pPhysObject;
	}
	else
	{
		return 0;
	}
}

S_API void CPhysics::ReleasePhysObject(CPhysObject** pObject)
{
	if (IS_VALID_PTR(m_pObjects))
	{
		if (IS_VALID_PTR(pObject) && IS_VALID_PTR(*pObject))
			(*pObject)->Release();

		m_pObjects->Release(pObject);
	}
}

S_API void CPhysics::ClearPhysObjects()
{
	if (IS_VALID_PTR(m_pObjects))
	{
		unsigned int i;
		CPhysObject* pObj = m_pObjects->GetFirst(i);
		while (pObj)
		{
			pObj->Release();
			pObj = m_pObjects->GetNext(i);
		}

		m_pObjects->ReleaseAll();
	}
}

SP_NMSPACE_END
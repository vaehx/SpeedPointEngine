#include <Physics\CPhysics.h>

SP_NMSPACE_BEG

S_API CPhysics::CPhysics()
	: m_pObjects(0)
{
}

S_API CPhysics::~CPhysics()
{
}

S_API void CPhysics::SetPhysObjectPool(IComponentPool<IPhysObject>* pPool)
{
	assert(pPool);
	m_pObjects = pPool;
}

S_API IPhysObject* CPhysics::GetPhysObjects()
{
	if (IS_VALID_PTR(m_pObjects))
		return m_pObjects->Get();
	else
		return 0;
}

S_API void CPhysics::ReleasePhysObject(IPhysObject** pObject)
{
	if (IS_VALID_PTR(m_pObjects))
	{
		if (IS_VALID_PTR(pObject) && IS_VALID_PTR(*pObject))
			(*pObject)->OnRelease();

		m_pObjects->Release(pObject);
	}
}

S_API void CPhysics::ClearPhysObjects()
{
	if (IS_VALID_PTR(m_pObjects))
	{
		unsigned int i;
		IPhysObject* pObj = m_pObjects->GetFirst(i);
		while (pObj)
		{
			pObj->OnRelease();
			pObj = m_pObjects->GetNext(i);
		}

		m_pObjects->ReleaseAll();
	}
}

SP_NMSPACE_END
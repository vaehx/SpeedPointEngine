#pragma once

#include <SPrerequisites.h>
#include <Abstract\IPhysics.h>

SP_NMSPACE_BEG

class S_API CPhysics : public IPhysics
{
private:
	IComponentPool<IPhysObject>* m_pObjects;

protected:
	virtual void SetPhysObjectPool(IComponentPool<IPhysObject>* pPool);

public:
	CPhysics();
	~CPhysics();

	ILINE virtual IPhysObject* GetPhysObjects();
	ILINE virtual void ReleasePhysObject(IPhysObject** pObject);
	ILINE virtual void ClearPhysObjects();
};

SP_NMSPACE_END
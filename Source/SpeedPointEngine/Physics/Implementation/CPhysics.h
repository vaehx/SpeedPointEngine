#pragma once

#include "..\IPhysics.h"
#include <Common\SPrerequisites.h>

SP_NMSPACE_BEG

#define RESTING_TOLERANCE 0.02f

class S_API CPhysics : public IPhysics
{
private:
	IComponentPool<PhysObject>* m_pObjects;

protected:
	virtual void SetPhysObjectPool(IComponentPool<PhysObject>* pPool);

public:
	CPhysics();
	~CPhysics();

	ILINE virtual PhysObject* CreatePhysObject();
	ILINE virtual void ClearPhysObjects();
	ILINE virtual void Update(float fTime);
};

SP_NMSPACE_END
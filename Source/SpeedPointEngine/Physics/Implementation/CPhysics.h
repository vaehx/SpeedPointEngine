#pragma once

#include "..\IPhysics.h"
#include <Common\SPrerequisites.h>

SP_NMSPACE_BEG

class S_API CPhysics : public IPhysics
{
private:
	IComponentPool<CPhysObject>* m_pObjects;

protected:
	virtual void SetPhysObjectPool(IComponentPool<CPhysObject>* pPool);

public:
	CPhysics();
	~CPhysics();

	ILINE virtual CPhysObject* CreatePhysObject(const SPhysObjectParams& params = SPhysObjectParams());
	ILINE virtual void ReleasePhysObject(CPhysObject** pObject);
	ILINE virtual void ClearPhysObjects();
	ILINE virtual void Update(double fTime);
};

SP_NMSPACE_END
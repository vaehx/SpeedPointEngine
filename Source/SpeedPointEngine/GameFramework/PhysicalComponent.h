#pragma once

#include <EntitySystem\IEntity.h>
#include <Physics\PhysObject.h>

SP_NMSPACE_BEG

class S_API CPhysicalComponent : public IComponent, public PhysObject
{
	DEFINE_COMPONENT

public:
	CPhysicalComponent();
	virtual ~CPhysicalComponent() {}

	// IComponent:
public:
	virtual void OnRelease();

	// PhysObject:
public:
	virtual void OnSimulationPrepare();
	virtual void OnSimulationFinished();
	virtual void OnIntersection(const geo::SIntersection& contact, const PhysObject* other);
};

SP_NMSPACE_END

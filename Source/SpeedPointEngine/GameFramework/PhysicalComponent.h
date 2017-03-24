#pragma once

#include <EntitySystem\IEntity.h>
#include <Physics\PhysObject.h>

SP_NMSPACE_BEG

class S_API CPhysicalComponent : public IComponent, public CPhysObject
{
	DEFINE_COMPONENT

public:
	CPhysicalComponent();
	virtual ~CPhysicalComponent() {}

	// IComponent:
public:
	
	virtual void OnEntityTransformed();
};

SP_NMSPACE_END

#pragma once

#include <Abstract\PhysObject.h>
#include <Abstract\IEntity.h>

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

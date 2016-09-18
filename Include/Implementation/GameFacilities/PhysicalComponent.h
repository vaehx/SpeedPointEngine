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
	ILINE virtual const char* GetSerializationType() const { return "Physical"; }
	ILINE virtual void Serialize(map<string, string>& params) const;
	ILINE virtual void Deserialize(const map<string, string>& params);
	
	virtual void OnEntityTransformed();
};

SP_NMSPACE_END

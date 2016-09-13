#pragma once

#include <Abstract\PhysObject.h>
#include <Abstract\IEntity.h>

SP_NMSPACE_BEG

class S_API CPhysicalComponent : public IComponent, public CPhysObject
{
private:
	bool m_bTrash;

public:
	CPhysicalComponent();
	virtual ~CPhysicalComponent() {}

	virtual void Release();
	virtual bool IsTrash() const;

	// IComponent:
public:
	virtual void OnEntityTransformed();
};

SP_NMSPACE_END

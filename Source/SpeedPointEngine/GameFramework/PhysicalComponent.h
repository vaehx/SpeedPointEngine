#pragma once

#include <EntitySystem\IEntity.h>
#include <Physics\PhysObject.h>

SP_NMSPACE_BEG

class S_API CPhysicalComponent : public IComponent, public PhysObject
{
public:
	DEFINE_COMPONENT("Physical")

	CPhysicalComponent();
	virtual ~CPhysicalComponent() {}

	const string& GetProxyGeomFile() const { return m_ProxyGeomFile; }
	void LoadProxyFromSPM(const string& proxyGeomFile);

	// IComponent:
public:
	virtual void OnRelease();
	virtual void Serialize(ISerContainer* ser, bool serialize = true);

	// PhysObject:
public:
	virtual void OnSimulationPrepare();
	virtual void OnSimulationFinished();
	virtual void OnIntersection(const geo::SIntersection& contact, const PhysObject* other);

private:
	string m_ProxyGeomFile; // abs res path
};

SP_NMSPACE_END

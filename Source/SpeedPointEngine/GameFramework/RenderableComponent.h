//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <EntitySystem\IEntity.h>
#include <3DEngine\I3DEngine.h>
#include <3DEngine\IParticleSystem.h>

SP_NMSPACE_BEG

template<class RenderObjImpl>
class S_API CRenderObjectComponent : public IComponent, public RenderObjImpl
{
protected:
	typedef typename RenderObjImpl::IsDerivedFromIRenderObject IRenderObjectGuard;

	// IComponent:
public:
	virtual void OnRelease()
	{
		RenderObjImpl::Clear();
	}

	// IProxyRenderObject:
public:
	virtual void OnRender()
	{
		if (m_pEntity)
		{
			if (SRenderDesc* pRenderDesc = RenderObjImpl::GetRenderDesc())
				pRenderDesc->transform = m_pEntity->GetTransform();
		}
	}
};

class S_API CRenderMeshComponent : public CRenderObjectComponent<CRenderMesh>
{
public:
	DEFINE_COMPONENT("RenderMesh")
	virtual void Serialize(ISerContainer* ser, bool serialize = true);
};

class S_API CRenderLightComponent : public IComponent, public CRenderLight
{
public:
	DEFINE_COMPONENT("RenderLight");

	virtual void OnRelease();
	virtual void Serialize(ISerContainer* ser, bool serialize = true);
	virtual void OnRender();
};

//////////////////////////////////////////////////////////////////////////////////////////////////

class S_API CParticleEmitterComponent : public IComponent, public CParticleEmitter
{
	// CParticleEmitter:
public:
	DEFINE_COMPONENT("ParticleEmitter");

	virtual SInstancedRenderDesc* GetRenderDesc();
	virtual void Clear();
	virtual void Serialize(ISerContainer* ser, bool serialize = true);
};


SP_NMSPACE_END

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
	DEFINE_COMPONENT

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


typedef S_API class CRenderObjectComponent<CRenderMesh> CRenderMeshComponent;
typedef S_API class CRenderObjectComponent<CRenderLight> CRenderLightComponent;

//////////////////////////////////////////////////////////////////////////////////////////////////

class S_API CParticleEmitterComponent : public IComponent, public CParticleEmitter
{
	DEFINE_COMPONENT

	// CParticleEmitter:
public:
	virtual SInstancedRenderDesc* GetRenderDesc()
	{
		SInstancedRenderDesc* renderDesc = CParticleEmitter::GetRenderDesc();
		if (m_pEntity)
			renderDesc->transform = m_pEntity->GetTransform() * renderDesc->transform;

		return renderDesc;
	}

	virtual void Clear()
	{
		CParticleEmitter::Clear();
		IComponent::Release();
	}
};


SP_NMSPACE_END

//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2016 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <EntitySystem\IEntity.h>
#include <3DEngine\I3DEngine.h>

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

SP_NMSPACE_END

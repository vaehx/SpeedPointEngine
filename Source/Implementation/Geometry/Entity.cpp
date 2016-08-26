//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2015, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	Entities.cpp
// Created:	6/24/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#include <Implementation\Geometry\Entity.h>
#include <Implementation\Geometry\RenderableComponent.h>
#include <Abstract\IPhysicalComponent.h>
#include <Abstract\IGameEngine.h>
#include <Abstract\I3DEngine.h>
#include <Abstract\IVertexBuffer.h>

SP_NMSPACE_BEG

S_API CEntity::CEntity()
	: m_bTransformInvalid(true),
	m_Scale(1.0f)
{
	for (int i = 0; i < NUM_COMPONENTS; ++i)
	{
		m_pComponents[i] = 0;
	}
}

S_API void CEntity::Clear()
{
	for (int i = 0; i < NUM_COMPONENTS; ++i)
	{
		ReleaseComponent(m_pComponents[i]);
		m_pComponents[i] = 0;
	}
}


void CEntity::OnEntityTransformEvent()
{
	SEntityEvent e;
	e.type = eENTITY_EVENT_TRANSFORM;
	e.transform.pos = m_Pos;
	e.transform.rot = m_Rot;
	e.transform.scale = m_Scale;
	e.transform.pivot = m_Pivot;

	m_bTransformInvalid = true;
	OnEntityEvent(e);
}

S_API const Vec3f& CEntity::GetPos() const
{
	return m_Pos;
}

S_API void CEntity::SetPos(const Vec3f& pos)
{	
	m_Pos = pos;
	OnEntityTransformEvent();
}

S_API void CEntity::Translate(const Vec3f& translate)
{
	SetPos(GetPos() + translate);
}

S_API const Quat& CEntity::GetRotation() const
{
	return m_Rot;
}

S_API void CEntity::SetRotation(const Quat& rotation)
{
	m_Rot = rotation;
	OnEntityTransformEvent();
}

S_API void CEntity::Rotate(const Quat& rotate)
{
	SetRotation(GetRotation() * rotate);
}

S_API const Vec3f& CEntity::GetScale() const
{
	return m_Scale;
}

S_API void CEntity::SetScale(const Vec3f& scale)
{
	m_Scale = scale;
	OnEntityTransformEvent();
}

S_API const Vec3f& CEntity::GetPivot() const
{
	return m_Pivot;
}
S_API void CEntity::SetPivot(const Vec3f& pivot)
{
	m_Pivot = pivot;
	OnEntityTransformEvent();
}

S_API const SMatrix& CEntity::GetTransform()
{
	if (m_bTransformInvalid)
	{
		STransformationDesc transform;
		transform.translation = SMatrix::MakeTranslationMatrix(m_Pos);
		transform.rotation = m_Rot.ToRotationMatrix();
		transform.scale = SMatrix::MakeScaleMatrix(m_Scale);
		transform.preRotation = SMatrix::MakeTranslationMatrix(-m_Pivot);
		m_Transform = transform.BuildTRS();
		m_bTransformInvalid = false;
	}

	return m_Transform;
}

S_API const char* CEntity::GetName() const
{
	return m_Name.c_str();
}

S_API void CEntity::SetName(const char* name)
{
	m_Name = name;
}

void CEntity::OnEntityEvent(const SEntityEvent& event)
{
	for (int i = 0; i < NUM_COMPONENTS; ++i)
	{
		if (IS_VALID_PTR(m_pComponents[i]))
			m_pComponents[i]->OnEntityEvent(event);
	}
}

S_API IComponent* CEntity::CreateComponent(EComponentType component)
{
	IEntitySystem* pEntitySystem = SpeedPointEnv::GetEngine()->GetEntitySystem();
	if (!IS_VALID_PTR(pEntitySystem))
	{
		CLog::Log(S_ERROR, "Cannot create entity component: pEntitySystem invalid!");
		return 0;
	}

	if (!IS_VALID_PTR(m_pComponents[component]))
	{
		switch (component)
		{
		case eCOMPONENT_RENDERABLE:
		{
			m_pComponents[(int)component] = pEntitySystem->CreateRenderableComponent();
			break;
		}

		case eCOMPONENT_PHYSICAL:
			m_pComponents[component] = pEntitySystem->CreatePhysicalComponent();
			break;
		}

		if (IS_VALID_PTR(m_pComponents[component]))
			m_pComponents[component]->SetEntity(this);
	}

	return m_pComponents[component];
}

S_API IRenderableComponent* CEntity::CreateRenderable()
{
	return (IRenderableComponent*)CreateComponent(eCOMPONENT_RENDERABLE);
}

S_API IPhysicalComponent* CEntity::CreatePhysical()
{
	return (IPhysicalComponent*)CreateComponent(eCOMPONENT_PHYSICAL);
}

// Returns NULL if the component was not created
S_API IComponent* CEntity::GetComponent(EComponentType component) const
{
	return m_pComponents[component];
}

S_API IRenderableComponent* CEntity::GetRenderable() const
{
	return (IRenderableComponent*)GetComponent(eCOMPONENT_RENDERABLE);
}

S_API IPhysicalComponent* CEntity::GetPhysical() const
{
	return (IPhysicalComponent*)GetComponent(eCOMPONENT_PHYSICAL);
}

S_API void CEntity::SetComponent(EComponentType type, IComponent* pComponent)
{
	m_pComponents[type] = pComponent;
}

S_API void CEntity::ReleaseComponent(IComponent* pComponent)
{
	if (!IS_VALID_PTR(pComponent))
		return;

	IEntitySystem* pEntitySystem = SpeedPointEnv::GetEngine()->GetEntitySystem();

	for (int i = 0; i < NUM_COMPONENTS; ++i)
	{
		if (m_pComponents[i] == pComponent)
		{
			// Unset entity so there can be no loop back to Entity::ReleaseComponent()
			m_pComponents[i]->SetEntity(0);

			//TODO: How to release app-custom components?
			switch (i)
			{
			case eCOMPONENT_RENDERABLE:
				pEntitySystem->RemoveRenderableComponent((IRenderableComponent*)m_pComponents[i]);
				break;
			case eCOMPONENT_PHYSICAL:
				pEntitySystem->RemovePhysicalComponent((IPhysicalComponent*)m_pComponents[i]);
				break;
			}

			m_pComponents[i] = 0;
			break;
		}
	}
}

S_API AABB CEntity::GetAABB()
{
	IRenderableComponent* pRenderable = GetRenderable();
	if (pRenderable)
		return pRenderable->GetAABB();

	IPhysicalComponent* pPhysical = GetPhysical();
	if (pPhysical)
		return pPhysical->GetAABB();

	return AABB();
}

S_API AABB CEntity::GetWorldAABB()
{
	return GetAABB().Transform(GetTransform());
}


SP_NMSPACE_END

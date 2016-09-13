//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2016, Pascal Rosenkranz
// ------------------------------------------------------------------------------
// Filename:	Entities.cpp
// Created:		6/24/2014 by Pascal Rosenkranz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#include <Implementation\Geometry\Entity.h>
#include <Implementation\Geometry\RenderableComponent.h>
#include <Implementation\Geometry\PhysicalComponent.h>
#include <Abstract\IGameEngine.h>
#include <Abstract\I3DEngine.h>
#include <Abstract\IVertexBuffer.h>

SP_NMSPACE_BEG

S_API IComponent::~IComponent()
{
	m_pEntity = 0;
	m_bTrash = true;
}

//////////////////////////////////////////////////////////////////////////////////

S_API CEntity::CEntity()
	: m_bTransformInvalid(true),
	m_Scale(1.0f)
{
}

S_API void CEntity::Clear()
{
	// We need to clear this first, otherwise Component::Release() will call Entity::ReleaseComponent()
	// which will delete the element in the list during the loop

	vector<IComponent*> components(m_Components.begin(), m_Components.end());
	for (auto itComponent = components.begin(); itComponent != components.end(); ++itComponent)
	{
		IComponent* component = *itComponent;
		if (component)
			component->Release();
	}
	m_Components.clear();


	vector<IComponent*> managed(m_ManagedComponents.begin(), m_ManagedComponents.end());
	for (auto itManaged = managed.begin(); itManaged != managed.end(); ++itManaged)
	{
		IComponent* pManaged = *itManaged;
		if (pManaged)
		{
			pManaged->Release();
			delete pManaged;
		}
	}
	m_ManagedComponents.clear();
}

// ----------------------------------------------------------------------------------------------------------

S_API void CEntity::OnEntityTransformed()
{
	m_bTransformInvalid = true;

	for (auto itComponent = m_Components.begin(); itComponent != m_Components.end(); ++itComponent)
	{
		IComponent* component = *itComponent;
		if (component)
			component->OnEntityTransformed();
	}
}

S_API const Vec3f& CEntity::GetPos() const
{
	return m_Pos;
}

S_API void CEntity::SetPos(const Vec3f& pos)
{	
	m_Pos = pos;
	OnEntityTransformed();
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
	OnEntityTransformed();
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
	OnEntityTransformed();
}

S_API const Vec3f& CEntity::GetPivot() const
{
	return m_Pivot;
}
S_API void CEntity::SetPivot(const Vec3f& pivot)
{
	m_Pivot = pivot;
	OnEntityTransformed();
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
	}

	return m_Transform;
}

// ----------------------------------------------------------------------------------------------------------

S_API AABB CEntity::GetAABB()
{
	CRenderableComponent* pRenderable = GetComponent<CRenderableComponent>();
	if (pRenderable)
		return pRenderable->GetAABB();

	CPhysicalComponent* pPhysical = GetComponent<CPhysicalComponent>();
	if (pPhysical)
		return pPhysical->GetAABB();

	return AABB();
}

S_API AABB CEntity::GetWorldAABB()
{
	return GetAABB().Transform(GetTransform());
}

// ----------------------------------------------------------------------------------------------------------

S_API const char* CEntity::GetName() const
{
	return m_Name.c_str();
}

S_API void CEntity::SetName(const char* name)
{
	m_Name = name;
}

// ----------------------------------------------------------------------------------------------------------

S_API void CEntity::AddComponentIntrl(IComponent* pComponent, bool managed /*=false*/)
{
	if (!pComponent)
		return;

	if (HasComponent(pComponent))
		return;

	m_Components.push_back(pComponent);
	pComponent->SetEntity(this);

	if (managed)
		m_ManagedComponents.push_back(pComponent);
}

S_API bool CEntity::HasComponent(IComponent* pComponent) const
{
	for (auto itComponent = m_Components.begin(); itComponent != m_Components.end(); ++itComponent)
	{
		if (*itComponent == pComponent)
			return true;
	}

	return false;
}

S_API void CEntity::ReleaseComponent(IComponent* pComponent)
{
	if (!pComponent)
		return;

	// In Release(), the component will call ReleaseComponent() of its entity, but
	// it will flag itself as trash before, so we dont have to call Release() again.
	if (!pComponent->IsTrash())
		pComponent->Release();

	for (auto itComponent = m_Components.begin(); itComponent != m_Components.end(); ++itComponent)
	{
		if (*itComponent == pComponent)
		{
			m_Components.erase(itComponent);
			break;
		}
	}

	for (auto itManaged = m_ManagedComponents.begin(); itManaged != m_ManagedComponents.end(); ++itManaged)
	{
		if (*itManaged == pComponent)
		{
			m_ManagedComponents.erase(itManaged);
			delete pComponent;
			break;
		}
	}
}

S_API unsigned int CEntity::GetNumComponents() const
{
	return m_Components.size();
}

S_API IComponent* CEntity::GetComponentByIndex(unsigned int index) const
{
	if (index < m_Components.size())
		return m_Components[index];
	else
		return 0;
}

SP_NMSPACE_END

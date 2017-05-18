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

#include "Entity.h"

SP_NMSPACE_BEG

S_API IComponent::~IComponent()
{
	m_pEntity = 0;
	m_bTrash = true;
}

//////////////////////////////////////////////////////////////////////////////////

S_API CEntity::CEntity()
	: m_pParent(0),
	m_bTransformInvalid(true),
	m_Scale(1.0f)
{
	RegisterProperty("pos", &m_Pos);
	RegisterProperty("rot", &m_Rot);
	RegisterProperty("scale", &m_Scale);
	RegisterProperty("name", &m_Name);
}

S_API void CEntity::Clear()
{
	for (auto itChild = m_Childs.begin(); itChild != m_Childs.end(); ++itChild)
	{
		CEntity* pChild = static_cast<CEntity*>(itChild->pEntity);
		if (pChild)
		{
			pChild->SetParent(0);
			if (itChild->dealloc)
				delete pChild;
		}
	}
	m_Childs.clear();

	if (m_pParent)
		m_pParent->RemoveChild(this);
	m_pParent = 0;


	vector<IComponent*> components(m_Components.begin(), m_Components.end());
	for (auto itComponent = components.begin(); itComponent != components.end(); ++itComponent)
	{
		IComponent* component = *itComponent;
		if (component)
		{
			component->SetEntity(0);
			component->Release();
		}
	}
	m_Components.clear();


	vector<IComponent*> managed(m_ManagedComponents.begin(), m_ManagedComponents.end());
	for (auto itManaged = managed.begin(); itManaged != managed.end(); ++itManaged)
	{
		IComponent* pManaged = *itManaged;
		if (pManaged)
		{
			pManaged->SetEntity(0);
			pManaged->Release();
			delete pManaged;
		}
	}
	m_ManagedComponents.clear();
}

// ----------------------------------------------------------------------------------------------------------

S_API void CEntity::SetParent(IEntity* pParent)
{
	m_pParent = pParent;
}

S_API IEntity* CEntity::GetParent() const
{
	return m_pParent;
}

S_API void CEntity::AddChild(IEntity* pEntity)
{
	CEntity* pEntityImpl = static_cast<CEntity*>(pEntity);
	if (!pEntityImpl)
		return;

	for (auto itChild = m_Childs.begin(); itChild != m_Childs.end(); ++itChild)
	{
		if (itChild->pEntity == pEntity)
			return;
	}

	SChildEntity child;
	child.pEntity = pEntity;
	child.dealloc = false;
	m_Childs.push_back(child);

	pEntityImpl->SetParent(this);
}

S_API IEntity* CEntity::CreateChild()
{
	CEntity* pEntity = new CEntity();
	pEntity->SetParent(this);

	SChildEntity child;
	child.pEntity = pEntity;
	child.dealloc = true;
	m_Childs.push_back(child);

	return pEntity;
}

S_API void CEntity::RemoveChild(IEntity* pEntity)
{
	CEntity* pEntityImpl = static_cast<CEntity*>(pEntity);
	if (!pEntityImpl)
		return;

	for (auto itChild = m_Childs.begin(); itChild != m_Childs.end(); ++itChild)
	{
		if (itChild->pEntity == pEntity)
		{
			pEntityImpl->SetParent(0);
			if (itChild->dealloc)
				delete pEntityImpl;

			m_Childs.erase(itChild);
			return;
		}
	}
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
	SetRotation(rotate * GetRotation());
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

S_API Mat44 CEntity::GetTransform()
{
	if (m_bTransformInvalid)
	{
		STransformationDesc transform;
		transform.translation = Mat44::MakeTranslationMatrix(m_Pos);
		transform.rotation = m_Rot.ToRotationMatrix();
		transform.scale = Mat44::MakeScaleMatrix(m_Scale);
		transform.preRotation = Mat44::MakeTranslationMatrix(-m_Pivot);
		m_Transform = transform.BuildTRS();

		m_bTransformInvalid = false;
	}

	if (m_pParent)
		return m_pParent->GetTransform() * m_Transform;
	else
		return m_Transform;
}

S_API Vec3f CEntity::GetLeft() const
{
	return m_Rot * Vec3f(1.0f, 0, 0);
}

S_API Vec3f CEntity::GetForward() const
{
	return m_Rot * Vec3f(0, 0, 1.0f);
}

S_API Vec3f CEntity::GetUp() const
{
	return m_Rot * Vec3f(0, 1.0f, 0);
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
	{
		CLog::Log(S_DEBUG, "Not adding component: Entity already has such a component");
		return;
	}

	m_Components.push_back(pComponent);
	pComponent->SetEntity(this);

	if (managed)
		m_ManagedComponents.push_back(pComponent);

	// Trigger transform update
	pComponent->OnEntityTransformed();
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



//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

S_API EntityProperty::EntityProperty(const string& name)
	: m_Name(name),
	m_Type(EP_TYPE_UNDEFINED),
	m_StoreType(EP_STORE_TYPE_PROPERTY),
	m_Ptr(0),
	m_pExternal(0)
{
}

S_API EntityProperty::EntityProperty(const string& name, void* ptr, EEntityPropertyType ptrType)
	: m_Name(name),
	m_Type(ptrType),
	m_StoreType(EP_STORE_TYPE_REFERENCE),
	m_Ptr(ptr),
	m_pExternal(0)
{
}

S_API EntityProperty::EntityProperty(const string& name, IExternalEntityProperty* pExternal)
	: m_Name(name),
	m_Type(EP_TYPE_UNDEFINED),
	m_StoreType(EP_STORE_TYPE_EXTERNAL),
	m_Ptr(0),
	m_pExternal(pExternal)
{
}

S_API EntityProperty::EntityProperty(const EntityProperty& prop)
	: m_Name(prop.m_Name),
	m_Value(prop.m_Value),
	m_Type(prop.m_Type),
	m_StoreType(prop.m_StoreType),
	m_Ptr(prop.m_Ptr),
	m_pExternal(prop.m_pExternal)
{
}

S_API EntityProperty::~EntityProperty()
{
	Reset();
}

S_API const string& EntityProperty::GetName() const
{
	return m_Name;
}

S_API EEntityPropertyType EntityProperty::GetType() const
{
	return m_Type;
}

S_API EEntityPropertyStoreType EntityProperty::GetStoreType() const
{
	return m_StoreType;
}

S_API void EntityProperty::Reset()
{
	m_Type = EP_TYPE_UNDEFINED;
	m_StoreType = EP_STORE_TYPE_PROPERTY;

	if (m_pExternal)
		delete m_pExternal;

	m_pExternal = 0;
	m_Ptr = 0;
	m_Value.str = "";
	m_Value.quat = Quat();
	m_Value.vec3 = Vec3f();
	m_Value.us = 0;
}


S_API void EntityProperty::SetExternal(IExternalEntityProperty* external)
{
	if (!external)
		return;
	if (m_StoreType != EP_STORE_TYPE_PROPERTY)
		return;
	m_Type = EP_TYPE_UNDEFINED;
	m_StoreType = EP_STORE_TYPE_EXTERNAL;
	m_pExternal = external;
}


#define ENTITY_PROPERTY_SET_PTR_IMPL(type, typenm) \
	template<> void EntityProperty::Set<type>(type* ptr) \
	{ \
		if (m_Type != EP_TYPE_UNDEFINED && m_Type != typenm) \
			return; \
		if (m_StoreType != EP_STORE_TYPE_PROPERTY) \
			return; \
		m_Type = typenm; \
		m_StoreType = EP_STORE_TYPE_REFERENCE; \
		m_Ptr = (void*)ptr; \
	}

#define ENTITY_PROPERTY_SET_IMPL(type, typenm, valnm) \
	ENTITY_PROPERTY_SET_PTR_IMPL(type, typenm) \
	template<> void EntityProperty::Set<type>(const type& val) \
	{ \
		if (m_StoreType == EP_STORE_TYPE_EXTERNAL) { \
			if (m_pExternal) { \
				/* Type check is done via dynamic_cast here */ \
				ExternalEntityProperty<type>* extprop = \
					dynamic_cast<ExternalEntityProperty<type>*>(m_pExternal); \
				if (extprop) \
					extprop->Set(val); \
			} \
		} \
		else if (m_Type == EP_TYPE_UNDEFINED || m_Type == typenm) { \
			m_Type = typenm; \
			switch (m_StoreType) { \
				case EP_STORE_TYPE_PROPERTY: \
					m_Value.valnm = val; \
					break; \
				case EP_STORE_TYPE_REFERENCE: { \
						type* ptr = reinterpret_cast<type*>(m_Ptr); \
						if (ptr) \
							*ptr = val; \
						break; \
					} \
				default: \
					break; \
			} \
		} \
	}

#define ENTITY_PROPERTY_GET_IMPL_BODY(type, typenm, valnm, defval, ExtGetFn) \
	{ \
		static const type _default = defval; \
		if (m_StoreType == EP_STORE_TYPE_EXTERNAL) { \
			if (m_pExternal) { \
				/* Type check is done via dynamic_cast here */ \
				ExternalEntityProperty<type>* extprop = \
					dynamic_cast<ExternalEntityProperty<type>*>(m_pExternal); \
				if (extprop) \
					return extprop->ExtGetFn(); \
			} \
		} \
		else if (m_Type == typenm) { \
			switch (m_StoreType) { \
				case EP_STORE_TYPE_PROPERTY: \
					return m_Value.valnm; \
				case EP_STORE_TYPE_REFERENCE: \
					return (m_Ptr ? *reinterpret_cast<type*>(m_Ptr) : _default); \
				default: \
					break; \
			} \
		} \
		return _default; \
	}

#define ENTITY_PROPERTY_IMPL(type, typenm, valnm, defval) \
	ENTITY_PROPERTY_SET_IMPL(type, typenm, valnm) \
	template<> type EntityProperty::Get<type>() const \
		ENTITY_PROPERTY_GET_IMPL_BODY(type, typenm, valnm, defval, Get)

#define ENTITY_PROPERTY_IMPL_CREF(type, typenm, valnm, defval, GetFnName) \
	ENTITY_PROPERTY_SET_IMPL(type, typenm, valnm) \
	const type& EntityProperty::GetFnName() const \
		ENTITY_PROPERTY_GET_IMPL_BODY(type, typenm, valnm, defval, GetRef)



ENTITY_PROPERTY_IMPL(short, EP_TYPE_SIGNED, s, SHRT_MAX)
ENTITY_PROPERTY_IMPL(unsigned short, EP_TYPE_UNSIGNED, us, USHRT_MAX)
ENTITY_PROPERTY_IMPL(int, EP_TYPE_SIGNED, s, INT_MAX)
ENTITY_PROPERTY_IMPL(unsigned int, EP_TYPE_UNSIGNED, us, UINT_MAX)
ENTITY_PROPERTY_IMPL(long, EP_TYPE_SIGNED, s, LONG_MAX)
ENTITY_PROPERTY_IMPL(unsigned long, EP_TYPE_UNSIGNED, us, ULONG_MAX)
ENTITY_PROPERTY_IMPL(float, EP_TYPE_FLOAT, f, FLT_MAX)
ENTITY_PROPERTY_IMPL(SpeedPoint::Quat, EP_TYPE_QUATERNION, quat, SpeedPoint::Quat())

ENTITY_PROPERTY_IMPL_CREF(string, EP_TYPE_STRING, str, "???", GetString)
void EntityProperty::Set(const char* val)
{
	Set(std::string(val));
}

ENTITY_PROPERTY_IMPL_CREF(Vec3f, EP_TYPE_VEC3, vec3, Vec3f(), GetVec3)
void EntityProperty::Set(float x, float y, float z)
{
	Set(Vec3f(x, y, z));
}




SP_NMSPACE_END

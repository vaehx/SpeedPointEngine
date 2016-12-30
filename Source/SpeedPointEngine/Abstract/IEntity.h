//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2015, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	Objects.h
// Created:	8/12/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//	- 6/24/2015: Merge in components
//
//////////////////////////////////////////////////////////////////////////////////

#ifndef __ivisibleobject_h__
#define __ivisibleobject_h__

#if _MSC_VER > 1000
#pragma once
#endif

#include "SPrerequisites.h"
#include "Transformable.h"
#include "BoundBox.h"
#include "Vector3.h"
#include "Quaternion.h"
#include <vector>
#include <string>
#include <map>

using std::vector;
using std::string;
using std::map;

SP_NMSPACE_BEG

struct S_API IEntity;
struct S_API IComponent;
struct S_API IMaterial;
struct S_API SAxisAlignedBoundBox;
typedef struct S_API SAxisAlignedBoundBox AABB;
struct S_API IGameEngine;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//typedef vector<SMaterial*> MaterialPtrList;

class MaterialPtrList
{
private:
	vector<IMaterial*> m_Mats;
public:
	MaterialPtrList() {}	

	~MaterialPtrList()
	{
		Clear();
	}

	void Add(IMaterial* pMat)
	{
		m_Mats.push_back(pMat);
	}

	void AddAll(MaterialPtrList& matlist)
	{
		m_Mats.reserve(matlist.GetCount());
		for (unsigned int i = 0; i < matlist.GetCount(); ++i)
			m_Mats.push_back(matlist.Get(i));
	}

	void Del(IMaterial* pMat)
	{
		for (auto itMat = m_Mats.begin(); itMat != m_Mats.end(); itMat++)
		{
			if (*itMat != pMat)
				continue;		
				
			m_Mats.erase(itMat);
			return;
		}
	}

	IMaterial* Get(unsigned int index)
	{
		if (index >= GetCount())
			return nullptr;

		return m_Mats[index];
	}

	void Set(unsigned int index, IMaterial* pMat)
	{
		if (index >= GetCount())
			return;

		m_Mats[index] = pMat;
	}

	unsigned int GetCount() const
	{
		return m_Mats.size();
	}

	void Clear()
	{
		m_Mats.clear();
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


enum EEntityPropertyType
{
	EP_TYPE_UNDEFINED = 0,
	EP_TYPE_SIGNED,
	EP_TYPE_UNSIGNED,
	EP_TYPE_FLOAT,
	EP_TYPE_STRING,
	EP_TYPE_VEC3,
	EP_TYPE_QUATERNION
};


class EntityProperty
{
private:
	string m_Name;
	struct {
		string str;
		Vec3f vec3;
		Quat quat;
		union {
			long s;
			unsigned long us;
			float f;
		};
	} m_Value;
	EEntityPropertyType m_Type;
	bool m_bPtr;
	void* m_Ptr;

public:
	EntityProperty(const string& name)
		: m_Name(name),
		m_Type(EP_TYPE_UNDEFINED),
		m_bPtr(false),
		m_Ptr(0)
	{
	}

	EntityProperty(const string& name, void* ptr, EEntityPropertyType ptrType)
		: m_Name(name),
		m_Type(ptrType),
		m_bPtr(true),
		m_Ptr(ptr)
	{
	}

	EntityProperty(const EntityProperty& prop)
		: m_Name(prop.m_Name),
		m_Value(prop.m_Value),
		m_Type(prop.m_Type),
		m_bPtr(prop.m_bPtr),
		m_Ptr(prop.m_Ptr)
	{
	}

	const string& GetName() const
	{
		return m_Name;
	}

	EEntityPropertyType GetType() const
	{
		return m_Type;
	}

	void Reset()
	{
		m_Type = EP_TYPE_UNDEFINED;
		m_bPtr = false;
		m_Ptr = 0;
	}

	template<typename T>
	void Set(const T& val);

	// Makes the entity point to the given pointer.
	// If this property was previously set, this method will do nothing.
	template<typename T>
	void Set(T* ptr);

	void Set(const char* val);
	void Set(float x, float y, float z);

	template<typename T>
	T Get() const;

	const string& GetString() const
	{
		static const string _default = "???";
		if (m_Type != EP_TYPE_STRING)
			return _default;
		if (!m_bPtr)
			return m_Value.str;

		string* str = reinterpret_cast<string*>(m_Ptr);
		return (str ? *str : _default);
	}

	const Vec3f& GetVec3() const
	{
		static const Vec3f _default;
		if (m_Type != EP_TYPE_VEC3)
			return _default;
		if (!m_bPtr)
			return m_Value.vec3;

		Vec3f* vec3 = reinterpret_cast<Vec3f*>(m_Ptr);
		return (vec3 ? *vec3 : _default);
	}
};


#define ENTITY_PROPERTY_SET_IMPL(type, typenm, valnm) \
	template<> void EntityProperty::Set<type>(const type& val) \
	{ \
		if (m_Type == EP_TYPE_UNDEFINED || m_Type == typenm) { \
			if (m_bPtr) { \
				type* ptr = reinterpret_cast<type*>(m_Ptr); \
				if (ptr) *ptr = val; \
			} \
			else { \
				m_Value.valnm = val; \
			} \
			m_Type = typenm; \
		} \
	}

#define ENTITY_PROPERTY_SET_PTR_IMPL(type, typenm) \
	template<> void EntityProperty::Set<type>(type* ptr) \
	{ \
		if (m_Type != EP_TYPE_UNDEFINED) \
			return; \
		m_bPtr = true; \
		m_Ptr = (void*)ptr; \
		m_Type = typenm; \
	}

#define ENTITY_PROPERTY_GET_IMPL(type, typenm, valnm, defval) \
	template<> type EntityProperty::Get<type>() const \
	{ \
		static const type _default = defval; \
		if (m_Type != typenm) \
			return _default; \
		if (!m_bPtr) \
			return (type)m_Value.valnm; \
		\
		type* ptr = reinterpret_cast<type*>(m_Ptr); \
		return (ptr ? *ptr : _default); \
	}

#define ENTITY_PROPERTY_IMPL(type, typenm, valnm, defval) \
	ENTITY_PROPERTY_SET_IMPL(type, typenm, valnm) \
	ENTITY_PROPERTY_SET_PTR_IMPL(type, typenm) \
	ENTITY_PROPERTY_GET_IMPL(type, typenm, valnm, defval)


ENTITY_PROPERTY_IMPL(short, EP_TYPE_SIGNED, s, SHRT_MAX)
ENTITY_PROPERTY_IMPL(unsigned short, EP_TYPE_UNSIGNED, us, USHRT_MAX)
ENTITY_PROPERTY_IMPL(int, EP_TYPE_SIGNED, s, INT_MAX)
ENTITY_PROPERTY_IMPL(unsigned int, EP_TYPE_UNSIGNED, us, UINT_MAX)
ENTITY_PROPERTY_IMPL(long, EP_TYPE_SIGNED, s, LONG_MAX)
ENTITY_PROPERTY_IMPL(unsigned long, EP_TYPE_UNSIGNED, us, ULONG_MAX)

ENTITY_PROPERTY_IMPL(float, EP_TYPE_FLOAT, f, FLT_MAX)

ENTITY_PROPERTY_IMPL(std::string, EP_TYPE_STRING, str, "???")
void EntityProperty::Set(const char* val)
{
	Set(std::string(val));
}

ENTITY_PROPERTY_IMPL(SpeedPoint::Vec3f, EP_TYPE_VEC3, vec3, SpeedPoint::Vec3f())
void EntityProperty::Set(float x, float y, float z)
{
	Set(Vec3f(x, y, z));
}

ENTITY_PROPERTY_IMPL(SpeedPoint::Quat, EP_TYPE_QUATERNION, quat, SpeedPoint::Quat())


class EntityPropertyContainer
{
private:
	map<string, EntityProperty> m_Properties;

	EntityProperty& AddProperty(const string& name)
	{
		auto inserted = m_Properties.insert(std::pair<string, EntityProperty>(name, EntityProperty(name)));
		return inserted.first->second;
	}

public:
	const map<string, EntityProperty>* GetProperties() const
	{
		return &m_Properties;
	}

	template<typename T>
	T GetProperty(const string& name, bool* found = 0) const
	{
		auto itFound = m_Properties.find(name);
		if (itFound == m_Properties.end())
		{
			if (found)
				*found = false;
			return T();
		}

		if (found)
			*found = true;
		return itFound->second.Get<T>();
	}

	template<typename T>
	void SetProperty(const string& name, const T& val)
	{
		AddProperty(name).Set(val);
	}

	template<typename T>
	void RegisterProperty(const string& name, T* ptr)
	{
		AddProperty(name).Set(ptr);
	}

	void SetProperty(const string& name, const char* val)
	{
		AddProperty(name).Set(val);
	}

	void SetProperty(const string& name, float x, float y, float z)
	{
		AddProperty(name).Set(x, y, z);
	}

	void DeleteProperty(const string& name)
	{
		m_Properties.erase(name);
	}

	bool HasProperty(const string& name) const
	{
		return m_Properties.find(name) != m_Properties.end();
	}
};





////////////////////////////////////////////////////////////////////////////////////////////////
//
//			ENTITY
//
////////////////////////////////////////////////////////////////////////////////////////////////

struct S_API IEntity : public EntityPropertyContainer
{
	virtual ~IEntity() {}

	ILINE virtual const Vec3f& GetPos() const = 0;
	ILINE virtual void SetPos(const Vec3f& pos) = 0;
	ILINE virtual void Translate(const Vec3f& translate) = 0;

	ILINE virtual const Quat& GetRotation() const = 0;
	ILINE virtual void SetRotation(const Quat& rotation) = 0;
	ILINE virtual void Rotate(const Quat& rotate) = 0;

	ILINE virtual const Vec3f& GetScale() const = 0;
	ILINE virtual void SetScale(const Vec3f& scale) = 0;

	ILINE virtual const Vec3f& GetPivot() const = 0;
	ILINE virtual void SetPivot(const Vec3f& pivot) = 0;

	ILINE virtual const char* GetName() const = 0;
	ILINE virtual void SetName(const char* name) = 0;

	ILINE virtual Mat44 GetTransform() = 0;

	ILINE virtual Vec3f GetLeft() const = 0;
	ILINE virtual Vec3f GetForward() const = 0;
	ILINE virtual Vec3f GetUp() const = 0;

	ILINE virtual AABB GetAABB() = 0;
	ILINE virtual AABB GetWorldAABB() = 0;

	// If the component is already added to the entity, will not add again
	// Will not add if the given component cannot be cast to an IComponent ptr (will return 0 then)
	template<typename ComponentImpl>
	ILINE ComponentImpl* AddComponent(ComponentImpl* pComponentImpl)
	{
		IComponent* pComponent = dynamic_cast<IComponent*>(pComponentImpl);
		if (pComponent)
		{
			AddComponentIntrl(pComponent, false);
			return pComponentImpl;
		}
		else
		{
			return 0;
		}
	}

	// Adds and manages ownership of a new component of the given type
	// Returns 0 if the given component type cannot be cast to an IComponent ptr.
	template<typename ComponentImpl>
	ILINE ComponentImpl* CreateComponent()
	{
		ComponentImpl* pComponentImpl = new ComponentImpl();
		IComponent* pComponent = dynamic_cast<IComponent*>(pComponentImpl);
		if (pComponent)
		{
			AddComponentIntrl(pComponent, true);
			return pComponentImpl;
		}
		else
		{
			delete pComponentImpl;
			return 0;
		}
	}

	ILINE virtual bool HasComponent(IComponent* pComponent) const = 0;
	
	// Warning: If this is a managed component, it will be destructed
	// by this call and the pointer will be INVALID !
	ILINE virtual void ReleaseComponent(IComponent* pComponent) = 0;

	ILINE virtual unsigned int GetNumComponents() const = 0;
	ILINE virtual IComponent* GetComponentByIndex(unsigned int index) const = 0;

	template<typename T>
	ILINE T* GetComponent() const
	{
		//TODO: This approach might be slow. Check that.
		unsigned int numComponents = GetNumComponents();
		for (unsigned int i = 0; i < numComponents; ++i)
		{
			T* pCom = dynamic_cast<T*>(GetComponentByIndex(i));
			if (pCom)
				return pCom;
		}

		return 0;
	}


	ILINE virtual void AddChild(IEntity* pEntity) = 0;
	
	// Deallocation will be handled by this entity!
	ILINE virtual IEntity* CreateChild() = 0;

	ILINE virtual void RemoveChild(IEntity* pEntity) = 0;
	ILINE virtual IEntity* GetParent() const = 0;


	ILINE virtual void Clear() = 0;

protected:
	ILINE virtual void AddComponentIntrl(IComponent* pComponent, bool managed = false) = 0;
};

typedef S_API struct IEntity IObject;



struct S_API IReferenceObject : public IEntity
{
	virtual ~IReferenceObject() {}

	virtual IObject* GetBase() = 0;
	virtual SResult SetBase(IObject* base) = 0;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Entity Component
struct S_API IComponent
{
protected:
	IEntity* m_pEntity;
	bool m_bTrash;

public:
	IComponent() : m_pEntity(0), m_bTrash(false) {}
	IComponent(IEntity* pEntity) : m_pEntity(pEntity), m_bTrash(false) { }

	// Implement in CEntity.cpp to make this class abstract
	virtual ~IComponent() = 0;

	// We need this, because we need a standard constructor to be able
	// to construct the components in the subsystems
	ILINE virtual void SetEntity(IEntity* pEntity)
	{
		m_pEntity = pEntity;
	}

	ILINE virtual IEntity* GetEntity() const
	{
		return m_pEntity;
	}

	ILINE void Release()
	{
		m_bTrash = true;
		OnRelease();
		if (m_pEntity)
		{
			m_pEntity->ReleaseComponent(this);
			m_pEntity = 0;
		}
	}

	ILINE bool IsTrash() const
	{
		return m_bTrash;
	}



	// Events:

	ILINE virtual void OnRelease() {};

	// We let the entity call this event itself, instead of having a Transformable component that
	// other components can register their event handlers to
	ILINE virtual void OnEntityTransformed() {};
};

#define DEFINE_COMPONENT \
	virtual void Release() { IComponent::Release(); } \
	virtual bool IsTrash() const { return IComponent::IsTrash(); }





SP_NMSPACE_END

#endif

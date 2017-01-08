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
struct S_API IEntityReceipt;
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


struct IExternalEntityProperty
{
	virtual ~IExternalEntityProperty() {}
};

template<typename T>
struct ExternalEntityProperty : public IExternalEntityProperty
{
public:
	typedef std::function<T()> Getter;
	typedef std::function<const T&()> RefGetter;
	typedef std::function<void(const T&)> Setter;

private:
	Getter m_GetFn;
	RefGetter m_RefGetFn;
	Setter m_SetFn;

public:
	ExternalEntityProperty(const Getter& getFn, const RefGetter& refGetFn, const Setter& setFn)
		: m_GetFn(getFn),
		m_RefGetFn(refGetFn),
		m_SetFn(setFn)
	{
	}

	void Set(const T& val) const
	{
		if (m_SetFn)
			m_SetFn(val);
	}

	T Get(const T& defval = T()) const
	{
		// Prefer getting the value as an lvalue reference
		if (m_RefGetFn)
			return m_RefGetFn();
		else if (m_GetFn)
			return m_GetFn();
		else
			return defval;
	}

	const T& GetRef() const
	{
		static const T _default = T();
		if (m_RefGetFn)
			return m_RefGetFn();
		else
			return _default;
	}
};



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

enum EEntityPropertyStoreType
{
	EP_STORE_TYPE_PROPERTY = 0,
	EP_STORE_TYPE_REFERENCE,
	EP_STORE_TYPE_EXTERNAL // via setter/getter
};


class EntityProperty
{
private:
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

	string m_Name;
	
	EEntityPropertyStoreType m_StoreType;
	EEntityPropertyType m_Type;
	void* m_Ptr;
	IExternalEntityProperty* m_pExternal;

	inline void SetExternal(IExternalEntityProperty* external);

public:
	EntityProperty(const string& name);
	EntityProperty(const string& name, void* ptr, EEntityPropertyType ptrType);
	EntityProperty(const string& name, IExternalEntityProperty* pExternal);
	EntityProperty(const EntityProperty& prop);
	~EntityProperty();

	const string& GetName() const;
	EEntityPropertyType GetType() const;
	EEntityPropertyStoreType GetStoreType() const;

	void Reset();

	template<typename T>
	void SetExternal(const typename ExternalEntityProperty<T>::Getter& getFn, const typename ExternalEntityProperty<T>::Setter& setFn)
	{
		SetExternal(new ExternalEntityProperty<T>(getFn, ExternalEntityProperty<T>::RefGetter(), setFn));
	}

	template<typename T>
	void SetExternalRef(const typename ExternalEntityProperty<T>::RefGetter& refGetFn, const typename ExternalEntityProperty<T>::Setter& setFn)
	{
		SetExternal(new ExternalEntityProperty<T>(ExternalEntityProperty<T>::Getter(), refGetFn, setFn));
	}

	template<typename T>
	void Set(const T& val);

	// Makes the entity point to the given pointer.
	template<typename T>
	void Set(T* ptr);

	void Set(const char* val);
	void Set(float x, float y, float z);

	template<typename T>
	T Get() const;

	const string& GetString() const;
	const Vec3f& GetVec3() const;
};


/*
	Property storing value:
		entity->SetProperty("mass", 100.0f);

	Pointer to the value:
		entity->RegisterProperty("mass", &m_Mass);

	Property using getter & setter:
		entity->RegisterProperty("mass", bind(&Physical::GetMass, this), bind(&Physical::SetMass, this));


	The type of the property cannot be changed after setting the first time.
	The store type of the property can only be altered from the EP_STORE_TYPE_PROPERTY (storing value).
*/

class EntityPropertyContainer
{
private:
	map<string, EntityProperty> m_Properties;

	EntityProperty& AddProperty(const string& name)
	{
		// Will not insert, if property with name already exists
		auto inserted = m_Properties.insert(std::pair<string, EntityProperty>(name, EntityProperty(name)));
		return inserted.first->second;
	}

public:
	const void GetAllPropertyNames(vector<string>& names) const
	{
		for (auto itProperty = m_Properties.begin(); itProperty != m_Properties.end(); ++itProperty)
			names.push_back(itProperty->first);
	}

	template<typename T>
	T GetProperty(const string& name, bool* found = 0, const T& defval = T()) const
	{
		auto itFound = m_Properties.find(name);
		if (itFound != m_Properties.end())
		{
			if (found)
				*found = true;

			return itFound->second.Get<T>();
		}

		if (found)
			*found = false;

		return defval;
	}

	// Overrides existing property with given name!
	template<typename T>
	void RegisterProperty(const string& name, T* ptr)
	{
		AddProperty(name).Set(ptr);
	}


#define ENTITY_REGISTER_PROPERTY(setExternalFn, getFncPtr, setFncPtr) \
	template<typename T, typename C, typename ...Args> \
	void RegisterProperty(const string& name, C* c, getFncPtr, setFncPtr, Args... args) \
	{ \
		AddProperty(name).setExternalFn<T>(std::bind(getter, c, args...), std::bind(setter, c, std::placeholders::_1, args...)); \
	} \
	template<typename T, typename C, typename ...Args> \
	void RegisterProperty(const string& name, C* c, getFncPtr, Args... args) \
	{ \
		AddProperty(name).setExternalFn<T>(std::bind(getter, c, args...), ExternalEntityProperty<T>::Setter()); \
	} \

	ENTITY_REGISTER_PROPERTY(SetExternal, T(C::*getter)(Args...), void(C::*setter)(const T&, Args...))
	ENTITY_REGISTER_PROPERTY(SetExternal, T(C::*getter)(Args...) const, void(C::*setter)(const T&, Args...) const)
	ENTITY_REGISTER_PROPERTY(SetExternalRef, const T&(C::*getter)(Args...), void(C::*setter)(const T&, Args...))
	ENTITY_REGISTER_PROPERTY(SetExternalRef, const T&(C::*getter)(Args...) const, void(C::*setter)(const T&, Args...) const)

	template<typename T>
	void SetProperty(const string& name, const T& val)
	{
		AddProperty(name).Set(val);
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

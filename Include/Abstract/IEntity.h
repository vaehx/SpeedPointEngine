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

#include <SPrerequisites.h>
#include "Transformable.h"
#include "BoundBox.h"
#include "Vector3.h"
#include "Quaternion.h"
#include <vector>
#include <string>

using std::vector;
using std::string;

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





////////////////////////////////////////////////////////////////////////////////////////////////
//
//			ENTITY
//
////////////////////////////////////////////////////////////////////////////////////////////////

struct S_API IEntity
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

	ILINE virtual const SMatrix& GetTransform() = 0;

	ILINE virtual AABB GetAABB() = 0;
	ILINE virtual AABB GetWorldAABB() = 0;

	// If the component is already added to the entity, will not add again
	// Will not add if the given component cannot be cast to an IComponent ptr
	template<typename ComponentImpl>
	ILINE void AddComponent(ComponentImpl* pComponentImpl)
	{
		IComponent* pComponent = dynamic_cast<IComponent*>(pComponentImpl);
		if (pComponent)
			AddComponentIntrl(pComponent, false);
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

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
struct S_API IMaterial;
struct S_API SAxisAlignedBoundBox;
typedef struct S_API SAxisAlignedBoundBox AABB;
struct S_API SLightDesc;
struct S_API SRenderDesc;
struct S_API IGeometry;
struct S_API IVertexBuffer;
struct S_API IIndexBuffer;
struct S_API SGeomSubset;
struct S_API SCamera;
struct S_API IGameEngine;
struct S_API ITexture;

struct S_API IRenderableComponent;
struct S_API IPhysicalComponent;


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

enum S_API EEntityEvent
{
	// The transformation of the entity is invalidated
	eENTITY_EVENT_TRANSFORM
};

struct S_API SEntityEvent
{
	EEntityEvent type;
	union
	{
		struct
		{
			Vec3f pos, scale, pivot;
			Quat rot;
		} transform;
	};

	SEntityEvent()
	{
	}
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum EComponentType
{
	eCOMPONENT_RENDERABLE = 0,
	eCOMPONENT_PHYSICAL,
	//eCOMPONENT_ANIMATION,
	//eCOMPONENT_SCRIPT

	NUM_COMPONENTS
};

struct IComponent
{
	ILINE virtual EComponentType GetType() const = 0;
	ILINE virtual void SetEntity(IEntity* entity) = 0;
	ILINE virtual IEntity* GetEntity() const = 0;
	ILINE virtual void OnEntityEvent(const SEntityEvent& event) {}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct S_API IAnimateableComponent
{
	virtual ~IAnimateableComponent()
	{
	}
};

struct S_API IScriptComponent
{
	virtual ~IScriptComponent()
	{
	}
};


////////////////////////////////////////////////////////////////////////////////////////////////
//
//			ENTITY
//
////////////////////////////////////////////////////////////////////////////////////////////////

// A complex object - composed of components
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

	ILINE virtual IComponent* CreateComponent(EComponentType type) = 0;
	
	ILINE virtual IRenderableComponent* CreateRenderable() = 0;
	ILINE virtual IPhysicalComponent* CreatePhysical() = 0;

	// Returns NULL if the component was not created
	ILINE virtual IComponent* GetComponent(EComponentType type) const = 0;
	ILINE virtual IRenderableComponent* GetRenderable() const = 0;
	ILINE virtual IPhysicalComponent* GetPhysical() const = 0;

	ILINE virtual void SetComponent(EComponentType type, IComponent* pComponent) = 0;

	ILINE virtual void ReleaseComponent(IComponent* pComponent) = 0;

	ILINE virtual void Clear() = 0;
};

typedef S_API struct IEntity IObject;



struct S_API IReferenceObject : public IEntity
{
	virtual ~IReferenceObject() {}

	virtual IObject* GetBase() = 0;
	virtual SResult SetBase(IObject* base) = 0;
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct S_API IEntitySystem
{
	virtual ~IEntitySystem() {}

	virtual IEntity* CreateEntity() = 0;
	virtual IRenderableComponent* CreateRenderableComponent() const = 0;
	virtual IPhysicalComponent* CreatePhysicalComponent() const = 0;

	virtual void RemoveRenderableComponent(IRenderableComponent* renderable) const = 0;
	virtual void RemovePhysicalComponent(IPhysicalComponent* physical) const = 0;
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// A light
struct S_API ILight : public STransformable
{
public:
	virtual ~ILight() {}

	virtual void GetLightDesc(SLightDesc* pDestDesc) const = 0;
};





SP_NMSPACE_END

#endif
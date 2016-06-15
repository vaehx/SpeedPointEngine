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
#include <vector>
#include <string>

using std::vector;
using std::string;

SP_NMSPACE_BEG

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

enum EComponentType
{
	eCOMPONENT_RENDERABLE = 0,
	//eCOMPONENT_PHYSICAL,
	//eCOMPONENT_ANIMATION,
	//eCOMPONENT_SCRIPT

	NUM_COMPONENTS
};

struct IComponent
{
	ILINE virtual void Init(IEntity* pEntity) = 0;
	ILINE virtual EComponentType GetType() const = 0;
	ILINE virtual void Clear() = 0;
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
struct S_API IEntity : public STransformable
{
	virtual ~IEntity() {}

	ILINE virtual const char* GetName() const = 0;
	ILINE virtual void SetName(const char* name) = 0;

	ILINE virtual IComponent* CreateComponent(EComponentType type) const = 0;
	
	// Returns NULL if the component was not created
	ILINE virtual IComponent* GetComponent(EComponentType type) const = 0;

	ILINE virtual void SetComponent(EComponentType type, IComponent* pComponent) const = 0;
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

	virtual IEntity* CreateEntity() const = 0;
};




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// A light
struct S_API ILight : public STransformable
{
public:
	virtual ~ILight() {}

	virtual void GetLightDesc(SLightDesc* pDestDesc) const = 0;
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// A static object does not dynamic, i.e. does not move or is modified in any other way per frame
struct S_API IStaticObject : public STransformable
{
public:
	virtual ~IStaticObject() {}

	virtual const char* GetName() const = 0;
	virtual void SetName(const char* name) = 0;
	
	ILINE virtual IRenderableComponent* GetRenderable() = 0;

	virtual SRenderDesc* GetRenderDesc() = 0;
	virtual void Clear() = 0;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct S_API ISkyBox
{
	virtual ~ISkyBox() {}

	virtual SResult InitGeometry(IGameEngine* pEngine) = 0;
	virtual void SetTexture(ITexture* pTexture) = 0;
	virtual void Clear() = 0;

	// SkyBox always centered around camera
	virtual void SetPosition(const Vec3f& pos) = 0;

	// Updates the renderDesc at the given Cameras position
	virtual SRenderDesc* GetUpdatedRenderDesc() = 0;
};





SP_NMSPACE_END

#endif
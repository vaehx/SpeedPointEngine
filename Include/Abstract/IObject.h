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

typedef unsigned long EntityType;
#define SP_ENTITY_RENDERABLE 0x01
#define SP_ENTITY_PHYSICAL 0x01 << 1
#define SP_ENTITY_ANIMATEABLE 0x01 << 2
#define SP_ENTITY_SCRIPTED 0x01 << 3

struct S_API IRenderableComponent
{
	virtual ~IRenderableComponent()
	{
	}

	virtual void Clear() = 0;

	ILINE virtual void SetVisible(bool visible) = 0;

	ILINE virtual void GetUpdatedRenderDesc(SRenderDesc* pDest) = 0;

	virtual IGeometry* GetGeometry() = 0;

	virtual IVertexBuffer* GetVertexBuffer() = 0;

	virtual SGeomSubset* GetSubset(unsigned int i) = 0;
	virtual unsigned int GetSubsetCount() const = 0;

	virtual IMaterial* GetSubsetMaterial(unsigned int subset = 0) = 0;

	virtual void SetViewProjMatrix(const SMatrix& mtx) = 0;
	virtual void UnsetViewProjMatrix() = 0;
};


struct S_API IPhysicalComponent
{
	virtual ~IPhysicalComponent()
	{
	}
};

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
class S_API IEntity : public STransformable
{
protected:
	AABB m_AABB;
	string m_Name;

public:	
	virtual ~IEntity() {}

	ILINE virtual void RecalcBoundBox()
	{
	}

	ILINE virtual const AABB& GetBoundBox() const { return m_AABB; }

	ILINE virtual EntityType GetType() const { return 0; };

	ILINE virtual const char* GetName() const { return m_Name.c_str(); }
	ILINE virtual void SetName(const char* name) { m_Name = name; }

	ILINE virtual bool IsRenderable() const { return (GetType() & SP_ENTITY_RENDERABLE) != 0; }
	ILINE virtual bool IsPhysical() const { return (GetType() & SP_ENTITY_PHYSICAL) != 0; }
	ILINE virtual bool IsAnimateable() const { return (GetType() & SP_ENTITY_ANIMATEABLE) != 0; }
	ILINE virtual bool IsScripted() const { return (GetType() & SP_ENTITY_SCRIPTED) != 0; }

	ILINE virtual IRenderableComponent* GetRenderable() const { return 0; }
	ILINE virtual IPhysicalComponent* GetPhysical() const { return 0; }
	ILINE virtual IAnimateableComponent* GetAnimateable() const { return 0; }
	ILINE virtual IScriptComponent* GetScriptable() const { return 0; }
};

typedef S_API class IEntity IObject;



struct S_API IReferenceObject : public IEntity
{
	virtual ~IReferenceObject() {}

	virtual IObject* GetBase() = 0;
	virtual SResult SetBase(IObject* base) = 0;
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

	virtual const AABB& GetBoundBox() const = 0;
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
	virtual void GetUpdatedRenderDesc(SRenderDesc* pDestDesc) = 0;
};





SP_NMSPACE_END

#endif
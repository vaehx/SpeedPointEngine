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
//
//////////////////////////////////////////////////////////////////////////////////

#ifndef __ivisibleobject_h__
#define __ivisibleobject_h__

#if _MSC_VER > 1000
#pragma once
#endif

#include <SPrerequisites.h>
#include "ITexture.h"
#include "Transformable.h"
#include "BoundBox.h"
#include <vector>

using std::vector;

SP_NMSPACE_BEG

struct S_API IIndexBuffer;
struct S_API IVertexBuffer;
struct S_API IGeometry;
struct S_API STransformation;
struct S_API IGameEngine;
struct S_API IRenderer;
struct S_API SInitialGeometryDesc;
struct S_API IMaterial;
struct S_API SAxisAlignedBoundBox;
typedef struct S_API SAxisAlignedBoundBox AABB;
struct S_API SSceneNode;

struct IRenderableComponent;
struct IAnimateableComponent;
struct IPhysicalComponent;
struct IScriptableComponent;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum S_API EObjectType
{
	eGEOMOBJ_STATIC,
	eGEOMOBJ_RIGID, // has physics
	eGEOMOBJ_ENTITY, // all components are optional except scripting
	eGEOMOBJ_CHARACTER, // animateable / skinned
	eGEOMOBJ_VEGETATION,
	eGEOMOBJ_WATER,
	eGEOMOBJ_TERRAIN,
	eGEOMOBJ_SKYBOX,
	eGEOMOBJ_REFERENCE
	// ...
};


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

// abstract, not really interface
class S_API IObject : public STransformable
{
protected:
	AABB m_AABB;

public:
	virtual ~IObject() {}

	virtual void RecalcBoundBox() = 0;
	virtual const AABB& GetBoundBox() const { return m_AABB; }

	virtual EObjectType GetType() const = 0;

	virtual bool IsRenderable() const { return false; }
	virtual bool IsPhysical() const { return false; }
	virtual bool IsAnimateable() const { return false; }
	virtual bool IsScriptable() const { return false; }

	virtual IRenderableComponent* GetRenderable() { return 0; }
	virtual IPhysicalComponent* GetPhysical() { return 0; }
	virtual IAnimateableComponent* GetAnimateable() { return 0; }
	virtual IScriptableComponent* GetScriptable() { return 0; }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


struct S_API ISkyBox
{
};





SP_NMSPACE_END

#endif
//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2014, iSmokiieZz
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

SP_NMSPACE_BEG

struct S_API IIndexBuffer;
struct S_API IVertexBuffer;
struct S_API IGeometry;
struct S_API STransformation;
struct S_API IGameEngine;
struct S_API IRenderer;
struct S_API SInitialGeometryDesc;
struct S_API SMaterial;
struct S_API SAxisAlignedBoundBox;
typedef struct S_API SAxisAlignedBoundBox AABB;


///////////////////////////////////////////////////////////////////////////////////

enum S_API EObjectType
{
	eGEOMOBJ_STATIC,
	eGEOMOBJ_ENTITY,
	eGEOMOBJ_TERRAIN,
	eGEOMOBJ_VEGETATION,
	eGEOMOBJ_WATER
	// ...
};


///////////////////////////////////////////////////////////////////////////////////


struct S_API SInitialMaterials
{
	unsigned short nMaterials;
	SMaterial* pMaterials;
};


///////////////////////////////////////////////////////////////////////////////////

struct S_API IObject
{
	virtual ~IObject() {}

	virtual EObjectType GetType() const = 0;	
	virtual bool IsRenderable() const = 0;
	virtual bool IsTransformable() const = 0;
	virtual void RecalcBoundBox() = 0;
	virtual const AABB& GetBoundBox() const = 0;
};

struct S_API IRenderableObject : public IObject, public STransformable
{
	// Notes:
	// - renderable object not necessarily has geometry.

	virtual SResult Render() = 0;	
	virtual bool IsRenderable() const { return true; }
	virtual bool IsTransformable() const { return true; }

	virtual SResult CreateNormalsGeometry(IRenderableObject** pNormalGeometryObject) const = 0;

	virtual IGeometry* GetGeometry() = 0;	
};


// Summary:
//	An simple object that can be rendered and positioned in the world
//	When implementing, also inherit from STransformable to add support for transformation tools
struct S_API IStaticObject : public IRenderableObject
{
	virtual ~IStaticObject()
	{
	}

	// pInitialMaterials - If nullptr, then the engine's default material is taken
	virtual SResult Init(IGameEngine* pEngine, IRenderer* pRenderer, const SInitialMaterials* pInitialMaterials = nullptr, SInitialGeometryDesc* pInitialGeom = nullptr) = 0;	
		
	virtual SMaterial* GetMaterials() = 0;	
	virtual unsigned short GetMaterialCount() const = 0;

	// Summary:
	//	Sets first material (=single Material)
	virtual void SetMaterial(const SMaterial& singleMat) = 0;

	// Summary:
	//	Verifies that there is a single material and returns always a non-null pointer
	virtual SMaterial* GetSingleMaterial() = 0;
	
	virtual void Clear() = 0;
};


struct S_API IWaterVolume
{
};

struct S_API IOcean
{
};

struct S_API ISkyBox
{
};





SP_NMSPACE_END

#endif
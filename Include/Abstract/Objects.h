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

SP_NMSPACE_BEG

struct S_API ITexture;
struct S_API IIndexBuffer;
struct S_API IVertexBuffer;
struct S_API IGeometry;
struct S_API STransformation;
struct S_API IMaterial;
struct S_API IGameEngine;
struct S_API IRenderer;
struct S_API SInitialGeometryDesc;


enum S_API EObjectType
{
	eGEOMOBJ_STATIC,
	eGEOMOBJ_ENTITY,
	eGEOMOBJ_TERRAIN,
	eGEOMOBJ_VEGETATION,
	eGEOMOBJ_WATER
	// ...
};



struct S_API IObject
{
	virtual ~IObject() {}

	virtual EObjectType GetType() const = 0;
	//virtual SBoundBox& GetBoundBox() = 0;
};


// Summary:
//	An simple object that can be rendered and positioned in the world
//	When implementing, also inherit from STransformable to add support for transformation tools
struct S_API IStaticObject : public IObject
{
	virtual ~IStaticObject()
	{
	}

	virtual SResult Init(IGameEngine* pEngine, IRenderer* pRenderer, SInitialGeometryDesc* pInitialGeom = nullptr) = 0;
	virtual SResult Init(IGameEngine* pEngine, IRenderer* pRenderer, const Material& material, SInitialGeometryDesc* pInitialGeom = nullptr) = 0;

	virtual IGeometry* GetGeometry() = 0;	
	virtual IMaterial* GetMaterial() = 0;	

	virtual SResult Render() = 0;
	virtual void Clear() = 0;
};



struct S_API ITerrain
{
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
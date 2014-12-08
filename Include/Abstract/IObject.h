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

SP_NMSPACE_BEG

struct S_API IIndexBuffer;
struct S_API IVertexBuffer;
struct S_API IGeometry;
struct S_API STransformation;
struct S_API IGameEngine;
struct S_API IRenderer;
struct S_API SInitialGeometryDesc;


///////////////////////////////////////////////////////////////////////////////////

// Summary:
//	Description of the material of an objects surface.
//	The texture are only pointers (aggregations) and are not destructed by Material.
struct S_API Material
{
	ITexture* textureMap;	// aggregation, color for full unlit roughness
	ITexture* normalMap;
	ITexture* ambientOcclusionMap;
	float emissive;

	ITexture* glossinessMap;
	float globalGlossFactor;
	bool useGlobalGloss;

	Material()
		: textureMap(nullptr),
		normalMap(nullptr),
		ambientOcclusionMap(nullptr),
		emissive(0.0f),
		glossinessMap(nullptr),
		globalGlossFactor(0.0f),
		useGlobalGloss(true)
	{
	}

	Material(const Material& m)
		: textureMap(m.textureMap),
		normalMap(m.normalMap),
		ambientOcclusionMap(m.ambientOcclusionMap),
		emissive(m.emissive),
		glossinessMap(m.glossinessMap),
		globalGlossFactor(m.globalGlossFactor),
		useGlobalGloss(m.useGlobalGloss)
	{
	}

	~Material()
	{
		Clear();
	}

	void Clear()
	{
		textureMap = nullptr;
		ambientOcclusionMap = nullptr;
		glossinessMap = nullptr;
	}
};



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



struct S_API IObject
{
	virtual ~IObject() {}

	virtual EObjectType GetType() const = 0;	
	virtual bool IsRenderable() const = 0;	
	//virtual SBoundBox& GetBoundBox() = 0;
};

struct S_API IRenderableObject : public IObject
{
	// Notes:
	// - renderable object not necessarily has geometry.

	virtual SResult Render() = 0;	
	virtual bool IsRenderable() const { return true; }
};


// Summary:
//	An simple object that can be rendered and positioned in the world
//	When implementing, also inherit from STransformable to add support for transformation tools
struct S_API IStaticObject : public IRenderableObject
{
	virtual ~IStaticObject()
	{
	}

	virtual SResult Init(IGameEngine* pEngine, IRenderer* pRenderer, SInitialGeometryDesc* pInitialGeom = nullptr) = 0;
	virtual SResult Init(IGameEngine* pEngine, IRenderer* pRenderer, const Material& material, SInitialGeometryDesc* pInitialGeom = nullptr) = 0;

	virtual IGeometry* GetGeometry() = 0;	
	virtual Material& GetMaterial() = 0;	
	
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
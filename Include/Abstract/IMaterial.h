//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2015, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	Material.h
// Created:	12/22/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SPrerequisites.h>
#include "Vector3.h"


SP_NMSPACE_BEG

struct S_API ITexture;


struct S_API SShaderResources
{
	ITexture* textureMap;	// aggregation, color for full unlit roughness
	ITexture* normalMap;
	ITexture* ambientOcclusionMap;
	float3 emissive;

	ITexture* roughnessMap;
	float roughness;	// if the glossiness Map is set it is used instead of the global gloss factor

	SShaderResources()
		: textureMap(0),
		normalMap(0),
		ambientOcclusionMap(0),
		emissive(0, 0, 0),
		roughnessMap(0),
		roughness(1.0f)
	{
	}

	SShaderResources(const SShaderResources& src)
	{
		CopyFrom(src);
	}

	SShaderResources& operator = (const SShaderResources& src)
	{
		CopyFrom(src);
		return *this;
	}

	void CopyFrom(const SShaderResources& src)
	{
		textureMap = src.textureMap;
		normalMap = src.normalMap;
		ambientOcclusionMap = src.ambientOcclusionMap;
		emissive = src.emissive;
		roughnessMap = src.roughnessMap;
		roughness = src.roughness;
	}
};

// Material Layer
struct S_API SMaterialLayer
{
	SShaderResources resources;

	SMaterialLayer() {}
	SMaterialLayer(const SMaterialLayer& layer)
		: resources(layer.resources) {}

	SMaterialLayer& operator =(const SMaterialLayer& layer)
	{
		resources = layer.resources;
		return *this;
	}
};



////////////////////////////////////////////////////////////////////////////////////////////////
// SpeedPoint Material
////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_MAT_LAYER_COUNT

struct S_API IMaterial
{	
	virtual ~IMaterial() {}

	virtual void SetName(const char* name) = 0;
	virtual const char* GetName() = 0;
	
	// Initially, the material has 1 layer
	virtual void SetLayerCount(unsigned int layers) = 0;

	// returns 0 if index > layer count. Initially, the material has 1 layer
	virtual SMaterialLayer* GetLayer(unsigned int index) = 0;	

	virtual void Clear() = 0;
};









////////////////////////////////////////////////////////////////////////////////////////////////
// Material Manager
////////////////////////////////////////////////////////////////////////////////////////////////

struct S_API IMaterialManager
{
	virtual ~IMaterialManager() {}
	
	// name is case-insensitive
	virtual IMaterial* FindMaterial(const char* name) = 0;

	// name is case-insensitive
	virtual IMaterial* CreateMaterial(const char* name) = 0;

	virtual void RemoveMaterial(const char* name) = 0;
	virtual void RemoveMaterial(IMaterial** pMat) = 0;

	virtual IMaterial* GetDefaultMaterial() = 0;

	virtual void Clear() = 0;
};


SP_NMSPACE_END
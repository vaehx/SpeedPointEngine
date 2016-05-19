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
#include "IGameEngine.h"
#include <string>


using std::string;

SP_NMSPACE_BEG

struct S_API ITexture;
struct S_API SInitialGeometryDesc;

enum S_API EIllumModel
{
	eILLUM_HELPER,	// solid color
	eILLUM_SKYBOX,
	eILLUM_PHONG,
	eILLUM_BLINNPHONG,
	eILLUM_COOKTORRANCE
};

struct S_API SShaderResources
{
	ITexture* textureMap;	// aggregation, color for full unlit roughness
	ITexture* normalMap;
	ITexture* ambientOcclusionMap;
	float3 diffuse;
	float3 emissive;	

	EIllumModel illumModel;

	ITexture* roughnessMap;
	float roughness;	// if the glossiness Map is set it is used instead of the global gloss factor

	bool enableBackfaceCulling;

	SShaderResources()
		: textureMap(0),
		normalMap(0),
		ambientOcclusionMap(0),
		diffuse(0, 0, 0),		
		emissive(0, 0, 0),		
		roughnessMap(0),
		roughness(1.0f),
		illumModel(eILLUM_BLINNPHONG),
		enableBackfaceCulling(true)
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
		diffuse = src.diffuse;		
		illumModel = src.illumModel;
		roughnessMap = src.roughnessMap;
		roughness = src.roughness;
		enableBackfaceCulling = src.enableBackfaceCulling;
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

	virtual void SetName(const string& name) = 0;
	virtual const string& GetName() const = 0;
	
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
	
	// Loads all materials from the given bank file into memory, overriding existing materials with same name.
	// While loading the material, 
	virtual void LoadMaterialBank(const string& smbFile) = 0;

	// Summary:
	//		Returns the material object with the given name.
	//		If there is no such object, it will be created and the pointer returned.
	//		If there are multiple materials with the given name, the first one found will be returned.
	// Arguments:
	//		name - is case sensitive
	virtual IMaterial* GetMaterial(const string& name) = 0;

	// Does the same as GetMaterial()
	virtual IMaterial* CreateMaterial(const string& name)
	{
		return GetMaterial(name);
	}

	virtual void RemoveMaterial(const string& name) = 0;
	virtual void RemoveMaterial(IMaterial** pMat) = 0;

	virtual IMaterial* GetDefaultMaterial() = 0;
	virtual void LogAllMaterials() = 0;
	virtual void Clear() = 0;
};


SP_NMSPACE_END

//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2014, iSmokiieZz
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


SP_NMSPACE_BEG

struct S_API ITexture;

// Summary:
//	Description of the material of an objects surface.
//	The texture are only pointers (aggregations) and are not destructed by Material.
struct S_API SMaterial
{
	SString name;

	SColor ambientColor;
	SColor diffuseColor;
	SColor specularColor;

	ITexture* textureMap;	// aggregation, color for full unlit roughness
	ITexture* normalMap;
	ITexture* ambientOcclusionMap;
	float emissive;

	ITexture* glossinessMap;
	float globalGlossFactor;
	bool useGlobalGloss;

	SMaterial()
		: name(),
		textureMap(nullptr),
		normalMap(nullptr),
		ambientOcclusionMap(nullptr),
		emissive(0.0f),
		glossinessMap(nullptr),
		globalGlossFactor(0.0f),
		useGlobalGloss(true)
	{
	}

	SMaterial(const SMaterial& m)
		: textureMap(m.textureMap),
		normalMap(m.normalMap),
		ambientOcclusionMap(m.ambientOcclusionMap),
		emissive(m.emissive),
		glossinessMap(m.glossinessMap),
		globalGlossFactor(m.globalGlossFactor),
		useGlobalGloss(m.useGlobalGloss)
	{		
		name = m.name;
	}

	SMaterial& operator = (const SMaterial& m)
	{
		textureMap = m.textureMap;
		normalMap = m.normalMap;
		ambientOcclusionMap = m.ambientOcclusionMap;
		emissive = m.emissive;
		glossinessMap = m.glossinessMap;
		globalGlossFactor = m.globalGlossFactor;
		useGlobalGloss = m.useGlobalGloss;
		name = m.name;
		return *this;
	}

	~SMaterial()
	{
		Clear();
	}

	void Clear()
	{		
		textureMap = nullptr;
		normalMap = nullptr;
		ambientOcclusionMap = nullptr;
		glossinessMap = nullptr;		
	}
};


SP_NMSPACE_END
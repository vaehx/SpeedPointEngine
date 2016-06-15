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
#include "IRenderer.h"
#include "Vector3.h"
#include <string>
#include <vector>


using std::string;
using std::vector;

SP_NMSPACE_BEG

struct S_API SInitialGeometryDesc;

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

	// Returns a list of all material names
	virtual void ListMaterials(vector<string>& list) const = 0;

	virtual void Clear() = 0;
};


SP_NMSPACE_END

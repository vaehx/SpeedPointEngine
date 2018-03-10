//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Renderer\IRenderer.h>
#include <Common\SPrerequisites.h>
#include <Common\Vector3.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

SP_NMSPACE_BEG

struct S_API SMaterialDefinition
{
	string name;
	string textureMap;
	string normalMap;
	string roughnessMap;
	float roughness; // ignored if roughnessMap valid
	float metalness;
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

	// Changes the count of definitions in this material.
	// If new count > current count: Existing definitions are kept and additional space allocated.
	// If new count < current count: Definitions with index >= new count will be removed
	//
	// By default, 1 default definition is already allocated.
	// There must be at least one definition.
	// If numDefinitions is 0, it will be set to 1 first.
	virtual void AllocateDefinitions(unsigned int numDefinitions) = 0;

	virtual unsigned int GetDefinitionCount() const = 0;
	virtual SMaterialDefinition* GetDefinition(unsigned int idx = 0) = 0;
	virtual SMaterialDefinition* GetDefinition(const string& name) = 0;
	
	// Returns UINT_MAX if not found
	virtual unsigned int GetDefinitionIndex(const string& name) const = 0;
};



////////////////////////////////////////////////////////////////////////////////////////////////
// Material Manager
////////////////////////////////////////////////////////////////////////////////////////////////

struct S_API IMaterialManager
{
	virtual ~IMaterialManager() {}

	// Summary:
	//		Returns the material object with the given name or 0 if not found.
	//		If this material is loaded from file, the specification is the absolute resource
	//		path to the material file.
	// Arguments:
	//		specification - case-sensitive
	virtual IMaterial* GetMaterial(const string& specification) = 0;

	// Summary:
	//		Loads the material from file only if it was not loaded yet. Otherwise a pointer to
	//		the existing material instance is returned.
	//		The absResourcePath will be used as the specification for the material.
	virtual IMaterial* LoadMaterial(const string& absResourcePath) = 0;

	// Summary:
	//		Creates a new, empty material with the given specification.
	//		If a material with that specification already exists, a pointer to that material is returned
	virtual IMaterial* CreateMaterial(const string& specification) = 0;

	virtual void RemoveMaterial(const string& specification) = 0;
	virtual void RemoveMaterial(IMaterial** pMat) = 0;

	virtual IMaterial* GetDefaultMaterial() = 0;

	virtual void GetAllMaterialSpecifications(vector<string>& list) const = 0;

	virtual void Clear() = 0;
};


SP_NMSPACE_END

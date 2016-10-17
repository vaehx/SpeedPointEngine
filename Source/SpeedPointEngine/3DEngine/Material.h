//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2015, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	Material.h
// Created:	8/18/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Abstract\IMaterial.h>
#include <Abstract\ChunkedObjectPool.h>

SP_NMSPACE_BEG

struct S_API IResourcePool;

class S_API Material : public IMaterial
{
private:
	string m_Name;
	SMaterialLayer* m_pLayers;
	unsigned int m_nLayers;

public:
	Material();

	virtual ~Material()
	{
		Clear();
	}

	virtual void SetName(const string& name);
	virtual const string& GetName() const;
	
	virtual void SetLayerCount(unsigned int layers);
	virtual unsigned int GetLayerCount() const { return m_nLayers; }
	virtual SMaterialLayer* GetLayer(unsigned int index);

	virtual void Clear();
};





class S_API MaterialManager : public IMaterialManager
{
private:
	ChunkedObjectPool<Material> m_Materials;
	Material m_DefMat;

	// Returns 0 if not found in contrast to GetMaterial() which would return a new object
	Material* FindMaterial(const string& name);

public:
	MaterialManager();

	virtual void LoadMaterialBank(const string& smbFile);

	virtual void ListMaterials(vector<string>& list) const;

	virtual IMaterial* GetMaterial(const string& name);

	virtual void RemoveMaterial(const string& name);
	virtual void RemoveMaterial(IMaterial** pMat);

	virtual IMaterial* GetDefaultMaterial()
	{
		return &m_DefMat;
	}

	virtual void Clear();
};


SP_NMSPACE_END

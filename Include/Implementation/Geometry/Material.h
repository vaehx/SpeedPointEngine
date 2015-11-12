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

class S_API BasicMaterial : public IMaterial
{
private:
	char* m_pName;
	SMaterialLayer* m_pLayers;
	unsigned int m_nLayers;

public:
	BasicMaterial();

	virtual ~BasicMaterial()
	{
		Clear();
	}

	virtual void SetName(const char* name);
	virtual const char* GetName();
	
	virtual void SetLayerCount(unsigned int layers);
	virtual SMaterialLayer* GetLayer(unsigned int index);

	virtual void Clear();
};





class S_API BasicMaterialManager : public IMaterialManager
{
private:
	ChunkedObjectPool<BasicMaterial> m_Materials;
	BasicMaterial m_DefMat;	

public:
	BasicMaterialManager();

	virtual void LogAllMaterials();

	virtual IMaterial* FindMaterial(const char* name);
	virtual IMaterial* CreateMaterial(const char* name);

	virtual void RemoveMaterial(const char* name);
	virtual void RemoveMaterial(IMaterial** pMat);

	virtual IMaterial* GetDefaultMaterial()
	{
		return &m_DefMat;
	}

	virtual void Clear();
};


SP_NMSPACE_END
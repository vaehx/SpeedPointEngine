//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\IMaterial.h"
#include "..\I3DEngine.h"
#include <Common\ChunkedObjectPool.h>

SP_NMSPACE_BEG

//////////////////////////////////////////////////////////////////////////////////////////////////

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


//////////////////////////////////////////////////////////////////////////////////////////////////

class S_API MaterialManager : public IMaterialManager
{
private:
	I3DEngine* m_p3DEngine;
	ChunkedObjectPool<Material> m_Materials;
	Material m_DefMat;

public:
	MaterialManager(I3DEngine* p3DEngine);

	virtual IMaterial* GetMaterial(const string& specification);
	virtual IMaterial* LoadMaterial(const string& absResourcePath);
	virtual IMaterial* CreateMaterial(const string& specification);

	virtual void RemoveMaterial(const string& name);
	virtual void RemoveMaterial(IMaterial** pMat);

	virtual IMaterial* GetDefaultMaterial()
	{
		return &m_DefMat;
	}

	virtual void GetAllMaterialSpecifications(vector<string>& list) const;

	virtual void Clear();
};


SP_NMSPACE_END

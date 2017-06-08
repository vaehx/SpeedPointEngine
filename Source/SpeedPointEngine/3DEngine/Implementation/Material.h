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
	SMaterialDefinition* m_pDefinitions;
	unsigned int m_nDefinitions;

public:
	Material();
	virtual ~Material();

	virtual void SetName(const string& name);
	virtual const string& GetName() const;
	
	virtual void AllocateDefinitions(unsigned int numDefinitions);
	virtual unsigned int GetDefinitionCount() const { return m_nDefinitions; }
	virtual SMaterialDefinition* GetDefinition(unsigned int idx = 0);
	virtual SMaterialDefinition* GetDefinition(const string& name);
	virtual unsigned int GetDefinitionIndex(const string& name) const;
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

//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2015, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	Material.cpp
// Created:	8/19/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#include "Material.h"
#include <Abstract\IResourcePool.h>
#include <Abstract\IGeometry.h>
#include <FileSMB.h>

SP_NMSPACE_BEG

///////////////////////////////////////////////////////////////////////////////////////////
S_API Material::Material()
{
	m_Name = "unknown";
	m_pLayers = new SMaterialLayer[1];
	m_nLayers = 1;
}

///////////////////////////////////////////////////////////////////////////////////////////
S_API void Material::SetName(const string& name)
{
	m_Name = name;
}

///////////////////////////////////////////////////////////////////////////////////////////
S_API const string& Material::GetName() const
{
	return m_Name;
}

///////////////////////////////////////////////////////////////////////////////////////////
S_API void Material::SetLayerCount(unsigned int layers)
{
	SMaterialLayer* pOldLayers = m_pLayers;
	unsigned int nOldLayers = m_nLayers;

	m_nLayers = layers;
	m_pLayers = new SMaterialLayer[m_nLayers];
	if (IS_VALID_PTR(pOldLayers) && nOldLayers > 0)
	{
		memcpy(m_pLayers, pOldLayers, sizeof(SMaterialLayer) * min(nOldLayers, m_nLayers));
		delete[] pOldLayers;
		pOldLayers = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
S_API SMaterialLayer* Material::GetLayer(unsigned int index)
{	
	if (index >= m_nLayers || !IS_VALID_PTR(m_pLayers))
		return 0;	

	return &m_pLayers[index];
}

///////////////////////////////////////////////////////////////////////////////////////////
S_API void Material::Clear()
{
	if (IS_VALID_PTR(m_pLayers))
		delete[] m_pLayers;

	m_nLayers = 0;
	m_pLayers = 0;
	m_Name = "?CLEARED?";
}





///////////////////////////////////////////////////////////////////////////////////////////
S_API MaterialManager::MaterialManager()
{
}

///////////////////////////////////////////////////////////////////////////////////////////
S_API void MaterialManager::LoadMaterialBank(const string& smbFile)
{
	vector<SSMBMaterial> loadedMaterials;
	CSMBLoader smbLoader;
	smbLoader.ReadSMBFile(smbFile.c_str(), loadedMaterials);
	for (auto& itMaterial = loadedMaterials.begin(); itMaterial != loadedMaterials.end(); ++itMaterial)
	{		
		IMaterial* pNewMat = GetMaterial(itMaterial->name.c_str());
		
		SMaterialLayer* layer = pNewMat->GetLayer(0);
		layer->roughness = itMaterial->roughness;
		layer->textureMap = itMaterial->textureMap;
		layer->normalMap = itMaterial->normalMap;
		layer->roughnessMap = itMaterial->roughnessMap;
		// ...

		CLog::Log(S_DEBUG, "Loaded mat '%s' (tm='%s',nm='%s',rm='%s',roughness=%.2f)",
			itMaterial->name.c_str(),
			layer->textureMap.c_str(),
			layer->normalMap.c_str(),
			layer->roughnessMap.c_str(),
			layer->roughness);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
S_API Material* MaterialManager::FindMaterial(const string& name)
{
	if (name.empty())
		return 0;

	if (m_Materials.GetUsedObjectCount() == 0)
		return 0;

	unsigned int iterator;
	Material* pMat = m_Materials.GetFirstUsedObject(iterator);
	while (pMat)
	{
		if (!IS_VALID_PTR(pMat))
			break;

		if (pMat->GetName() == name)
			return pMat;

		pMat = m_Materials.GetNextUsedObject(iterator);
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////
S_API IMaterial* MaterialManager::GetMaterial(const string& name)
{
	Material* pMaterial = FindMaterial(name);
	if (!IS_VALID_PTR(pMaterial))
	{
		// Create new
		pMaterial = m_Materials.Get();
		pMaterial->SetName(name);
	}

	return pMaterial;
}

///////////////////////////////////////////////////////////////////////////////////////////
S_API void MaterialManager::RemoveMaterial(const string& name)
{
	IMaterial* pMat = FindMaterial(name);
	RemoveMaterial(&pMat);
}

///////////////////////////////////////////////////////////////////////////////////////////
S_API void MaterialManager::RemoveMaterial(IMaterial** ppMat)
{
	if (!IS_VALID_PTR(ppMat))
		return;

	Material* pMat = dynamic_cast<Material*>(*ppMat);
	if (IS_VALID_PTR(pMat))
		m_Materials.Release(&pMat);

	*ppMat = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////
S_API void MaterialManager::ListMaterials(vector<string>& list) const
{
	if (m_Materials.GetUsedObjectCount() > 0)
	{
		unsigned int iterator;
		Material* pMat = m_Materials.GetFirstUsedObject(iterator);
		while (pMat)
		{
			if (!IS_VALID_PTR(pMat))
				break;

			list.push_back(pMat->GetName());

			pMat = m_Materials.GetNextUsedObject(iterator);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
S_API void MaterialManager::Clear()
{
	m_Materials.Clear();
}

SP_NMSPACE_END

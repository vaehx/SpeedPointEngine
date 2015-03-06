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

#include <Implementation\Geometry\Material.h>
#include <Abstract\IResourcePool.h>

SP_NMSPACE_BEG

///////////////////////////////////////////////////////////////////////////////////////////
S_API BasicMaterial::BasicMaterial()
{
	m_pName = new char[16];
	SPSPrintf(m_pName, 16, "unknown");

	m_pLayers = new SMaterialLayer[1];
	m_nLayers = 1;	
}

///////////////////////////////////////////////////////////////////////////////////////////
S_API void BasicMaterial::SetName(const char* name)
{
	if (IS_VALID_PTR(m_pName))
		delete[] m_pName;

	m_pName = 0;
	sp_strcpy(&m_pName, name);
}

///////////////////////////////////////////////////////////////////////////////////////////
S_API const char* BasicMaterial::GetName()
{
	return m_pName;
}

///////////////////////////////////////////////////////////////////////////////////////////
S_API void BasicMaterial::SetLayerCount(unsigned int layers)
{
	SMaterialLayer* pOldLayers = m_pLayers;
	unsigned int nOldLayers = m_nLayers;

	m_nLayers = layers;
	m_pLayers = new SMaterialLayer[m_nLayers];
	if (IS_VALID_PTR(pOldLayers) && nOldLayers > 0)
	{
		memcpy(m_pLayers, pOldLayers, sizeof(SMaterialLayer) * min(nOldLayers, m_nLayers));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
S_API SMaterialLayer* BasicMaterial::GetLayer(unsigned int index)
{	
	if (index >= m_nLayers || !IS_VALID_PTR(m_pLayers))
		return 0;	

	return &m_pLayers[index];
}

///////////////////////////////////////////////////////////////////////////////////////////
S_API void BasicMaterial::Clear()
{
	if (IS_VALID_PTR(m_pLayers))
		delete[] m_pLayers;

	m_nLayers = 0;
	m_pLayers = 0;

	if (IS_VALID_PTR(m_pName))
		delete[] m_pName;

	m_pName = 0;
}













///////////////////////////////////////////////////////////////////////////////////////////
S_API BasicMaterialManager::BasicMaterialManager()
{
	SShaderResources& resources = m_DefMat.GetLayer(0)->resources;
	//resources.textureMap = ...;
}


///////////////////////////////////////////////////////////////////////////////////////////
S_API IMaterial* BasicMaterialManager::FindMaterial(const char* name)
{	
	if (m_Materials.GetUsedObjectCount() == 0)
		return 0;

	unsigned int iterator = 0;
	BasicMaterial* pMat;
	while (IS_VALID_PTR((pMat = m_Materials.GetNextUsedObject(iterator))))
	{	
		if (!IS_VALID_PTR(pMat))
			break;

		if (strcmp(pMat->GetName(), name))
			return pMat;
	}

	CLog::Log(S_ERROR, "Could not find material '%s'.", name);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////
S_API IMaterial* BasicMaterialManager::CreateMaterial(const char* name)
{
	return m_Materials.Get();
}

///////////////////////////////////////////////////////////////////////////////////////////
S_API void BasicMaterialManager::RemoveMaterial(const char* name)
{
	BasicMaterial* pMat = dynamic_cast<BasicMaterial*>(FindMaterial(name));
	
	if (IS_VALID_PTR(pMat))
		m_Materials.Release(&pMat);
}

///////////////////////////////////////////////////////////////////////////////////////////
S_API void BasicMaterialManager::RemoveMaterial(IMaterial** pMat)
{
	// Todo: Find a better way to cast to BasicMaterial**
	m_Materials.Release((BasicMaterial**)(pMat));
}

///////////////////////////////////////////////////////////////////////////////////////////
S_API void BasicMaterialManager::Clear()
{
	m_Materials.Clear();
}



SP_NMSPACE_END
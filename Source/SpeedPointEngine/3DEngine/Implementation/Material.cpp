//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "Material.h"
#include "..\IGeometry.h"
#include <Renderer\IResourcePool.h>
#include <FileMAT.h>
#include "C3DEngine.h"

SP_NMSPACE_BEG

//////////////////////////////////////////////////////////////////////////////////////////////////
S_API Material::Material()
{
	m_Name = "unknown";
	m_pLayers = new SMaterialLayer[1];
	m_nLayers = 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
S_API void Material::SetName(const string& name)
{
	m_Name = name;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
S_API const string& Material::GetName() const
{
	return m_Name;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////////////////////////
S_API SMaterialLayer* Material::GetLayer(unsigned int index)
{	
	if (index >= m_nLayers || !IS_VALID_PTR(m_pLayers))
		return 0;	

	return &m_pLayers[index];
}

//////////////////////////////////////////////////////////////////////////////////////////////////
S_API void Material::Clear()
{
	if (IS_VALID_PTR(m_pLayers))
		delete[] m_pLayers;

	m_nLayers = 0;
	m_pLayers = 0;
	m_Name = "?CLEARED?";
}





//////////////////////////////////////////////////////////////////////////////////////////////////
S_API MaterialManager::MaterialManager(I3DEngine* p3DEngine)
	: m_p3DEngine(p3DEngine)
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////
S_API IMaterial* MaterialManager::GetMaterial(const string& specification)
{
	if (specification.empty())
		return 0;

	if (m_Materials.GetUsedObjectCount() == 0)
		return 0;

	unsigned int iterator;
	Material* pMat = m_Materials.GetFirstUsedObject(iterator);
	while (pMat)
	{
		if (!IS_VALID_PTR(pMat))
			break;

		if (pMat->GetName() == specification)
			return pMat;

		pMat = m_Materials.GetNextUsedObject(iterator);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
S_API IMaterial* MaterialManager::LoadMaterial(const string& absResourcePath)
{
	if (absResourcePath.empty())
		return 0;
	
	IMaterial* pMaterial = GetMaterial(absResourcePath);
	if (pMaterial)
		return pMaterial;

	IResourcePool* pResources = m_p3DEngine->GetRenderer()->GetResourcePool();

	MATFile mat;
	if (!mat.ReadFromFile(pResources->GetResourceSystemPath(absResourcePath)))
	{
		CLog::Log(S_ERROR, "Failed load material '%s'", absResourcePath.c_str());
		return 0;
	}

	pMaterial = m_Materials.Get();
	pMaterial->SetName(absResourcePath);

	SMaterialLayer* layer = pMaterial->GetLayer(0);
	layer->roughness = mat.material.roughness;
	layer->textureMap = mat.material.textureMap;
	layer->normalMap = mat.material.normalMap;
	layer->roughnessMap = mat.material.roughnessMap;

	CLog::Log(S_DEBUG, "Loaded material '%s'", absResourcePath.c_str());

	return pMaterial;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
S_API IMaterial* MaterialManager::CreateMaterial(const string& specification)
{
	IMaterial* pMaterial = GetMaterial(specification);
	if (pMaterial)
		return pMaterial;

	pMaterial = m_Materials.Get();
	pMaterial->SetName(specification);

	CLog::Log(S_DEBUG, "Created material '%s'", specification);

	return pMaterial;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
S_API void MaterialManager::RemoveMaterial(const string& name)
{
	IMaterial* pMat = GetMaterial(name);
	RemoveMaterial(&pMat);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
S_API void MaterialManager::RemoveMaterial(IMaterial** ppMat)
{
	if (!IS_VALID_PTR(ppMat))
		return;

	Material* pMat = dynamic_cast<Material*>(*ppMat);
	if (IS_VALID_PTR(pMat))
		m_Materials.Release(&pMat);

	*ppMat = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
S_API void MaterialManager::GetAllMaterialSpecifications(vector<string>& list) const
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

//////////////////////////////////////////////////////////////////////////////////////////////////
S_API void MaterialManager::Clear()
{
	m_Materials.Clear();
}

SP_NMSPACE_END

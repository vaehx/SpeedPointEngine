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

//---------------------------------------------------------------------------------------------------------------------
S_API Material::Material()
{
	m_Name = "unknown";
	m_pDefinitions = new SMaterialDefinition[1];
	m_nDefinitions = 1;
}

//---------------------------------------------------------------------------------------------------------------------
S_API Material::~Material()
{
	if (m_pDefinitions)
		delete[] m_pDefinitions;

	m_pDefinitions = 0;
	m_nDefinitions = 0;
}

//---------------------------------------------------------------------------------------------------------------------
S_API void Material::SetName(const string& name)
{
	m_Name = name;
}

//---------------------------------------------------------------------------------------------------------------------
S_API const string& Material::GetName() const
{
	return m_Name;
}

//---------------------------------------------------------------------------------------------------------------------
S_API void Material::AllocateDefinitions(unsigned int numDefinitions)
{
	if (numDefinitions < 1)
	{
		CLog::Log(S_WARNING, "Material::AllocateDefinitions(): numDefinitions = 0. There must be at least 1 definition.");
		numDefinitions = 1;
	}
	
	if (numDefinitions == m_nDefinitions)
		return;

	SMaterialDefinition* pNewDefinitions = new SMaterialDefinition[numDefinitions];
	if (m_nDefinitions > 0 && m_pDefinitions)
	{
		// We can't memcpy here, as we will destroy the std::strings later...
		for (unsigned int i = 0; i < m_nDefinitions; ++i)
			pNewDefinitions[i] = m_pDefinitions[i];
	}
	
	if (m_pDefinitions)
		delete[] m_pDefinitions;

	m_pDefinitions = pNewDefinitions;
	m_nDefinitions = numDefinitions;
}

//---------------------------------------------------------------------------------------------------------------------
S_API SMaterialDefinition* Material::GetDefinition(unsigned int index /*= 0*/)
{	
	if (index < m_nDefinitions && m_pDefinitions)
		return &m_pDefinitions[index];
	else
		return 0;
}

//---------------------------------------------------------------------------------------------------------------------
S_API SMaterialDefinition* Material::GetDefinition(const string& name)
{
	return GetDefinition(GetDefinitionIndex(name));
}

//---------------------------------------------------------------------------------------------------------------------
S_API unsigned int Material::GetDefinitionIndex(const string& name) const
{
	if (!name.empty() && m_pDefinitions)
	{
		for (unsigned int i = 0; i < m_nDefinitions; ++i)
		{
			if (m_pDefinitions[i].name == name)
				return i;
		}
	}

	return UINT_MAX;
}



//////////////////////////////////////////////////////////////////////////////////////////////////





//---------------------------------------------------------------------------------------------------------------------
S_API MaterialManager::MaterialManager(I3DEngine* p3DEngine)
	: m_p3DEngine(p3DEngine)
{
}

//---------------------------------------------------------------------------------------------------------------------
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

//---------------------------------------------------------------------------------------------------------------------
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
		CLog::Log(S_ERROR, "MaterialManager::LoadMaterial(): Failed load material file '%s'", absResourcePath.c_str());
		return 0;
	}

	pMaterial = m_Materials.Get();
	pMaterial->SetName(absResourcePath);
	
	unsigned int numMaterials = mat.materials.size();
	if (numMaterials > 0)
	{
		pMaterial->AllocateDefinitions(numMaterials);

		for (unsigned int i = 0; i < numMaterials; ++i)
		{
			auto& material = mat.materials[i];
			SMaterialDefinition* definition = pMaterial->GetDefinition(i);
			definition->name			= material.name;
			definition->roughness		= material.roughness;
			definition->textureMap		= material.textureMap;
			definition->normalMap		= material.normalMap;
			definition->roughnessMap	= material.roughnessMap;
			definition->metalness		= material.metalness;
		}
	}

	CLog::Log(S_DEBUG, "Loaded material file '%s' (%u definitions)", absResourcePath.c_str(), numMaterials);

	return pMaterial;
}

//---------------------------------------------------------------------------------------------------------------------
S_API IMaterial* MaterialManager::CreateMaterial(const string& specification)
{
	IMaterial* pMaterial = GetMaterial(specification);
	if (pMaterial)
		return pMaterial;

	pMaterial = m_Materials.Get();
	pMaterial->SetName(specification);

	CLog::Log(S_DEBUG, "Created material '%s'", specification.c_str());

	return pMaterial;
}

//---------------------------------------------------------------------------------------------------------------------
S_API void MaterialManager::RemoveMaterial(const string& name)
{
	IMaterial* pMat = GetMaterial(name);
	RemoveMaterial(&pMat);
}

//---------------------------------------------------------------------------------------------------------------------
S_API void MaterialManager::RemoveMaterial(IMaterial** ppMat)
{
	if (!IS_VALID_PTR(ppMat))
		return;

	Material* pMat = dynamic_cast<Material*>(*ppMat);
	if (IS_VALID_PTR(pMat))
	{
		m_Materials.Release(&pMat);
	}

	*ppMat = 0;
}

//---------------------------------------------------------------------------------------------------------------------
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

//---------------------------------------------------------------------------------------------------------------------
S_API void MaterialManager::Clear()
{
	m_Materials.Clear();
}

SP_NMSPACE_END

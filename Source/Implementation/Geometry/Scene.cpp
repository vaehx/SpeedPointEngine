#include <Abstract\IGeometry.h>
#include <Implementation\Geometry\Scene.h>
#include <Implementation\Geometry\Entity.h>
#include <FileSPM.h>
#include <Abstract\IResourcePool.h>
#include <Abstract\IMaterial.h>

#include <Implementation\Geometry\RenderableComponent.h>
#include <Implementation\Geometry\PhysicalComponent.h>

#include <Abstract\I3DEngine.h>
#include <Abstract\IPhysics.h>

SP_NMSPACE_BEG

// -------------------------------------------------------------------------------------------------
S_API Scene::Scene()	
	: m_pEngine(0)
{
}

// -------------------------------------------------------------------------------------------------
S_API void Scene::Clear()
{
	for (auto itEntity = m_Entities.begin(); itEntity != m_Entities.end(); ++itEntity)
	{
		IEntity* pEntity = *itEntity;
		if (pEntity)
		{
			pEntity->Clear();
			delete pEntity;
		}
	}

	m_Entities.clear();

	m_pEngine = 0;
}

// -------------------------------------------------------------------------------------------------
S_API SResult Scene::Initialize(IGameEngine* pGameEngine)
{
	SP_ASSERTR(IS_VALID_PTR(pGameEngine), S_INVALIDPARAM);
	
	m_pEngine = pGameEngine;


	I3DEngine* p3DEngine = m_pEngine->Get3DEngine();
	if (p3DEngine)
		p3DEngine->CreateRenderMeshPool<CRenderableComponent>();
	else
		CLog::Log(S_ERROR, "Cannot create render mesh pool: 3DEngine not initialized");


	IPhysics* pPhysics = m_pEngine->GetPhysics();
	if (pPhysics)
		m_pEngine->GetPhysics()->CreatePhysObjectPool<CPhysicalComponent>();
	else
		CLog::Log(S_ERROR, "Cannot create phys object pool: Physics not initialized");

	return S_SUCCESS;
}

// -------------------------------------------------------------------------------------------------
S_API void Scene::AddObject(IEntity* pEntity)
{
	if (!pEntity)
	{
		CLog::Log(S_ERROR, "Tried to add nullptr entity to scene");
		return;
	}

	m_Entities.push_back(pEntity);
	CLog::Log(S_INFO, "Added '%s' to scene!", pEntity->GetName());
}

// -------------------------------------------------------------------------------------------------
S_API IEntity* Scene::SpawnEntity()
{
	return new CEntity();
}

// -------------------------------------------------------------------------------------------------
S_API IEntity* Scene::LoadObjectFromFile(const char* filename, const char* objName)
{
	SP_ASSERTR(IS_VALID_PTR(m_pEngine), nullptr, "Engine not initialized!");

	// Load SPM Model
	CSPMLoader spmLoader;
	if (!spmLoader.Load(filename, true))
	{
		CLog::Log(S_ERROR, "Failed to load object file '%s'", filename);
		return 0;
	}

	const std::vector<SModelMeta>& models = spmLoader.GetModels();

	SInitialGeometryDesc geomDesc;
	geomDesc.bRequireNormalRecalc = true;
	geomDesc.bRequireTangentRecalc = true;
	geomDesc.indexUsage = eGEOMUSE_STATIC;
	geomDesc.vertexUsage = eGEOMUSE_STATIC;
	geomDesc.primitiveType = PRIMITIVE_TYPE_TRIANGLELIST;

	// Determine subset and vertex count
	for (auto itModel = models.begin(); itModel != models.end(); ++itModel)
	{
		geomDesc.nSubsets += itModel->nLoadedSubsets;
		geomDesc.nVertices += itModel->nVertices;
	}

	geomDesc.pSubsets = new SInitialSubsetGeometryDesc[geomDesc.nSubsets];
	geomDesc.pVertices = new SVertex[geomDesc.nVertices];

	CLog::Log(S_DEBUG, "geomDesc.nSubsets = %d", geomDesc.nSubsets);

	// Flatten-out the subsets in all models into a single array of subsets	
	u32 vtxOffset = 0;
	unsigned int iSubset = 0;
	for (auto itModel = models.begin(); itModel != models.end(); ++itModel)
	{
		// Copy over vertices
		for (u32 iVtx = 0; iVtx < itModel->nVertices; ++iVtx)
		{
			geomDesc.pVertices[vtxOffset + iVtx] = itModel->pVertices[iVtx];
		}

		for (u16 iModelSubset = 0; iModelSubset < itModel->nLoadedSubsets; ++iModelSubset)
		{
			const SSubset& modelSubset = itModel->pSubsets[iModelSubset];
			SInitialSubsetGeometryDesc& subset = geomDesc.pSubsets[iSubset];
			subset.nIndices = modelSubset.nIndices;
			subset.pIndices = new SIndex[subset.nIndices];

			// TODO: Use SIndex in SPMFile, so we don't have to copy the indices here!
			for (u32 iIndex = 0; iIndex < subset.nIndices; ++iIndex)
				subset.pIndices[iIndex] = modelSubset.pIndices[iIndex] + vtxOffset;

			subset.pMaterial = m_pEngine->GetMaterialManager()->GetMaterial(modelSubset.materialName);

			iSubset++;
		}

		vtxOffset += itModel->nVertices;
	}

	// Create the entity
	IEntity* pEntity = SpawnEntity();
	pEntity->SetName(objName);

	SRenderMeshParams meshParams;
	meshParams.pGeomDesc = &geomDesc;
#ifdef _DEBUG
	meshParams._name = objName;
#endif
	pEntity->AddComponent(m_pEngine->Get3DEngine()->CreateMesh(meshParams));
	

	// Cleanup
	delete[] geomDesc.pVertices;

	for (u16 iSubset = 0; iSubset < geomDesc.nSubsets; ++iSubset)
		delete[] geomDesc.pSubsets[iSubset].pIndices;

	delete[] geomDesc.pSubsets;

	for (auto itModel = models.begin(); itModel != models.end(); ++itModel)
	{
		for (u16 iSubset = 0; iSubset < itModel->nSubsets; ++iSubset)
			delete[] itModel->pSubsets[iSubset].pIndices;

		delete[] itModel->pSubsets;
		delete[] itModel->pVertices;
	}

	return pEntity;
}

SP_NMSPACE_END


#include <Abstract\IGeometry.h>
#include <Implementation\Geometry\Scene.h>
#include <Implementation\Geometry\Terrain.h>
#include <Implementation\Geometry\StaticObject.h>
#include <FileSPM.h>
#include <Abstract\IResourcePool.h>
#include <Abstract\IMaterial.h>

SP_NMSPACE_BEG

// -------------------------------------------------------------------------------------------------
S_API Scene::Scene()	
	: m_pTerrain(0),
	m_pEngine(0),
	m_pSkyBox(0)
{
	m_pSceneNodes = new std::vector<SSceneNode>();
}

// -------------------------------------------------------------------------------------------------
S_API void Scene::Clear()
{
	if (IS_VALID_PTR(m_pTerrain))
	{
		m_pTerrain->Clear();
		delete m_pTerrain;
	}

	m_pTerrain = nullptr;

	if (!IS_VALID_PTR(m_pSceneNodes))
		delete m_pSceneNodes;

	m_pSceneNodes = 0;
}

// -------------------------------------------------------------------------------------------------
S_API SResult Scene::Initialize(IGameEngine* pGameEngine)
{
	SP_ASSERTR(IS_VALID_PTR(pGameEngine), S_INVALIDPARAM);
	m_pEngine = pGameEngine;
	return S_SUCCESS;
}

// -------------------------------------------------------------------------------------------------
S_API std::vector<SSceneNode>* Scene::GetSceneNodes()
{
	return m_pSceneNodes;
}

// -------------------------------------------------------------------------------------------------
void Scene::CheckSceneNodesArray()
{
	if (!IS_VALID_PTR(m_pSceneNodes))
	{
		m_pSceneNodes = new std::vector<SSceneNode>();		
	}
}


// -------------------------------------------------------------------------------------------------
S_API SResult Scene::AddSceneNode(const SSceneNode& node)
{
	if (!IS_VALID_PTR(node.pObject) && !IS_VALID_PTR(node.pLight) && !IS_VALID_PTR(node.pStatic))
	{
		return CLog::Log(S_ERROR, "Cannot AddSceneNode: All pointers of node invalid!");
	}

	CheckSceneNodesArray();

	m_pSceneNodes->push_back(node);

	return S_SUCCESS;
}

// -------------------------------------------------------------------------------------------------
S_API SResult Scene::AddStaticObject(IStaticObject* pStatic)
{
	CheckSceneNodesArray();

	if (!IS_VALID_PTR(pStatic))
		return S_INVALIDPARAM;

	SSceneNode node;
	node.aabb = pStatic->GetBoundBox();
	node.type = eSCENENODE_STATIC;
	node.pStatic = pStatic;

	m_pSceneNodes->push_back(node);
	return S_SUCCESS;
}

// -------------------------------------------------------------------------------------------------
S_API SResult Scene::AddObject(IObject* pObject)
{
	CheckSceneNodesArray();

	if (!IS_VALID_PTR(pObject))
		return S_INVALIDPARAM;

	pObject->RecalcBoundBox();

	SSceneNode node;
	node.aabb = pObject->GetBoundBox();
	node.type = eSCENENODE_ENTITY;
	node.pObject = pObject;

	m_pSceneNodes->push_back(node);

	return S_SUCCESS;
}

// -------------------------------------------------------------------------------------------------
S_API ITerrain* Scene::CreateTerrain(unsigned int nSegs, unsigned int nChunkSegs, float fSideSz, float baseHeight, float fChunkStepDist, unsigned int nLodLevels, bool center /*=true*/)
{
	SP_ASSERTR(IS_VALID_PTR(m_pEngine), nullptr);
	if (IS_VALID_PTR(m_pTerrain))
		m_pTerrain->Clear();

	m_pTerrain = new Terrain();	

	if (Failure(m_pTerrain->Init(m_pEngine, nSegs, nChunkSegs, fSideSz, baseHeight, fChunkStepDist, nLodLevels, center)))
	{
		delete m_pTerrain;
		m_pTerrain = 0;

		EngLog(S_ERROR, m_pEngine, "Failed initialize terrain in Scene::CreateTerrain");
	}

	return m_pTerrain;
}

// -------------------------------------------------------------------------------------------------
S_API IStaticObject* Scene::LoadStaticObjectFromFile(const char* filename, const char* objName)
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

			subset.pMaterial = m_pEngine->GetMaterialManager()->FindMaterial(modelSubset.materialName.c_str());

			iSubset++;
		}

		vtxOffset += itModel->nVertices;
	}

	StaticObject* pStaticObject = new StaticObject();

	SResult res;
	if (Failure(res = pStaticObject->Init(m_pEngine, objName, &geomDesc)))
		CLog::Log(S_ERROR, "Failed init static object while loading file '%s'", filename);

	
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

	return pStaticObject;
}

// -------------------------------------------------------------------------------------------------
S_API SResult Scene::CreateNormalsGeometry(IRenderableComponent* renderable, SInitialGeometryDesc* pNormalsGeometry) const
{
	if (!IS_VALID_PTR(renderable) || !IS_VALID_PTR(pNormalsGeometry))
		return S_INVALIDPARAM;

	if (!IS_VALID_PTR(m_pEngine))
		return S_NOTINIT;	
	
	if (Failure(renderable->GetGeometry()->CalculateNormalsGeometry(*pNormalsGeometry, 3.0f)))
	{
		return EngLog(S_ERROR, m_pEngine, "Failed Calculcate Normals Geometry!");
	}

	return S_SUCCESS;
}

// -------------------------------------------------------------------------------------------------
void Scene::SetSkyBox(ISkyBox* pSkyBox)
{
	m_pSkyBox = pSkyBox;
}

// -------------------------------------------------------------------------------------------------
ISkyBox* Scene::GetSkyBox() const
{
	return m_pSkyBox;
}

SP_NMSPACE_END
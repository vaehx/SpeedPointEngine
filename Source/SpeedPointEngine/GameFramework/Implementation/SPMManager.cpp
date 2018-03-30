#include "SPMManager.h"
#include "../IGameEngine.h"
#include <3DEngine\I3DEngine.h>
#include <3DEngine\IMaterial.h>
#include <Common\FileUtils.h>

SP_NMSPACE_BEG

using namespace std;

S_API SPMManager::SPMManager(unsigned int numMaxCached)
{
	m_NumMaxCached = numMaxCached;
	if (m_NumMaxCached < 1)
		m_NumMaxCached = 1;
}

S_API SPMManager::~SPMManager()
{
	Clear();
}

S_API const CSPMLoader* SPMManager::Load(const string& absResPath)
{
	// Look in cache first
	auto existing = m_Cache.find(absResPath);
	if (existing != m_Cache.end())
		return existing->second;

	// Not found in cache, load from disk
	IGameEngine* pEngine = SpeedPointEnv::GetEngine();
	string absSysPath = pEngine->GetResourceSystemPath(absResPath).c_str();

	CSPMLoader* pLoader = new CSPMLoader();
	if (!pLoader->Load(absSysPath.c_str()))
	{
		CLog::Log(S_ERROR, "SPMManager::Load(): Failed load SPM '%s'", absSysPath.c_str());
		return 0;
	}

	// Remove oldest cache entry based on FIFO
	if (m_Cache.size() == m_NumMaxCached)
	{
		delete m_Cache.at(m_EvictionQueue.front());
		m_Cache.erase(m_EvictionQueue.front());
		m_EvictionQueue.pop();
	}

	// Add new cache entry
	m_Cache.emplace(absResPath, pLoader);
	m_EvictionQueue.push(absResPath);

	CLog::Log(S_DEBUG, "(SPMManager) Loaded '%s'", absResPath.c_str());

	return pLoader;
}

S_API void SPMManager::FillInitialGeometryDescFromSPM(const CSPMLoader* pSPM, SInitialGeometryDesc& geomDesc)
{
	if (!pSPM)
		return;

	I3DEngine* p3DEngine = SpeedPointEnv::Get3DEngine();

	string spmSysPath = pSPM->GetLoadedFileName();
	string spmResPath = MakePathRelativeTo(spmSysPath, SpeedPointEnv::GetEngine()->GetResourceSystemPath());
	string modelDir = GetDirectoryPath(spmResPath);

	const std::vector<SModelMeta>& models = pSPM->GetModels();

	geomDesc.geomFile = spmResPath;
	geomDesc.bRequireNormalRecalc = false;
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

	// Flatten-out the subsets in all models into a single array of subsets	
	IMaterialManager* pMatMgr = p3DEngine->GetMaterialManager();
	u32 vtxOffset = 0;
	unsigned int iSubset = 0;
	for (auto itModel = models.begin(); itModel != models.end(); ++itModel)
	{
		// Copy over vertices
		for (u32 iVtx = 0; iVtx < itModel->nVertices; ++iVtx)
		{
			geomDesc.pVertices[vtxOffset + iVtx] = itModel->pVertices[iVtx];
		}

		// itModel->materialFile may already be absolute
		string absMaterialFile = MakePathAbsolute(itModel->materialFile, modelDir);

		IMaterial* pMaterial = pMatMgr->LoadMaterial(absMaterialFile);
		for (u16 iModelSubset = 0; iModelSubset < itModel->nLoadedSubsets; ++iModelSubset)
		{
			const SSubset& modelSubset = itModel->pSubsets[iModelSubset];
			SInitialSubsetGeometryDesc& subset = geomDesc.pSubsets[iSubset];
			subset.nIndices = modelSubset.nIndices;
			subset.pIndices = new SIndex[subset.nIndices];

			// TODO: Use SIndex in SPMFile, so we don't have to copy the indices here!
			for (u32 iIndex = 0; iIndex < subset.nIndices; ++iIndex)
				subset.pIndices[iIndex] = modelSubset.pIndices[iIndex] + vtxOffset;

			if (pMaterial)
			{
				subset.pMaterial = pMaterial;
				subset.iMaterialDefinition = pMaterial->GetDefinitionIndex(modelSubset.materialDefinition);
			}

			iSubset++;
		}

		vtxOffset += itModel->nVertices;
	}

	for (auto itModel = models.begin(); itModel != models.end(); ++itModel)
	{
		for (u16 iSubset = 0; iSubset < itModel->nSubsets; ++iSubset)
			delete[] itModel->pSubsets[iSubset].pIndices;

		delete[] itModel->pSubsets;
		delete[] itModel->pVertices;
	}
}

S_API void SPMManager::ClearInitialGeometryDesc(SInitialGeometryDesc& geomDesc)
{
	delete[] geomDesc.pVertices;
	geomDesc.pVertices = 0;

	if (geomDesc.pSubsets)
	{
		for (u16 iSubset = 0; iSubset < geomDesc.nSubsets; ++iSubset)
			delete[] geomDesc.pSubsets[iSubset].pIndices;

		delete[] geomDesc.pSubsets;
		geomDesc.pSubsets = 0;
	}
}

S_API IGeometry* SPMManager::CreateGeometryFromSPM(const CSPMLoader* pSPM, const string& name /*= ""*/)
{
	if (!pSPM)
	{
		CLog::Log(S_ERROR, "SPMManager::CreateGeometryFromSPM() failed: pSPM is invalid");
		return 0;
	}

	string usedName = name;
	if (usedName.empty())
		usedName = SystemPathToResourcePath(pSPM->GetLoadedFileName(), SpeedPointEnv::GetEngine()->GetResourceSystemPath());

	SInitialGeometryDesc geomDesc;
	FillInitialGeometryDescFromSPM(pSPM, geomDesc);

	IGeometryManager* pGeomMgr = SpeedPointEnv::Get3DEngine()->GetGeometryManager();
	IGeometry* pGeom = pGeomMgr->CreateGeometry(geomDesc, usedName);

	ClearInitialGeometryDesc(geomDesc);

	if (!pGeom)
	{
		CLog::Log(S_ERROR, "SPMManager::CreateGeometryFromSPM() failed: Could not create geometry");
		return 0;
	}

	return pGeom;
}

S_API geo::shape* SPMManager::ConvertSPMColShapeToGeoShape(const SSPMColShape* spmShape)
{
	if (!spmShape)
		return 0;

	geo::shape* pshape = 0;
	switch (spmShape->type)
	{
	case SPM_COLSHAPE_MESH:
		{
			const SSPMColShapeMesh* colmesh = dynamic_cast<const SSPMColShapeMesh*>(spmShape);
			geo::mesh* pmesh = new geo::mesh();
			pmesh->transform = Mat44::Identity;
			
			pmesh->num_points = colmesh->nVertices;
			pmesh->points = new Vec3f[pmesh->num_points];
			memcpy(pmesh->points, colmesh->pVertices, sizeof(Vec3f) * pmesh->num_points);
			
			pmesh->num_indices = colmesh->nIndices;
			pmesh->indices = new unsigned int[pmesh->num_indices];
			memcpy(pmesh->indices, colmesh->pIndices, sizeof(unsigned int) * pmesh->num_indices);

			pshape = pmesh;
			break;
		}
	case SPM_COLSHAPE_SPHERE:
		{
			const SSPMColShapeSphere* colsphere = dynamic_cast<const SSPMColShapeSphere*>(spmShape);
			pshape = new geo::sphere(colsphere->center, colsphere->radius);
			break;
		}
	case SPM_COLSHAPE_CAPSULE:
		{
			const SSPMColShapeCapsule* colcaps = dynamic_cast<const SSPMColShapeCapsule*>(spmShape);
			pshape = new geo::capsule(colcaps->center, colcaps->axis, colcaps->halfLength, colcaps->radius);
			break;
		}
	case SPM_COLSHAPE_CYLINDER:
		{
			const SSPMColShapeCylinder* colcyl = dynamic_cast<const SSPMColShapeCylinder*>(spmShape);
			pshape = new geo::cylinder(colcyl->center, colcyl->axis, colcyl->halfLength, colcyl->radius);
			break;
		}
	case SPM_COLSHAPE_BOX:
		{
			const SSPMColShapeBox* colbox = dynamic_cast<const SSPMColShapeBox*>(spmShape);
			geo::box* pbox = new geo::box();
			pbox->c = colbox->center;
			for (int i = 0; i < 3; ++i)
			{
				pbox->axis[i] = colbox->axis[i];
				pbox->dim[i] = colbox->dimensions[i];
			}
			break;
		}
	default:
		CLog::Log(S_WARN, "SPMManager: Unknown SPM colshape type %d", spmShape->type);
		return 0;
	}

	return pshape;
}

S_API void SPMManager::Clear()
{
	// Clear in order of the evicition queue first
	while (m_EvictionQueue.size() > 0)
	{
		delete m_Cache.at(m_EvictionQueue.front());
		m_Cache.erase(m_EvictionQueue.front());
		m_EvictionQueue.pop();
	}

	// Remove remaining elements (just in case the eviction queue missed elements)
	for (auto& entry : m_Cache)
	{
		delete entry.second;
		entry.second = 0;
	}

	m_Cache.clear();
}

SP_NMSPACE_END

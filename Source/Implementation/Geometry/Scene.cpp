
#include <Abstract\IGeometry.h>
#include <Implementation\Geometry\Scene.h>
#include <Implementation\Geometry\Terrain.h>
#include <Implementation\Geometry\File3DS.h>
#include <Implementation\Geometry\StaticObject.h>
#include <Abstract\IResourcePool.h>
#include <Abstract\IMaterial.h>

SP_NMSPACE_BEG

// -------------------------------------------------------------------------------------------------
S_API Scene::Scene()	
	: m_pTerrain(0)
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
	if (!IS_VALID_PTR(node.pObject))
	{
		return CLog::Log(S_ERROR, "Cannot AddSceneNode: Object ptr of node is invalid!");		
	}

	CheckSceneNodesArray();

	m_pSceneNodes->push_back(node);
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
S_API ITerrain* Scene::CreateTerrain(unsigned int nSegs, unsigned int nChunkSegs, float fSideSz, float baseHeight, float fChunkStepDist, unsigned int nLodLevels)
{
	SP_ASSERTR(IS_VALID_PTR(m_pEngine), nullptr);
	if (IS_VALID_PTR(m_pTerrain))
		m_pTerrain->Clear();

	m_pTerrain = new Terrain();	

	if (Failure(m_pTerrain->Init(m_pEngine, nSegs, nChunkSegs, fSideSz, baseHeight, fChunkStepDist, nLodLevels)))
	{
		EngLog(S_ERROR, m_pEngine, "Failed initialize terrain in Scene::CreateTerrain");
	}

	return m_pTerrain;
}

// -------------------------------------------------------------------------------------------------
S_API IStaticObject* Scene::LoadStaticObjectFromFile(const char* filename)
{
	SP_ASSERTR(IS_VALID_PTR(m_pEngine), nullptr, "Engine not initialized!");

	// assume it's a 3ds file	

	std::ifstream file;
	file.open(filename, std::ifstream::in | std::ifstream::binary);
	if (!file.is_open())
	{
		EngLog(S_ERROR, m_pEngine, "Could not load static object from file: Could not open file (not found, no permissions, ...)");
		return nullptr;
	}
		
	File3DS file3ds(file);
	file3ds.Read();	
	file.close();

	//
	// Disccussion: How to store multiple Named Object from the 3Ds file into single geometry?
	//


	// One possible solution: merge all points and faces into a single vertex / index buffer

	/*
	SInitialMaterials mats;
	mats.nMaterials = file3ds.materials.size();
	mats.pMaterials = new SMaterial[mats.nMaterials];
	*/	

	SInitialGeometryDesc geom;
	geom.nMatIndexAssigns = file3ds.materials.size();
	geom.pMatIndexAssigns = new SMaterialIndices[geom.nMatIndexAssigns];

	// add all material names to mat index assigns so that we have their names already
	unsigned int iMatIdx = 0;
	for (auto itMat = file3ds.materials.begin(); itMat != file3ds.materials.end(); itMat++)
	{
		unsigned int matNameLn = sp_strlen(itMat->material_name);
		geom.pMatIndexAssigns[iMatIdx].materialName = new char[matNameLn + 1];
		memcpy(geom.pMatIndexAssigns[iMatIdx].materialName, itMat->material_name, matNameLn);
		geom.pMatIndexAssigns[iMatIdx].materialName[matNameLn] = 0;
				

		// fill initial materials
		IMaterial* mat = m_pEngine->GetMaterialManager()->CreateMaterial(itMat->material_name);
		SShaderResources& matRes = mat->GetLayer(0)->resources;

		if (IS_VALID_PTR(itMat->texturemap.name))
			matRes.textureMap = m_pEngine->GetResources()->GetTexture(itMat->texturemap.name);

		if (IS_VALID_PTR(itMat->bumpmap.name))
			matRes.normalMap = m_pEngine->GetResources()->GetTexture(itMat->bumpmap.name);


		iMatIdx++;
	}

	// count vertices, indices and material assigns to be able to prepare buffers
	geom.nVertices = 0;
	geom.nIndices = 0;
	for (auto itObj = file3ds.objects.begin(); itObj != file3ds.objects.end(); itObj++)
	{
		geom.nVertices += itObj->nVertices;
		geom.nIndices += itObj->nFaces * 3; // three indices per face		

		for (auto itMeshMat = itObj->meshMaterials.begin(); itMeshMat != itObj->meshMaterials.end(); itMeshMat++)
		{
			bool bMatIndexAssignFound = false;
			for (unsigned int iMatIndexAssign = 0; iMatIndexAssign < geom.nMatIndexAssigns; ++iMatIndexAssign)
			{
				SMaterialIndices& matIndexAssign = geom.pMatIndexAssigns[iMatIndexAssign];
				if (strcmp(itMeshMat->name, matIndexAssign.materialName) != 0)
					continue;

				matIndexAssign.nIdxIndices += itMeshMat->nAffectedFaces * 3; // 3 indices per face
				bMatIndexAssignFound = true;
				break;
			}

			if (!bMatIndexAssignFound)
				EngLog(S_WARN, m_pEngine, "Could not determine material to which an object should be bound.");
		}		
	}	

	// remove all matIndexAssigns that have material, but no indices assigned
	unsigned int* pValidMatIndexAssigns = new unsigned int[geom.nMatIndexAssigns];
	unsigned int nValidMatIndexAssigns = 0;
	for (unsigned int iMatIndexAssign = 0; iMatIndexAssign < geom.nMatIndexAssigns; ++iMatIndexAssign)
	{
		if (geom.pMatIndexAssigns[iMatIndexAssign].nIdxIndices == 0)
			continue;

		pValidMatIndexAssigns[nValidMatIndexAssigns] = iMatIndexAssign;
		nValidMatIndexAssigns++;
	}

	if (nValidMatIndexAssigns < geom.nMatIndexAssigns)
	{
		SMaterialIndices* pNewMatIndexAssigns = new SMaterialIndices[nValidMatIndexAssigns];
		for (unsigned int iValidMatIndexAssign = 0; iValidMatIndexAssign < nValidMatIndexAssigns; ++iValidMatIndexAssign)
		{
			// use copy assignment operator
			pNewMatIndexAssigns[iValidMatIndexAssign]
				= geom.pMatIndexAssigns[pValidMatIndexAssigns[iValidMatIndexAssign]];
		}

		delete[] geom.pMatIndexAssigns;
		geom.pMatIndexAssigns = pNewMatIndexAssigns;
		geom.nMatIndexAssigns = nValidMatIndexAssigns;
	}

	delete[] pValidMatIndexAssigns;



	// create merged buffers
	geom.pVertices = new SVertex[geom.nVertices];
	geom.pIndices = new SIndex[geom.nIndices];	

	unsigned int vtxOffset = 0, idxOffset = 0;
	for (auto itObj = file3ds.objects.begin(); itObj != file3ds.objects.end(); itObj++)
	{
		// add vertices
		for (unsigned int iVtx = 0; iVtx < itObj->nVertices; ++iVtx)
			geom.pVertices[iVtx + vtxOffset] = SVertex(itObj->pVertices[iVtx].x, itObj->pVertices[iVtx].y, itObj->pVertices[iVtx].z);

		// add indices
		for (unsigned short iIdx = 0; iIdx < itObj->nFaces * 3; iIdx += 3)
		{
			unsigned short iFace = (unsigned short)(iIdx / 3);
			geom.pIndices[idxOffset + iIdx] = itObj->pFaces[iFace].v1 + vtxOffset;
			geom.pIndices[idxOffset + iIdx + 1] = itObj->pFaces[iFace].v2 + vtxOffset;
			geom.pIndices[idxOffset + iIdx + 2] = itObj->pFaces[iFace].v3 + vtxOffset;
		}		

		// add all mesh mat face indices to the matIndexAssigns, create them if not yet done
		for (auto itMeshMat = itObj->meshMaterials.begin(); itMeshMat != itObj->meshMaterials.end(); itMeshMat++)
		{
			// find according matIndexAssign and fill
			// Note: Error message if matIndexAssign not found already thrown above when counting required index buffer size
			for (unsigned int iMatIndexAssign = 0; iMatIndexAssign < geom.nMatIndexAssigns; ++iMatIndexAssign)
			{
				SMaterialIndices& matIndexAssign = geom.pMatIndexAssigns[iMatIndexAssign];
				if (strcmp(itMeshMat->name, matIndexAssign.materialName) != 0)
					continue;

				if (!IS_VALID_PTR(matIndexAssign.pIdxIndices))
					matIndexAssign.pIdxIndices = new unsigned long[matIndexAssign.nIdxIndices];

				for (unsigned short iMeshMatFace = 0; iMeshMatFace < itMeshMat->nAffectedFaces; ++iMeshMatFace)
				{
					// find according indices
					unsigned short iBegIndex = itMeshMat->pAffectedFaces[iMeshMatFace] * 3 + idxOffset;

					matIndexAssign.pIdxIndices[matIndexAssign.nFilledIdxIndices] = (unsigned long)iBegIndex;
					matIndexAssign.pIdxIndices[matIndexAssign.nFilledIdxIndices + 1] = (unsigned long)iBegIndex + 1;
					matIndexAssign.pIdxIndices[matIndexAssign.nFilledIdxIndices + 2] = (unsigned long)iBegIndex + 2;
					matIndexAssign.nFilledIdxIndices += 3;				
				}

				break; // found
			}
		}

		vtxOffset += itObj->nVertices;
		idxOffset += itObj->nFaces * 3;
	}


	geom.bRequireNormalRecalc = true;

	// Initialize result object
	StaticObject* pStaticObject = new StaticObject();
	IStaticObject* pReturn = pStaticObject;	
	if (Failure(pStaticObject->Init(m_pEngine, &geom)))
	{
		EngLog(S_ERROR, m_pEngine, "Failed Init Static Object to store loaded 3ds file!");
		pReturn = 0;
	}

	SP_SAFE_DELETE_ARR(geom.pVertices, 1);
	SP_SAFE_DELETE_ARR(geom.pIndices, 1);
	SP_SAFE_DELETE_ARR(geom.pMatIndexAssigns, 1);	


	return pReturn;
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
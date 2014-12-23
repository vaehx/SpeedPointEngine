
#include <Implementation\Geometry\Scene.h>
#include <Implementation\Geometry\Terrain.h>
#include <Implementation\Geometry\File3DS.h>
#include <Implementation\Geometry\StaticObject.h>
#include <Abstract\IResourcePool.h>

SP_NMSPACE_BEG

// -------------------------------------------------------------------------------------------------
S_API void Scene::Clear()
{
	if (IS_VALID_PTR(m_pTerrain))
	{
		m_pTerrain->Clear();
		delete m_pTerrain;
	}

	m_pTerrain = nullptr;
}

// -------------------------------------------------------------------------------------------------
S_API SResult Scene::Initialize(IGameEngine* pGameEngine)
{
	SP_ASSERTR(IS_VALID_PTR(pGameEngine), S_INVALIDPARAM);
	m_pEngine = pGameEngine;
	return S_SUCCESS;
}

// -------------------------------------------------------------------------------------------------
S_API ITerrain* Scene::CreateTerrain(float width, float depth, unsigned int nX, unsigned int nZ,
	float baseHeight, ITexture* pColorMap, ITexture* pDetailMap)
{
	SP_ASSERTR(IS_VALID_PTR(m_pEngine), nullptr);
	if (IS_VALID_PTR(m_pTerrain))
		m_pTerrain->Clear();

	m_pTerrain = new Terrain();
	if (Failure(m_pTerrain->Initialize(m_pEngine, nX, nZ)))
		EngLog(S_ERROR, m_pEngine, "Failed initialize terrain in Scene::CreateTerrain");

	if (Failure(m_pTerrain->CreatePlanar(width, depth, baseHeight)))
		EngLog(S_ERROR, m_pEngine, "Failed create planar terrain in Scene::CreateTerrain");

	m_pTerrain->SetColorMap(pColorMap);
	m_pTerrain->SetDetailMap(pDetailMap);

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

	SInitialMaterials mats;
	mats.nMaterials = file3ds.materials.size();
	mats.pMaterials = new SMaterial[mats.nMaterials];

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
		SMaterial& mat = mats.pMaterials[iMatIdx];
		mat.name = new char[matNameLn + 1];
		memcpy(mat.name, itMat->material_name, matNameLn);
		mat.name[matNameLn] = 0;

		if (IS_VALID_PTR(itMat->texturemap.name)) mat.textureMap = m_pEngine->GetResources()->GetTexture(itMat->texturemap.name);
		if (IS_VALID_PTR(itMat->bumpmap.name)) mat.normalMap = m_pEngine->GetResources()->GetTexture(itMat->bumpmap.name);


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
	IStaticObject* pStaticObject = new StaticObject();
	if (Failure(pStaticObject->Init(m_pEngine, m_pEngine->GetRenderer(), &mats, &geom)))
	{
		EngLog(S_ERROR, m_pEngine, "Failed Init Static Object to store loaded 3ds file!");
		return nullptr;
	}

	return pStaticObject;
}

SP_NMSPACE_END
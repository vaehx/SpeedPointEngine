// SpeedPoint Basic Terrain

#include <Implementation\Geometry\Terrain.h>
#include <Abstract\IGameEngine.h>
#include <Abstract\IRenderer.h>
#include <Abstract\IVertexBuffer.h>
#include <Abstract\IIndexBuffer.h>


SP_NMSPACE_BEG

// -------------------------------------------------------------------------------------------------------------------
S_API Terrain::~Terrain()
{
	Clear();
}

// -------------------------------------------------------------------------------------------------------------------
S_API SResult Terrain::Initialize(IGameEngine* pEngine, unsigned int nX, unsigned int nZ)
{
	m_nX = nX; m_nZ = nZ;	
	SP_ASSERTR(m_pEngine = pEngine, S_INVALIDPARAM);
	
	return S_SUCCESS;
}

// -------------------------------------------------------------------------------------------------------------------
S_API SResult Terrain::CreatePlanar(float fW, float fD, float baseHeight)
{
	if (!IS_VALID_PTR(m_pEngine) || m_nX == 0 || m_nZ == 0 || fW < 1.0f || fD < 1.0f)
		return S_INVALIDPARAM;

	SInitialGeometryDesc initGeom;
	initGeom.nVertices = (m_nX + 1) * (m_nZ + 1);
	initGeom.nIndices = m_nX * m_nZ * 6;
	initGeom.pVertices = new SVertex[initGeom.nVertices];
	initGeom.pIndices = new SIndex[initGeom.nIndices];

	float diffX = fW / (float)m_nX,
		diffZ = fD / (float)m_nZ;

	float ratioU = 1.0f / (float)(m_nX),
		ratioV = 1.0f / (float)(m_nZ);

	float posOffsetX = fW * 0.5f, posOffsetZ = fD * 0.5f;

	for (unsigned int z = 0; z <= m_nZ; ++z)
	{
		for (unsigned int x = 0; x <= m_nX; ++x)
		{
			unsigned int iVtxCur = z * (m_nX + 1) + x;
			float h = baseHeight + (sinf(2.0f * SP_PI * ratioU * x) + sinf(2.0f * SP_PI * ratioV * z)) * 0.7f;
			initGeom.pVertices[iVtxCur] = SVertex(
				x * diffX - posOffsetX, h, z * diffZ - posOffsetZ,		// position
				0.0f, 1.0f, 0.0f,				// normal
				-1.0f, 0.0f, 0.0f,				// tangent
				x * ratioU, z * ratioV);			// texcoord [0;1]
			
			if (x < m_nX && z < m_nZ)
			{
				unsigned int idxCur = (z * m_nX + x) * 6;
				initGeom.pIndices[idxCur] = iVtxCur;
				initGeom.pIndices[idxCur + 1] = iVtxCur + 1;
				initGeom.pIndices[idxCur + 2] = iVtxCur + 1 + (m_nX + 1);
				initGeom.pIndices[idxCur + 3] = iVtxCur + 1 + (m_nX + 1);
				initGeom.pIndices[idxCur + 4] = iVtxCur + (m_nX + 1);
				initGeom.pIndices[idxCur + 5] = iVtxCur;
			}
		}
	}	

	// Calculate normals
	for (unsigned int iVtx = 0; iVtx < initGeom.nVertices; ++iVtx)
	{
		SVertex& vtx = initGeom.pVertices[iVtx];
		unsigned int iX = iVtx % (m_nX + 1),
			iZ = (unsigned int)((iVtx - iX) / (m_nX + 1));

		SVector3 dirAcc(0,0,0);
		unsigned int nAffectedVertices = 1;				
		
		// 1-2
		if (iX > 0 && iZ > 0)
		{
			const SVertex& vtx1 = initGeom.pVertices[iVtx - 1],
				&vtx2 = initGeom.pVertices[iVtx - (m_nX + 1)];

			dirAcc += SVector3Normalize(SVector3Cross(
				SVector3(vtx1.x - vtx.x, vtx1.y - vtx.y, vtx1.z - vtx.z),
				SVector3(vtx2.x - vtx.x, vtx2.y - vtx.y, vtx2.z - vtx.z)));
		}
		else
			dirAcc += SVector3(0, 1.0f, 0);

		// 2-3
		if (iX < m_nX && iZ > 0)
		{
			const SVertex& vtx2 = initGeom.pVertices[iVtx - (m_nX + 1)],
				&vtx3 = initGeom.pVertices[iVtx + 1];

			dirAcc += SVector3Normalize(SVector3Cross(
				SVector3(vtx2.x - vtx.x, vtx2.y - vtx.y, vtx2.z - vtx.z),
				SVector3(vtx3.x - vtx.x, vtx3.y - vtx.y, vtx3.z - vtx.z)));
		}
		else
			dirAcc += SVector3(0, 1.0f, 0);

		// 3 - 4
		if (iX < m_nX && iZ < m_nZ)
		{
			const SVertex& vtx3 = initGeom.pVertices[iVtx + 1],
				&vtx4 = initGeom.pVertices[iVtx + (m_nX + 1)];

			dirAcc += SVector3Normalize(SVector3Cross(
				SVector3(vtx3.x - vtx.x, vtx3.y - vtx.y, vtx3.z - vtx.z),
				SVector3(vtx4.x - vtx.x, vtx4.y - vtx.y, vtx4.z - vtx.z)));
		}
		else
			dirAcc += SVector3(0, 1.0f, 0);

		// 4 - 1
		if (iX > 0 && iZ < m_nZ)
		{
			const SVertex& vtx4 = initGeom.pVertices[iVtx + (m_nX + 1)],
				&vtx1 = initGeom.pVertices[iVtx - 1];

			dirAcc += SVector3Normalize(SVector3Cross(
				SVector3(vtx4.x - vtx.x, vtx4.y - vtx.y, vtx4.z - vtx.z),
				SVector3(vtx1.x - vtx.x, vtx1.y - vtx.y, vtx1.z - vtx.z)));
		}
		else
			dirAcc += SVector3(0, 1.0f, 0);

		SVector3 dirAccN = SVector3Normalize(dirAcc);

		if (dirAccN.y < 0)
			dirAccN = -dirAccN;

		vtx.nx = dirAccN.x;
		vtx.ny = dirAccN.y;
		vtx.nz = dirAccN.z;
	}

	return m_Geometry.Init(m_pEngine, m_pEngine->GetRenderer(), &initGeom);
}

// -------------------------------------------------------------------------------------------------------------------
S_API SResult Terrain::RenderTerrain(void)
{
	IRenderer* pRenderer = m_pEngine->GetRenderer();
	
	SRenderDesc dsc;
	dsc.drawCallDesc.pVertexBuffer = m_Geometry.GetVertexBuffer();
	dsc.drawCallDesc.pIndexBuffer = m_Geometry.GetIndexBuffer();
	dsc.drawCallDesc.iStartVBIndex = 0;
	dsc.drawCallDesc.iEndVBIndex = dsc.drawCallDesc.pVertexBuffer->GetVertexCount() - 1;
	dsc.drawCallDesc.iStartIBIndex = 0;
	dsc.drawCallDesc.iEndIBIndex = dsc.drawCallDesc.pIndexBuffer->GetIndexCount() - 1;

	dsc.pGeometry = &m_Geometry;
	dsc.technique = eRENDER_FORWARD;
	SMatrixIdentity(&dsc.drawCallDesc.transform.scale);

	return pRenderer->RenderGeometry(dsc);
}

// -------------------------------------------------------------------------------------------------------------------
S_API SResult Terrain::Clear(void)
{
	m_pEngine = nullptr;
	m_nX = 0; m_nZ = 0;
	m_Geometry.Clear();

	return S_SUCCESS;
}


SP_NMSPACE_END
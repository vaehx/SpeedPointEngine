//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2011-2014, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include <Implementation\DirectX11\DirectX11OutputPlane.h>
#include <Abstract\IVertexBuffer.h>
#include <Abstract\IIndexBuffer.h>
#include <Abstract\Matrix.h>
#include <Abstract\Vector3.h>
#include <SpeedPointEngine.h>
#include <Util\SVertex.h>
#include <Implementation\DirectX11\DirectX11VertexBuffer.h>
#include <Implementation\DirectX11\DirectX11IndexBuffer.h>

SP_NMSPACE_BEG

// --------------------------------------------------------------------------------
S_API usint32 DirectX11OutputPlane::GetIndexCount()
{
	return 600; // 10 * 10 * 6;
}

// --------------------------------------------------------------------------------
S_API SResult DirectX11OutputPlane::Initialize(SpeedPointEngine* eng, IRenderer* renderer, int nW, int nH)
{
	SP_ASSERTR(eng && renderer, S_INVALIDPARAM);	

	if (Failure(Clear()))
		return S_ERROR;

	m_pEngine = eng;
	m_pDXRenderer = (DirectX11Renderer*)renderer;

	// Initialize the matrices
	float fWidth = (float)nW * 0.1f;
	float fHeight = (float)nH * 0.1f;
	
	SMatrix mtxProj;
	SPMatrixOrthoRH(&mtxProj, fWidth, fHeight, 2.0f, 200.0f);	

	SMatrix mtxView;
	SPMatrixLookAtRH(&mtxView, SVector3(0, 0, -10.0f), SVector3(0, 0, 0.0f), SVector3(0, 1.0f, 0));	



	SVertex* pVertices = (SVertex*)(malloc(sizeof(SVertex)* 11 * 11));
	SIndex* pIndices = (SIndex*)(malloc(sizeof(SIndex) * GetIndexCount()));

	UINT vPos = 0, iPos = 0;

	float fXDiff = fWidth * 0.1f;
	float fXHalf = fXDiff * (fWidth * 0.5f);

	float fYDiff = fHeight * 0.1f;
	float fYHalf = fYDiff * (fHeight * 0.5f);

	for (int yy = 0; yy < 11; yy++)
	{
		for (int xx = 0; xx < 11; xx++)
		{
			pVertices[vPos] = SVertex(
				(float)(xx - 5) * fXDiff, (float)(yy - 5) * fYDiff, 2.0f,	// position
				0.0f, 0.0f, -1.0f,						// normal
				1.0f, 0.0f, 0.0f,						// tangent
				(float)xx * 0.1f, (float)yy * 0.1f);				// texture coords

			if (yy < 10 && xx < 10)
			{
				// Add a facette / 2 triangles
				pIndices[iPos] = vPos;
				pIndices[iPos + 1] = vPos + 11;
				pIndices[iPos + 2] = vPos + 1;
				pIndices[iPos + 3] = vPos + 11;
				pIndices[iPos + 4] = vPos + 12;
				pIndices[iPos + 5] = vPos + 1;
				iPos += 6;
			}

			++vPos;
		}
	}



	// Create the geometry with plane of 10 * 10 fields
	m_pVertexBuffer = new DirectX11VertexBuffer();
	if (Failure(m_pVertexBuffer->Initialize(m_pEngine, renderer, eVBUSAGE_STATIC, 11 * 11, pVertices)))
	{
		return m_pEngine->LogE("Failed initialize vertex Buffer of OutputPlane!");
	}

	m_pIndexBuffer = new DirectX11IndexBuffer();
	if (Failure(m_pIndexBuffer->Initialize(m_pEngine, renderer, eIBUSAGE_STATIC, 10 * 10 * 6, pIndices)))
	{
		return m_pEngine->LogE("Failed initialize index buffer of OutputPlane!");
	}

	free(pVertices);
	free(pIndices);

	return S_SUCCESS;
}

// --------------------------------------------------------------------------------
S_API bool DirectX11OutputPlane::IsInitialized()
{
	return m_pEngine && m_pDXRenderer
		&& m_pVertexBuffer
		&& m_pIndexBuffer
		&& m_pVertexBuffer->IsInited()
		&& m_pIndexBuffer->IsInited();
}

// --------------------------------------------------------------------------------
S_API SResult DirectX11OutputPlane::Render(IFBO* pGBufferAlbedo, IFBO* pLightingBuffer)
{
	return S_NOTIMPLEMENTED;
}

// --------------------------------------------------------------------------------
S_API SResult DirectX11OutputPlane::Clear(void)
{
	SResult res = S_SUCCESS;	

	if (m_pIndexBuffer && Failure(m_pIndexBuffer->Clear()))
	{
		if (m_pEngine)
			return m_pEngine->LogE("Failed Clear index Buffer of output plane!");
		else
			res = S_ERROR;
	}
	m_pIndexBuffer = 0;

	if (m_pVertexBuffer && Failure(m_pVertexBuffer->Clear()))
	{		
		if (m_pEngine)
			return m_pEngine->LogE("Failed Clear vertex buffer of output plane!");
		else
			res = S_ERROR;
	}	
	m_pVertexBuffer = 0;

	m_pEngine = 0;
	m_pDXRenderer = 0;

	// okay done.
	return res;
}



SP_NMSPACE_END
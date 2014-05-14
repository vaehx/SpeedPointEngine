//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2011-2014, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Implementation\DirectX11\DirectX11Effect.h>
#include <SSpeedPointEngine.h>
#include <Implementation\DirectX11\DirectX11Renderer.h>

SP_NMSPACE_BEG

// -------------------------------------------------------------------------
S_API SResult DirectX11Effect::Initialize(SpeedPointEngine* pEngine, char* cFilename)
{
	SP_ASSERTR((m_pEngine = pEngine) && cFilename, S_INVALIDPARAM);

	if (IsInitialized())
		Clear();

	IRenderer* pRenderer = pEngine->GetRenderer();	
	SP_ASSERTR(pRenderer->GetType() == S_DIRECTX11, S_INVALIDPARAM);

	m_pDXRenderer = (DirectX11Renderer*)pRenderer;

	// TODO: Load cso binary file here
	/* PSEUDOCODE:
	SpeedPoint::CSOReader* pReader; // instantiate and initialize it anywhere you want
	auto pVertexShaderBytecode = pReader->GetAllData(cFilename);
	void* pShaderData = pVertexShaderBytecode->GetData();
	usint32 nShaderLength = pVertexShaderBytecode->GetLength();
	*/
	void* pShaderData = 0;
	usint32 nShaderLength = 0;

	HRESULT hRes;
	hRes = m_pDXRenderer->GetD3D11Device()->CreateVertexShader(pShaderData, nShaderLength, 0, &m_pVertexShader);
	if (Failure(hRes))
	{
		m_pEngine->LogE("Failed create vertex shader from file!");
	}

	pShaderData = 0; // todo: set this to the pixel shader bytecode data here
	nShaderLength = 0;
	hRes = m_pDXRenderer->GetD3D11Device()->CreatePixelShader(pShaderData, nShaderLength, 0, &m_pPixelShader);
	if (Failure(hRes))
	{
		m_pEngine->LogE("Failed create pixel shader from file!");
	}

	return Failure(hRes) ? S_ERROR : S_SUCCESS;
}

// -------------------------------------------------------------------------
S_API bool DirectX11Effect::IsInitialized()
{
	return (m_pEngine && (m_pPixelShader || m_pVertexShader) && m_pDXRenderer);
}

// -------------------------------------------------------------------------
S_API SResult DirectX11Effect::Clear(void)
{
	SP_SAFE_RELEASE(m_pPixelShader);
	SP_SAFE_RELEASE(m_pVertexShader);

	m_pDXRenderer = 0;
	m_pEngine = 0;

	return S_SUCCESS;
}

// -------------------------------------------------------------------------
S_API SResult DirectX11Effect::SetTechnique(char* cTechnique)
{


	// TODO: is this even possible in DX11 anylonger?



	return S_NOTIMPLEMENTED;
}




SP_NMSPACE_END
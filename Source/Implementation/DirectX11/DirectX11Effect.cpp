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
#include <d3dcompiler.h>

#include <fstream>
using std::ifstream;

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



	//
	//
	// For now we only compile the shader at runtime!!
	//
	//


	// 1. Load shader file

	char* fxBuffer;
	unsigned int size;
	ifstream fxInput(cFilename, ifstream::binary);

	if (!fxInput.good())
		return m_pEngine->LogE("Failed open FX file ifstream!");

	fxInput.seekg(0, fxInput.end);
	size = fxInput.tellg();

	fxInput.seekg(0, fxInput.beg);

	fxBuffer = new char[size];
	fxInput.read(fxBuffer, size);

	fxInput.close();


	// 2. Compile the buffer

	usint32 compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	compileFlags |= D3DCOMPILE_DEBUG;
#endif	

	ID3DBlob *pVSBlob, *pPSBlob, *pErrorBlob;
	if (Failure(D3DCompile(
		(void*)fxBuffer,
		size,
		0, 0, 0, "VSMain",
		"vs_4_0",
		compileFlags,
		0,
		&pVSBlob, &pErrorBlob)))
	{
		MessageBox(0, (CHAR*)pErrorBlob->GetBufferPointer(), "Effect compile error (VS)!", MB_OK | MB_ICONERROR);
		return S_ERROR;
	}

	if (Failure(D3DCompile(
		(void*)fxBuffer,
		size,
		0, 0, 0, "PSMain",
		"ps_4_0",
		compileFlags,
		0,
		&pPSBlob, &pErrorBlob)))
	{
		MessageBox(0, (CHAR*)pErrorBlob->GetBufferPointer(), "Effect compile error (PS)!", MB_OK | MB_ICONERROR);
		return S_ERROR;
	}


	
	// 3. delete the ifstream buffer

	delete fxBuffer;



	// 4. Use the compiled bytecode to create the shaders

	usint32 nShaderLength = 0;

	HRESULT hRes;
	hRes = m_pDXRenderer->GetD3D11Device()->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), 0, &m_pVertexShader);
	if (Failure(hRes))
	{
		m_pEngine->LogE("Failed create vertex shader!");
	}
	
	hRes = m_pDXRenderer->GetD3D11Device()->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), 0, &m_pPixelShader);
	if (Failure(hRes))
	{
		m_pEngine->LogE("Failed create pixel shader!");
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


	// TODO: is this even possible in DX11 anylonger? --> no!



	return S_NOTIMPLEMENTED;
}




SP_NMSPACE_END
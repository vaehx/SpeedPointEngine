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
#include <Abstract\IGameEngine.h>
#include <Implementation\DirectX11\DirectX11Renderer.h>
#include <d3dcompiler.h>

#include <fstream>
using std::ifstream;

SP_NMSPACE_BEG

// -------------------------------------------------------------------------
DirectX11Effect::DirectX11Effect()
: m_pDXRenderer(0),
m_pEngine(0),
m_pPixelShader(0),
m_pVertexShader(0),
m_pVSInputLayout(0)
{
}

// -------------------------------------------------------------------------
DirectX11Effect::~DirectX11Effect()
{
	Clear();
}

// -------------------------------------------------------------------------
S_API SResult DirectX11Effect::Initialize(IGameEngine* pEngine, const char* cFilename, const char* cEntry)
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
	// Maybe dynamically compose the shader someday!
	//
	//


	// 1. Load shader file

	char* fxBuffer;
	unsigned int size;
	ifstream fxInput(cFilename, ifstream::binary);

	if (!fxInput.good())
		return m_pEngine->LogE("Failed open FX file ifstream!");

	fxInput.seekg(0, fxInput.end);
	size = (unsigned int)fxInput.tellg();

	fxInput.seekg(0, fxInput.beg);

	fxBuffer = new char[size];
	fxInput.read(fxBuffer, size);

	fxInput.close();


	// 2. Compile the buffer

	usint32 compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	compileFlags |= D3DCOMPILE_DEBUG;
#endif	

	char* composedEntryName = new char[60];
	ZeroMemory(composedEntryName, 60);
	sprintf_s(composedEntryName, 60, "VS_%s", cEntry);

	ID3DBlob *pVSBlob = 0, *pPSBlob = 0, *pErrorBlob = 0;
	if (Failure(D3DCompile(
		(void*)fxBuffer,
		size,
		0, 0, 0, composedEntryName,
		"vs_4_0",
		compileFlags,
		0,
		&pVSBlob, &pErrorBlob)))
	{
		MessageBox(0, (CHAR*)pErrorBlob->GetBufferPointer(), "Effect compile error (VS)!", MB_OK | MB_ICONERROR);
		return S_ERROR;
	}

	composedEntryName[0] = 'P'; // VS... -> PS...
	if (Failure(D3DCompile(
		(void*)fxBuffer,
		size,
		0, 0, 0, composedEntryName,
		"ps_4_0",
		compileFlags,
		0,
		&pPSBlob, &pErrorBlob)))
	{
		MessageBox(0, (CHAR*)pErrorBlob->GetBufferPointer(), "Effect compile error (PS)!", MB_OK | MB_ICONERROR);
		return S_ERROR;
	}

	delete[] composedEntryName;


	
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




	// 5. Create the polygon layout for the SVertex structure
	
	D3D11_INPUT_ELEMENT_DESC vtxDesc[4];
	memset(vtxDesc, 0, sizeof(D3D11_INPUT_ELEMENT_DESC) * 4);

	vtxDesc[0].AlignedByteOffset = 0;
	vtxDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vtxDesc[0].InputSlot = 0;
	vtxDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vtxDesc[0].SemanticIndex = 0;
	vtxDesc[0].SemanticName = "POSITION";	

	vtxDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vtxDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vtxDesc[1].InputSlot = 0;
	vtxDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vtxDesc[1].SemanticIndex = 0;
	vtxDesc[1].SemanticName = "NORMAL";

	vtxDesc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vtxDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vtxDesc[2].InputSlot = 0;
	vtxDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vtxDesc[2].SemanticIndex = 0;
	vtxDesc[2].SemanticName = "TANGENT";

	vtxDesc[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vtxDesc[3].Format = DXGI_FORMAT_R32G32_FLOAT;
	vtxDesc[3].InputSlot = 0;
	vtxDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vtxDesc[3].SemanticIndex = 0;
	vtxDesc[3].SemanticName = "TEXCOORD";

	/*
	vtxDesc[4].AlignedByteOffset = 0;
	vtxDesc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vtxDesc[4].InputSlot = 0;
	vtxDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vtxDesc[4].SemanticIndex = 1;
	vtxDesc[4].SemanticName = "TEXCOORD";
	*/
	
	if (Failure(m_pDXRenderer->GetD3D11Device()->CreateInputLayout(vtxDesc, 4, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_pVSInputLayout)))
		return m_pEngine->LogE("Failed Create input layout for VS!");



	// 6. Clear unneeded stuff

	SP_SAFE_RELEASE(pVSBlob);
	SP_SAFE_RELEASE(pPSBlob);
	SP_SAFE_RELEASE(pErrorBlob);



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
	if (IS_VALID_PTR(m_pPixelShader))
		m_pPixelShader->Release();

	if (IS_VALID_PTR(m_pVertexShader))
		m_pVertexShader->Release();

	m_pPixelShader = nullptr;
	m_pVertexShader = nullptr;

	if (IS_VALID_PTR(m_pVSInputLayout))
		m_pVSInputLayout->Release();
	
	m_pVSInputLayout = nullptr;

	m_pDXRenderer = 0;
	m_pEngine = 0;

	return S_SUCCESS;
}

// -------------------------------------------------------------------------
S_API SResult DirectX11Effect::Enable()
{
	SP_ASSERTR(m_pDXRenderer && m_pPixelShader && m_pVertexShader && m_pVSInputLayout, S_NOTINIT);

	ID3D11DeviceContext* pDXDeviceContext = m_pDXRenderer->GetD3D11DeviceContext();	
	pDXDeviceContext->VSSetShader(m_pVertexShader, 0, 0);		
	pDXDeviceContext->PSSetShader(m_pPixelShader, 0, 0);	
	pDXDeviceContext->IASetInputLayout(m_pVSInputLayout);	

	return S_SUCCESS;
}

// -------------------------------------------------------------------------
S_API SResult DirectX11Effect::SetTechnique(char* cTechnique)
{
	return Enable();
}




SP_NMSPACE_END
//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2011-2014, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Implementation\DirectX11\DirectX11Shader.h>
#include <Abstract\IGameEngine.h>
#include <Implementation\DirectX11\DirectX11Renderer.h>
#include <d3dcompiler.h>
#include <Abstract\ISettings.h>

#include <fstream>
using std::ifstream;

SP_NMSPACE_BEG

// -------------------------------------------------------------------------
DirectX11Shader::DirectX11Shader()
: m_pDXRenderer(0),
m_pEngine(0),
m_pPixelShader(0),
m_pVertexShader(0),
m_pVSInputLayout(0)
{
}

// -------------------------------------------------------------------------
DirectX11Shader::~DirectX11Shader()
{
	Clear();
}

// -------------------------------------------------------------------------
S_API SResult DirectX11Shader::Load(IRenderer* pRenderer, const SShaderInfo& info)
{
	assert(IS_VALID_PTR(pRenderer));
	assert(pRenderer->GetType() == S_DIRECTX11);

	Clear();

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
	ifstream fxInput(info.filename, ifstream::binary);

	if (!fxInput.good())
	{
		return CLog::Log(S_ERROR, "Failed open FX file ifstream for %s", info.filename.c_str());
	}

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
	
	string composedEntryName = "VS_" + info.entry;	

	ID3DBlob *pVSBlob = 0, *pPSBlob = 0, *pErrorBlob = 0;
	if (Failure(D3DCompile(
		(void*)fxBuffer,
		size,
		0, 0, 0, composedEntryName.c_str(),
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
		0, 0, 0, composedEntryName.c_str(),
		"ps_4_0",
		compileFlags,
		0,
		&pPSBlob, &pErrorBlob)))
	{
		MessageBox(0, (CHAR*)pErrorBlob->GetBufferPointer(), "Effect compile error (PS)!", MB_OK | MB_ICONERROR);
		return S_ERROR;
	}

	CLog::Log(S_DEBUG, "Loaded and compiled effect '%s'", info.filename.c_str());


	
	// 3. delete the ifstream buffer

	delete[] fxBuffer;



	// 4. Use the compiled bytecode to create the shaders

	usint32 nShaderLength = 0;

	HRESULT hRes;
	hRes = m_pDXRenderer->GetD3D11Device()->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), 0, &m_pVertexShader);
	if (Failure(hRes))
	{
		CLog::Log(S_ERROR, "Failed create vertex shader!");
	}
	
	hRes = m_pDXRenderer->GetD3D11Device()->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), 0, &m_pPixelShader);
	if (Failure(hRes))
	{
		CLog::Log(S_ERROR, "Failed create pixel shader!");
	}	




	// 5. Create the polygon layout for the SVertex structure
	
	if (info.vertexType == eSHADERVERTEX_SIMPLE)
	{
		const unsigned int NUM_ELEMENTS = 2;

		D3D11_INPUT_ELEMENT_DESC vtxDesc[NUM_ELEMENTS];
		memset(vtxDesc, 0, sizeof(D3D11_INPUT_ELEMENT_DESC) * NUM_ELEMENTS);

		vtxDesc[0].AlignedByteOffset = 0;
		vtxDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vtxDesc[0].SemanticName = "POSITION";

		vtxDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		vtxDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vtxDesc[1].SemanticName = "NORMAL";

		if (Failure(m_pDXRenderer->GetD3D11Device()->CreateInputLayout(vtxDesc, NUM_ELEMENTS, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_pVSInputLayout)))
			return CLog::Log(S_ERROR, "Failed Create (Simple-Vertex) input layout for VS!");
	}
	else
	{
		const unsigned int NUM_ELEMENTS = 5;

		D3D11_INPUT_ELEMENT_DESC vtxDesc[NUM_ELEMENTS];
		memset(vtxDesc, 0, sizeof(D3D11_INPUT_ELEMENT_DESC) * NUM_ELEMENTS);

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

		/* // Second texture coordinate:
		vtxDesc[4].AlignedByteOffset = 0;
		vtxDesc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vtxDesc[4].InputSlot = 0;
		vtxDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		vtxDesc[4].SemanticIndex = 1;
		vtxDesc[4].SemanticName = "TEXCOORD";
		*/

		vtxDesc[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		vtxDesc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vtxDesc[4].InputSlot = 0;
		vtxDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		vtxDesc[4].SemanticIndex = 0;
		vtxDesc[4].SemanticName = "COLOR";

		if (Failure(m_pDXRenderer->GetD3D11Device()->CreateInputLayout(vtxDesc, NUM_ELEMENTS, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_pVSInputLayout)))
			return m_pEngine->LogE("Failed Create input layout for VS!");
	}



	// 6. Clear unneeded stuff

	SP_SAFE_RELEASE(pVSBlob);
	SP_SAFE_RELEASE(pPSBlob);
	SP_SAFE_RELEASE(pErrorBlob);



	return Failure(hRes) ? S_ERROR : S_SUCCESS;
}

// -------------------------------------------------------------------------
S_API void DirectX11Shader::Clear()
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
}

// -------------------------------------------------------------------------
S_API SResult DirectX11Shader::Bind()
{
	if (!IS_VALID_PTR(m_pDXRenderer) || !IS_VALID_PTR(m_pPixelShader) || !IS_VALID_PTR(m_pVertexShader) || !IS_VALID_PTR(m_pVSInputLayout))
		return S_NOTINIT;	

	ID3D11DeviceContext* pDXDeviceContext = m_pDXRenderer->GetD3D11DeviceContext();	
	if (!IS_VALID_PTR(pDXDeviceContext))
		return S_NOTINIT;

	pDXDeviceContext->VSSetShader(m_pVertexShader, 0, 0);		
	pDXDeviceContext->PSSetShader(m_pPixelShader, 0, 0);	
	pDXDeviceContext->IASetInputLayout(m_pVSInputLayout);	

	return S_SUCCESS;
}







////////////////////////////////////////////////////////////////////////////////////////

//				Helper Shader Pass

////////////////////////////////////////////////////////////////////////////////////////

S_API SResult HelperShaderPass::Initialize(IRenderer* pRenderer)
{
	Clear();
	m_pRenderer = pRenderer;

	// Create shader
	SShaderInfo shaderInfo;
	shaderInfo.filename = pRenderer->GetEngine()->GetShaderPath(eSHADERFILE_HELPER);
	shaderInfo.entry = "helper";	
	shaderInfo.vertexType = eSHADERVERTEX_SIMPLE;

	m_pShader = m_pRenderer->CreateShader();
	m_pShader->Load(pRenderer, shaderInfo);

	// Create CB
	m_pConstants = m_pRenderer->CreateConstantsBuffer<SHelperConstants>();

	return S_SUCCESS;
}

S_API void HelperShaderPass::Clear()
{
	delete m_pConstants;
	m_pConstants = 0;

	delete m_pShader;
	m_pShader = 0;

	m_pRenderer = 0;
}

S_API SResult HelperShaderPass::Bind()
{
	if (!IS_VALID_PTR(m_pRenderer))
		return S_NOTINITED;

	m_pShader->Bind();

	m_pRenderer->BindSingleRT(m_pRenderer->GetTargetViewport());

	m_pRenderer->BindTexture((ITexture*)0, 0);
	m_pRenderer->BindTexture((ITexture*)0, 1);

	m_pRenderer->BindConstantsBuffer(m_pConstants);

	return S_SUCCESS;
}

S_API void HelperShaderPass::SetShaderResources(const SShaderResources& shaderResources, const SMatrix4& transform)
{
	SHelperConstants* constants = m_pConstants->GetConstants();
	constants->mtxTransform = transform;
	constants->color = shaderResources.diffuse;

	m_pConstants->Update();
}






////////////////////////////////////////////////////////////////////////////////////////

//				GBuffer Shader Pass

////////////////////////////////////////////////////////////////////////////////////////

S_API SResult GBufferShaderPass::Initialize(IRenderer* pRenderer)
{
	Clear();
	m_pGBuffer.resize(NUM_GBUFFER_LAYERS);

	const SSettingsDesc& settings = pRenderer->GetEngine()->GetSettings()->Get();
	for (int i = 0; i < NUM_GBUFFER_LAYERS; ++i)
	{
		m_pGBuffer[i] = pRenderer->CreateRT();
		m_pGBuffer[i]->Initialize(eFBO_R8G8B8A8, pRenderer, settings.app.nXResolution, settings.app.nYResolution);
		m_pGBuffer[i]->InitializeSRV();
		
		// Make the first layer carry the depth buffer
		if (i == 0)
			m_pGBuffer[i]->InitializeDSV();
	}

	SShaderInfo si;
	si.filename = pRenderer->GetEngine()->GetShaderPath(eSHADERFILE_ZPASS);
	si.entry = "zpass";

	m_pShader = pRenderer->CreateShader();
	m_pShader->Load(pRenderer, si);

	m_pConstants = pRenderer->CreateConstantsBuffer<SGBufferPassConstants>();

	return S_SUCCESS;
}

S_API void GBufferShaderPass::Clear()
{
	for (int i = 0; i < NUM_GBUFFER_LAYERS; ++i)
		delete m_pGBuffer[i];

	m_pGBuffer.clear();

	delete m_pConstants;
	m_pConstants = 0;

	delete m_pShader;
	m_pShader = 0;

	m_pRenderer = 0;
}

S_API SResult GBufferShaderPass::Bind()
{
	if (!IS_VALID_PTR(m_pRenderer))
		return S_NOTINITED;

	m_pShader->Bind();

	m_pRenderer->BindRTCollection(m_pGBuffer, m_pGBuffer[0], "GBufferShaderPass");
	m_pRenderer->BindConstantsBuffer(m_pConstants);

	return S_SUCCESS;
}

S_API void GBufferShaderPass::SetShaderResources(const SShaderResources& sr, const SMatrix4& transform)
{
	m_pRenderer->EnableBackfaceCulling(sr.enableBackfaceCulling);

	// Bind textures
	ITexture* pTextureMap = IS_VALID_PTR(sr.textureMap) ? sr.textureMap : m_pRenderer->GetDummyTexture();
	ITexture* pNormalMap = IS_VALID_PTR(sr.normalMap) ? sr.normalMap : m_pRenderer->GetDummyTexture();

	m_pRenderer->BindTexture(pTextureMap, 0);
	m_pRenderer->BindTexture(pNormalMap, 1);



	// Set constants
	SGBufferPassConstants* constants = m_pConstants->GetConstants();


	//TODO: Use correct material parameters here (roughness, fresnel coefficient, illumination model, ...)
	
	
		
	constants->mtxTransform = transform;
	constants->matEmissive = sr.emissive;
	constants->matAmbient = 0.1f;




	m_pConstants->Update();	
}

S_API const vector<IFBO*>& GBufferShaderPass::GetGBuffer() const
{
	return m_pGBuffer;
}




////////////////////////////////////////////////////////////////////////////////////////

//				Deferred Shading (Illumination) Shader Pass

////////////////////////////////////////////////////////////////////////////////////////

S_API SResult ShadingShaderPass::Initialize(IRenderer* pRenderer)
{
	m_pRenderer = pRenderer;

	SShaderInfo si;
	si.filename = pRenderer->GetEngine()->GetShaderPath(eSHADERFILE_DEFERRED_SHADING);
	si.entry = "illum";
	si.vertexType = eSHADERVERTEX_SIMPLE;

	m_pShader = pRenderer->CreateShader();
	m_pShader->Load(pRenderer, si);

	m_pConstants = pRenderer->CreateConstantsBuffer<SShadingPassConstants>();

	return S_SUCCESS;
}

S_API void ShadingShaderPass::Clear()
{
	delete m_pConstants;
	m_pConstants = 0;

	delete m_pShader;
	m_pShader = 0;

	m_pGBufferPass = 0;
	m_pShadowmapPass = 0;
	m_pRenderer = 0;
}

S_API SResult ShadingShaderPass::Bind()
{
	m_pShader->Bind();

	// Use the target viewport as the output
	m_pRenderer->BindSingleRT(m_pRenderer->GetTargetViewport());

	const vector<IFBO*>& gbuffer = m_pGBufferPass->GetGBuffer();
	for (int i = 0; i < NUM_GBUFFER_LAYERS; ++i)
		m_pRenderer->BindTexture(gbuffer[i], i);

	return S_SUCCESS;
}

S_API void ShadingShaderPass::SetShaderResources(const SShaderResources& pShaderResources, const SMatrix4& transform)
{
	// transform should be the transformation matrix of the light volume
	SShadingPassConstants* constants = m_pConstants->GetConstants();
	constants->mtxTransform = transform;

	m_pConstants->Update();
}





////////////////////////////////////////////////////////////////////////////////////////

//				Shadowmap Shader Pass

////////////////////////////////////////////////////////////////////////////////////////

S_API SResult ShadowmapShaderPass::Initialize(IRenderer* pRenderer)
{

}

S_API void ShadowmapShaderPass::Clear()
{

}

S_API SResult ShadowmapShaderPass::Bind()
{

}




////////////////////////////////////////////////////////////////////////////////////////

//				Posteffect Shader Pass

////////////////////////////////////////////////////////////////////////////////////////

S_API SResult PosteffectShaderPass::Initialize(IRenderer* pRenderer)
{

}

S_API void PosteffectShaderPass::Clear()
{

}

S_API SResult PosteffectShaderPass::Bind()
{

}



SP_NMSPACE_END
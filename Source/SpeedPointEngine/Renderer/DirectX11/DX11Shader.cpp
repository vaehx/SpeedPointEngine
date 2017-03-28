//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2016 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "DX11Shader.h"
#include "DX11Renderer.h"
#include <d3dcompiler.h>

#include <fstream>
using std::ifstream;

SP_NMSPACE_BEG


inline void FillSimpleInputLayout(D3D11_INPUT_ELEMENT_DESC** pInputLayout, unsigned int* pNumElements, const char** pName)
{
	*pName = "SHADERINPUTLAYOUT_SIMPLE";	
	unsigned int numElements = *pNumElements = 2;

	D3D11_INPUT_ELEMENT_DESC* vtxDesc = *pInputLayout = new D3D11_INPUT_ELEMENT_DESC[numElements];
	memset(vtxDesc, 0, sizeof(D3D11_INPUT_ELEMENT_DESC) * numElements);

	vtxDesc[0].AlignedByteOffset = 0;
	vtxDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vtxDesc[0].SemanticName = "POSITION";

	vtxDesc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vtxDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vtxDesc[1].SemanticName = "NORMAL";
}

inline void FillDefaultInputLayout(D3D11_INPUT_ELEMENT_DESC** pInputLayout, unsigned int* pNumElements, const char** pName)
{
	*pName = "SHADERINPUTLAYOUT_DEFAULT";
	unsigned int numElements = *pNumElements = 5;

	D3D11_INPUT_ELEMENT_DESC* vtxDesc = *pInputLayout = new D3D11_INPUT_ELEMENT_DESC[numElements];
	memset(vtxDesc, 0, sizeof(D3D11_INPUT_ELEMENT_DESC) * numElements);

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
}

inline void FillParticleInputLayout(D3D11_INPUT_ELEMENT_DESC** pInputLayout, unsigned int* pNumElements, const char** pName)
{
	FillDefaultInputLayout(pInputLayout, pNumElements, pName);
	unsigned int defaultElements = *pNumElements;
	D3D11_INPUT_ELEMENT_DESC* defaultInputLayout = *pInputLayout;

	*pName = "SHADERINPUTLAYOUT_PARTICLES";	
	unsigned int numElements = *pNumElements = *pNumElements + 2;

	D3D11_INPUT_ELEMENT_DESC* vtxDesc = *pInputLayout = new D3D11_INPUT_ELEMENT_DESC[numElements];
	memcpy(vtxDesc, defaultInputLayout, sizeof(D3D11_INPUT_ELEMENT_DESC) * defaultElements);
	memset(vtxDesc + defaultElements, 0, sizeof(D3D11_INPUT_ELEMENT_DESC) * (numElements - defaultElements));

	delete[] defaultInputLayout;
	defaultInputLayout = 0;

	// Vec3f direction
	vtxDesc[defaultElements + 0].AlignedByteOffset = 0;	// !!!
	vtxDesc[defaultElements + 0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vtxDesc[defaultElements + 0].InputSlot = 1;
	vtxDesc[defaultElements + 0].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	vtxDesc[defaultElements + 0].InstanceDataStepRate = 1;
	vtxDesc[defaultElements + 0].SemanticIndex = 1;
	vtxDesc[defaultElements + 0].SemanticName = "TEXCOORD";

	// unsigned __int32 startTime
	vtxDesc[defaultElements + 1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vtxDesc[defaultElements + 1].Format = DXGI_FORMAT_R32_UINT;
	vtxDesc[defaultElements + 1].InputSlot = 1;
	vtxDesc[defaultElements + 1].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	vtxDesc[defaultElements + 1].InstanceDataStepRate = 1;
	vtxDesc[defaultElements + 1].SemanticIndex = 2;
	vtxDesc[defaultElements + 1].SemanticName = "TEXCOORD";
}



// -----------------------------------------------------------------------------------------------
DX11Shader::DX11Shader()
: m_pDXRenderer(0),
m_pPixelShader(0),
m_pVertexShader(0),
m_pVSInputLayout(0)
{
}

// -----------------------------------------------------------------------------------------------
DX11Shader::~DX11Shader()
{
	Clear();
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Shader::Load(IRenderer* pRenderer, const SShaderInfo& info)
{
	assert(IS_VALID_PTR(pRenderer));
	assert(pRenderer->GetType() == S_DIRECTX11);

	Clear();

	m_pDXRenderer = dynamic_cast<DX11Renderer*>(pRenderer);
	assert(IS_VALID_PTR(m_pDXRenderer));


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
		return CLog::Log(S_ERROR, "Failed open FX file ifstream for %s (entry: %s)", info.filename.c_str(), info.entry.c_str());
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
	
	unsigned int numElements = 0;
	D3D11_INPUT_ELEMENT_DESC* vtxDesc = 0;
	const char* layoutName;

	switch (info.inputLayout)
	{
	case eSHADERINPUTLAYOUT_SIMPLE: // SSimpleVertex
		FillSimpleInputLayout(&vtxDesc, &numElements, &layoutName);
		break;
	case eSHADERINPUTLAYOUT_PARTICLES: // SVertex + SParticleInstance
		FillParticleInputLayout(&vtxDesc, &numElements, &layoutName);
		break;
	case eSHADERINPUTLAYOUT_DEFAULT: // SVertex
	default:
		FillDefaultInputLayout(&vtxDesc, &numElements, &layoutName);
		break;
	}

	if (Failure(m_pDXRenderer->GetD3D11Device()->CreateInputLayout(vtxDesc, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_pVSInputLayout)))
	{
		delete[] vtxDesc;
		return CLog::Log(S_ERROR, "Failed Create (%s) input layout for VS!", layoutName);
	}

	delete[] vtxDesc;
	vtxDesc = 0;


	// 6. Clear unneeded stuff

	SP_SAFE_RELEASE(pVSBlob);
	SP_SAFE_RELEASE(pPSBlob);
	SP_SAFE_RELEASE(pErrorBlob);



	return Failure(hRes) ? S_ERROR : S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API void DX11Shader::Clear()
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
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11Shader::Bind()
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

//				Forward Shader Pass

////////////////////////////////////////////////////////////////////////////////////////

S_API SResult ForwardShaderPass::Initialize(IRenderer* pRenderer)
{
	Clear();
	m_pRenderer = pRenderer;

	// Create shaders
	SShaderInfo si;
	si.filename = pRenderer->GetShaderPath(eSHADERFILE_FORWARD_HELPER);
	si.entry = "helper";	
	si.inputLayout = eSHADERINPUTLAYOUT_DEFAULT;

	m_pHelperShader = m_pRenderer->CreateShader();
	m_pHelperShader->Load(pRenderer, si);


	si.filename = pRenderer->GetShaderPath(eSHADERFILE_FORWARD);
	si.entry = "forward";
	si.inputLayout = eSHADERINPUTLAYOUT_DEFAULT;

	m_pShader = m_pRenderer->CreateShader();
	m_pShader->Load(pRenderer, si);


	si.filename = pRenderer->GetShaderPath(eSHADERFILE_SKYBOX);
	si.entry = "skybox";
	si.inputLayout = eSHADERINPUTLAYOUT_DEFAULT;

	m_pSkyboxShader = m_pRenderer->CreateShader();
	m_pSkyboxShader->Load(pRenderer, si);


	// Create CB
	m_Constants.Initialize(pRenderer);	

	return S_SUCCESS;
}

S_API void ForwardShaderPass::Clear()
{
	m_Constants.Clear();

	if (m_pShader)
	{
		m_pShader->Clear();
		delete m_pShader;
		m_pShader = 0;
	}

	if (m_pHelperShader)
	{
		m_pHelperShader->Clear();
		delete m_pHelperShader;
		m_pHelperShader = 0;
	}

	if (m_pSkyboxShader)
	{
		m_pSkyboxShader->Clear();
		delete m_pSkyboxShader;
		m_pSkyboxShader = 0;
	}

	m_pRenderer = 0;
}

S_API SResult ForwardShaderPass::Bind()
{
	if (!IS_VALID_PTR(m_pRenderer))
		return S_NOTINITED;

	m_pShader->Bind();

	m_pRenderer->BindSingleRT(m_pRenderer->GetTargetViewport());

	m_pRenderer->BindTexture((ITexture*)0, 0);
	m_pRenderer->BindTexture((ITexture*)0, 1);

	m_pRenderer->BindConstantsBuffer(m_Constants.GetCB());

	return S_SUCCESS;
}

S_API void ForwardShaderPass::SetShaderResources(const SShaderResources& sr, const SMatrix4& transform)
{
	if (sr.illumModel == eILLUM_HELPER)
	{
		m_pHelperShader->Bind();

		m_pRenderer->BindTexture((ITexture*)0, 0);
		m_pRenderer->BindTexture((ITexture*)0, 1);
	}
	else if (sr.illumModel == eILLUM_SKYBOX)
	{
		m_pSkyboxShader->Bind();

		m_pRenderer->EnableBackfaceCulling(false);

		m_pRenderer->BindTexture(sr.textureMap, 0);
		m_pRenderer->BindTexture((ITexture*)0, 1);
	}
	else
	{
		m_pShader->Bind();

		m_pRenderer->EnableBackfaceCulling(sr.enableBackfaceCulling);

		m_pRenderer->BindTexture(sr.textureMap, 0);
		m_pRenderer->BindTexture(sr.normalMap, 1);
	}



	// Set constants
	SMatObjConstants* constants = m_Constants.GetConstants();


	//TODO: Use correct material parameters here (roughness, fresnel coefficient, illumination model, ...)



	constants->mtxWorld = transform;
	constants->matEmissive = sr.diffuse;
	constants->matAmbient = 0.1f;

	m_Constants.Update();
}






////////////////////////////////////////////////////////////////////////////////////////

//				GBuffer Shader Pass

////////////////////////////////////////////////////////////////////////////////////////

S_API SResult GBufferShaderPass::Initialize(IRenderer* pRenderer)
{
	Clear();
	m_pGBuffer.resize(NUM_GBUFFER_LAYERS);

	for (int i = 0; i < NUM_GBUFFER_LAYERS; ++i)
	{
		m_pGBuffer[i] = pRenderer->CreateRT();
		m_pGBuffer[i]->Initialize(eFBO_R8G8B8A8, pRenderer, pRenderer->GetParams().resolution[0], pRenderer->GetParams().resolution[1]);
		m_pGBuffer[i]->InitializeSRV();
		
		// Make the first layer carry the depth buffer
		if (i == 0)
			m_pGBuffer[i]->InitializeDSV();
	}

	SShaderInfo si;
	si.filename = pRenderer->GetShaderPath(eSHADERFILE_DEFERRED_ZPASS);
	si.entry = "zpass";

	m_pShader = pRenderer->CreateShader();
	m_pShader->Load(pRenderer, si);

	m_Constants.Initialize(pRenderer);	

	return S_SUCCESS;
}

S_API void GBufferShaderPass::Clear()
{
	for (auto itLayer = m_pGBuffer.begin(); itLayer != m_pGBuffer.end(); ++itLayer)
	{
		IFBO* pLayer = *itLayer;
		if (pLayer)
		{
			pLayer->Clear();
			delete pLayer;
			*itLayer = 0;
		}
	}
	m_pGBuffer.clear();

	m_Constants.Clear();

	if (m_pShader)
	{
		m_pShader->Clear();
		delete m_pShader;
		m_pShader = 0;
	}

	m_pRenderer = 0;
}

S_API SResult GBufferShaderPass::Bind()
{
	if (!IS_VALID_PTR(m_pRenderer))
		return S_NOTINITED;

	m_pShader->Bind();

	m_pRenderer->BindRTCollection(m_pGBuffer, m_pGBuffer[0], "GBufferShaderPass");
	m_pRenderer->BindConstantsBuffer(m_Constants.GetCB());

	return S_SUCCESS;
}

S_API void GBufferShaderPass::SetShaderResources(const SShaderResources& sr, const SMatrix4& transform)
{
	if (sr.illumModel == eILLUM_HELPER)
		return; // helpers cannot be rendered deferred

	m_pRenderer->EnableBackfaceCulling(sr.enableBackfaceCulling);

	// Bind textures
	ITexture* pTextureMap = IS_VALID_PTR(sr.textureMap) ? sr.textureMap : m_pRenderer->GetDummyTexture();
	ITexture* pNormalMap = IS_VALID_PTR(sr.normalMap) ? sr.normalMap : m_pRenderer->GetDummyTexture();

	m_pRenderer->BindTexture(pTextureMap, 0);
	m_pRenderer->BindTexture(pNormalMap, 1);



	// Set constants
	SMatObjConstants* constants = m_Constants.GetConstants();


	//TODO: Use correct material parameters here (roughness, fresnel coefficient, illumination model, ...)
	
	
		
	constants->mtxWorld = transform;
	constants->matEmissive = sr.emissive;
	constants->matAmbient = 0.1f;




	m_Constants.Update();	
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
	si.filename = pRenderer->GetShaderPath(eSHADERFILE_DEFERRED_SHADING);
	si.entry = "illum";
	si.inputLayout = eSHADERINPUTLAYOUT_DEFAULT;

	m_pShader = pRenderer->CreateShader();
	m_pShader->Load(pRenderer, si);

	m_Constants.Initialize(pRenderer);	

	return S_SUCCESS;
}

S_API void ShadingShaderPass::Clear()
{
	m_Constants.Clear();

	if (m_pShader)
	{
		m_pShader->Clear();
		delete m_pShader;
		m_pShader = 0;
	}

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
	SShadingPassConstants* constants = m_Constants.GetConstants();
	constants->mtxWorld = transform;

	m_Constants.Update();
}





////////////////////////////////////////////////////////////////////////////////////////

//				Shadowmap Shader Pass

////////////////////////////////////////////////////////////////////////////////////////

S_API SResult ShadowmapShaderPass::Initialize(IRenderer* pRenderer)
{
	return S_SUCCESS;
}

S_API void ShadowmapShaderPass::Clear()
{

}

S_API SResult ShadowmapShaderPass::Bind()
{
	return S_SUCCESS;
}

S_API void ShadowmapShaderPass::SetShaderResources(const SShaderResources& sr, const SMatrix4& transform)
{
}





////////////////////////////////////////////////////////////////////////////////////////

//				Particle Shader Pass

////////////////////////////////////////////////////////////////////////////////////////

S_API SResult ParticleShaderPass::Initialize(IRenderer* pRenderer)
{
	m_pRenderer = pRenderer;

	SShaderInfo si;
	si.entry = "ParticleEmitter";
	si.filename = pRenderer->GetShaderPath(eSHADERFILE_PARTICLES);
	si.inputLayout = eSHADERINPUTLAYOUT_PARTICLES;

	m_pShader = pRenderer->CreateShader();
	if (Failure(m_pShader->Load(pRenderer, si)))
		return S_ERROR;

	m_Constants.Initialize(pRenderer);
	return S_SUCCESS;
}

S_API void ParticleShaderPass::Clear()
{
	m_Constants.Clear();
	if (m_pShader)
	{
		m_pShader->Clear();
		delete m_pShader;
	}
	m_pShader = 0;
	m_pRenderer = 0;
}

S_API SResult ParticleShaderPass::Bind()
{
	m_pShader->Bind();
	m_pRenderer->BindSingleRT(m_pRenderer->GetTargetViewport());
	m_pRenderer->BindConstantsBuffer(m_Constants.GetCB());
	return S_SUCCESS;
}

S_API void ParticleShaderPass::SetShaderResources(const SShaderResources& shaderResources, const SMatrix4& transform)
{
	m_pRenderer->BindTexture(shaderResources.textureMap);

	SObjectConstants* constants = m_Constants.GetConstants();
	constants->mtxWorld = transform;
	m_Constants.Update();
}

S_API void ParticleShaderPass::SetConstants(const SParticleEmitterConstants& constants)
{
	SParticleEmitterConstants* pConstants = m_Constants.GetConstants();
	memcpy(pConstants, &constants, sizeof(SParticleEmitterConstants));
}




////////////////////////////////////////////////////////////////////////////////////////

//				GUI Shader Pass

////////////////////////////////////////////////////////////////////////////////////////


S_API SResult GUIShaderPass::Initialize(IRenderer* pRenderer)
{
	m_pRenderer = pRenderer;

	SShaderInfo si;
	si.entry = "GUI";
	si.filename = pRenderer->GetShaderPath(eSHADERFILE_GUI);
	si.inputLayout = eSHADERINPUTLAYOUT_DEFAULT;
	
	m_pShader = pRenderer->CreateShader();
	m_pShader->Load(pRenderer, si);

	m_Constants.Initialize(pRenderer);
	return S_SUCCESS;
}

S_API void GUIShaderPass::Clear()
{
	m_Constants.Clear();
	if (m_pShader)
	{
		m_pShader->Clear();
		delete m_pShader;
	}
	m_pShader = 0;
	m_pRenderer = 0;
}

S_API SResult GUIShaderPass::Bind()
{
	if (!IS_VALID_PTR(m_pRenderer))
		return S_NOTINITED;

	m_pShader->Bind();

	m_pRenderer->BindSingleRT(m_pRenderer->GetTargetViewport());

	m_pRenderer->BindConstantsBuffer(m_Constants.GetCB());

	return S_SUCCESS;
}

S_API void GUIShaderPass::SetShaderResources(const SShaderResources& shaderResources, const Mat44& transform)
{
	m_pRenderer->BindTexture(shaderResources.textureMap);

	SObjectConstants* constants = m_Constants.GetConstants();
	constants->mtxWorld = transform;
	m_Constants.Update();
}






////////////////////////////////////////////////////////////////////////////////////////

//				Posteffect Shader Pass

////////////////////////////////////////////////////////////////////////////////////////

S_API SResult PosteffectShaderPass::Initialize(IRenderer* pRenderer)
{
	return S_SUCCESS;
}

S_API void PosteffectShaderPass::Clear()
{

}

S_API SResult PosteffectShaderPass::Bind()
{
	return S_SUCCESS;
}

S_API void PosteffectShaderPass::SetShaderResources(const SShaderResources& sr, const SMatrix4& transform)
{
}


SP_NMSPACE_END

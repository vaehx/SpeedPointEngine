//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2016 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "DX11Shader.h"
#include "DX11Renderer.h"
#include "..\IResourcePool.h"
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
	string entryNameMacro = "ENTRY_" + info.entry;

	D3D_SHADER_MACRO macros[] =
	{
		{ entryNameMacro.c_str(), "true" },
		{ "", "" }, // Shader Stage (Vertex Shader/Pixel Shader)
		{ 0, 0 }
	};

	ID3DBlob *pVSBlob = 0, *pPSBlob = 0, *pErrorBlob = 0;

	macros[1].Name = "VERTEX_SHADER";
	if (Failure(D3DCompile(
		(void*)fxBuffer,
		size,
		0, macros, 0, composedEntryName.c_str(),
		"vs_4_0",
		compileFlags,
		0,
		&pVSBlob, &pErrorBlob)))
	{
		string msgBoxTitle = "Effect compile error of '";
		msgBoxTitle += composedEntryName;
		MessageBox(0, (CHAR*)pErrorBlob->GetBufferPointer(), msgBoxTitle.c_str(), MB_OK | MB_ICONERROR);
		return S_ERROR;
	}

	macros[1].Name = "PIXEL_SHADER";
	composedEntryName[0] = 'P'; // VS... -> PS...
	if (Failure(D3DCompile(
		(void*)fxBuffer,
		size,
		0, macros, 0, composedEntryName.c_str(),
		"ps_4_0",
		compileFlags,
		0,
		&pPSBlob, &pErrorBlob)))
	{
		string msgBoxTitle = "Effect compile error of '";
		msgBoxTitle += composedEntryName;
		MessageBox(0, (CHAR*)pErrorBlob->GetBufferPointer(), msgBoxTitle.c_str(), MB_OK | MB_ICONERROR);
		return S_ERROR;
	}

	CLog::Log(S_DEBUG, "Loaded and compiled effect '%s' (Entry: '%s')", info.filename.c_str(), info.entry.c_str());



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

	// Bind vertex shader
	ID3D11VertexShader* pBoundVS = 0;
	pDXDeviceContext->VSGetShader(&pBoundVS, 0, 0);
	if (pBoundVS != m_pVertexShader)
	{
		pDXDeviceContext->VSSetShader(m_pVertexShader, 0, 0);
		pDXDeviceContext->IASetInputLayout(m_pVSInputLayout);
	}

	SP_SAFE_RELEASE(pBoundVS);

	// Bind pixel shader
	ID3D11PixelShader* pBoundPS = 0;
	pDXDeviceContext->PSGetShader(&pBoundPS, 0, 0);
	if (pBoundPS != m_pPixelShader)
		pDXDeviceContext->PSSetShader(m_pPixelShader, 0, 0);

	SP_SAFE_RELEASE(pBoundPS);

	return S_SUCCESS;
}





/*

Forward Render pipeline:
	
	Shadowmap -> Forward / Particles -> Post-FX -> GUI


Deferred Render pipeline:
	
	Shadowmap -> GBuffer (Albedo, Normals, Depth) -> Shading -> Forward / Particles -> Post-FX -> GUI


*/



////////////////////////////////////////////////////////////////////////////////////////

//				Shadowmap Shader Pass

////////////////////////////////////////////////////////////////////////////////////////

S_API ShadowmapShaderPass::ShadowmapShaderPass()
	: m_pShadowmap(0)
{
}

S_API SResult ShadowmapShaderPass::Initialize(IRenderer* pRenderer)
{
	Clear();
	SP_ASSERTR(m_pRenderer = pRenderer, S_INVALIDPARAM);

	// Create shadowmap FBO
	// TODO: We probably want to use a different resolution shadow map than the viewport...
	SIZE viewportSz = pRenderer->GetTargetViewport()->GetSize();
	unsigned int shadowmapResolution[2] = { (unsigned int)viewportSz.cx, (unsigned int)viewportSz.cy };

	m_pShadowmap = pRenderer->CreateRT();
	m_pShadowmap->InitializeDepthBufferAsTexture("$shadowmap", shadowmapResolution);

	SSceneConstants* sceneConstants = m_pRenderer->GetSceneConstants();
	sceneConstants->shadowMapRes[0] = viewportSz.cx;
	sceneConstants->shadowMapRes[1] = viewportSz.cy;

	ReloadShaders();

	// Initialize constants
	m_Constants.Initialize(pRenderer);

	return S_SUCCESS;
}

S_API void ShadowmapShaderPass::ReloadShaders()
{
	if (m_pShader)
	{
		m_pShader->Clear();
		delete m_pShader;
	}

	// Create shadowmap shader
	SShaderInfo si;
	si.entry = "shadowmap";
	si.filename = m_pRenderer->GetShaderPath(eSHADERFILE_SHADOW);
	si.inputLayout = eSHADERINPUTLAYOUT_SIMPLE;

	m_pShader = m_pRenderer->CreateShader();
	m_pShader->Load(m_pRenderer, si);
}

S_API void ShadowmapShaderPass::Clear()
{
	if (m_pShadowmap)
	{
		m_pShadowmap->Clear();
		delete m_pShadowmap;
		m_pShadowmap = 0;
	}

	if (m_pShader)
	{
		m_pShader->Clear();
		delete m_pShader;
		m_pShader = 0;
	}
	
	m_Constants.Clear();
}

S_API SResult ShadowmapShaderPass::Bind()
{
	if (!m_pRenderer)
		return S_ERROR;

	m_pShader->Bind();
	m_pRenderer->UnbindTexture(m_pShadowmap->GetDepthBufferTexture());
	m_pRenderer->BindSingleRT(m_pShadowmap);
	m_pRenderer->BindConstantsBuffer(m_Constants.GetCB());

	return S_SUCCESS;
}

S_API void ShadowmapShaderPass::SetShaderResources(const SShaderResources& sr, const Mat44& transform)
{
	SObjectConstants* constants = m_Constants.GetConstants();
	constants->mtxWorld = transform;
	m_Constants.Update();
}

S_API void ShadowmapShaderPass::OnEndFrame()
{
	if (m_pShadowmap)
	{
		m_pRenderer->UnbindTexture(m_pShadowmap->GetTexture());
		m_pRenderer->UnbindTexture(m_pShadowmap->GetDepthBufferTexture());
		m_pRenderer->BindSingleRT(m_pShadowmap);
		m_pRenderer->ClearBoundRTs(false, true);
	}
}

S_API IFBO* ShadowmapShaderPass::GetShadowmap() const
{
	return m_pShadowmap;
}






////////////////////////////////////////////////////////////////////////////////////////

//				Forward Shader Pass

////////////////////////////////////////////////////////////////////////////////////////

S_API SResult ForwardShaderPass::Initialize(IRenderer* pRenderer)
{
	Clear();
	m_pRenderer = pRenderer;

	ReloadShaders();


	// Create CB
	SMatObjConstants _constants;
	m_Constants.Initialize(pRenderer);

	m_HelperConstants.Initialize(pRenderer);

	return S_SUCCESS;
}

S_API void ForwardShaderPass::ReloadShaders()
{
	SShaderInfo si;

	// Helper shader
	si.filename = m_pRenderer->GetShaderPath(eSHADERFILE_HELPER);
	si.entry = "helper";
	si.inputLayout = eSHADERINPUTLAYOUT_DEFAULT;

	if (m_pHelperShader)
	{
		m_pHelperShader->Clear();
		delete m_pHelperShader;
	}

	m_pHelperShader = m_pRenderer->CreateShader();
	m_pHelperShader->Load(m_pRenderer, si);

	// Forward shader
	si.filename = m_pRenderer->GetShaderPath(eSHADERFILE_ILLUM);
	si.entry = "forward";
	si.inputLayout = eSHADERINPUTLAYOUT_DEFAULT;

	if (m_pShader)
	{
		m_pShader->Clear();
		delete m_pShader;
	}

	m_pShader = m_pRenderer->CreateShader();
	m_pShader->Load(m_pRenderer, si);

	// Skybox shader
	si.filename = m_pRenderer->GetShaderPath(eSHADERFILE_SKYBOX);
	si.entry = "skybox";
	si.inputLayout = eSHADERINPUTLAYOUT_DEFAULT;

	if (m_pSkyboxShader)
	{
		m_pSkyboxShader->Clear();
		delete m_pSkyboxShader;
	}

	m_pSkyboxShader = m_pRenderer->CreateShader();
	m_pSkyboxShader->Load(m_pRenderer, si);
}

S_API void ForwardShaderPass::Clear()
{
	m_Constants.Clear();
	m_HelperConstants.Clear();

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

	return S_SUCCESS;
}

S_API void ForwardShaderPass::SetShaderResources(const SShaderResources& sr, const Mat44& transform)
{
	ShadowmapShaderPass* pShadowmapPass =
		dynamic_cast<ShadowmapShaderPass*>(m_pRenderer->GetShaderPass(eSHADERPASS_SHADOWMAP));

	bool regularConstants = true;
	if (sr.illumModel == eILLUM_HELPER)
	{
		m_pHelperShader->Bind();
		m_pRenderer->BindConstantsBuffer(m_HelperConstants.GetCB());

		m_pRenderer->BindTexture((ITexture*)0, 0);
		m_pRenderer->BindTexture((ITexture*)0, 1);

		regularConstants = false;
		SHelperObjConstants* constants = m_HelperConstants.GetConstants();
		constants->mtxWorld = transform;
		constants->helperColor[0] = sr.diffuse.x;
		constants->helperColor[1] = sr.diffuse.y;
		constants->helperColor[2] = sr.diffuse.z;
		
		m_HelperConstants.Update();
	}
	else if (sr.illumModel == eILLUM_SKYBOX)
	{
		m_pSkyboxShader->Bind();
		m_pRenderer->BindConstantsBuffer(m_Constants.GetCB());

		m_pRenderer->EnableBackfaceCulling(false);
		m_pRenderer->EnableDepthTest(true, false);

		m_pRenderer->BindTexture(sr.textureMap ? sr.textureMap : m_pRenderer->GetDummyTexture(), 0);
		m_pRenderer->BindTexture((ITexture*)0, 1);
	}
	else
	{
		m_pShader->Bind();
		m_pRenderer->BindConstantsBuffer(m_Constants.GetCB());

		m_pRenderer->EnableBackfaceCulling(sr.enableBackfaceCulling);

		m_pRenderer->BindTexture(sr.textureMap ? sr.textureMap : m_pRenderer->GetDummyTexture(), 0);
		m_pRenderer->BindTexture(sr.normalMap ? sr.normalMap : m_pRenderer->GetDummyNormalmap(), 1);

		m_pRenderer->BindDepthBufferAsTexture(pShadowmapPass->GetShadowmap(), 2);
	}

	// Set constants
	if (regularConstants)
	{
		SMatObjConstants* constants = m_Constants.GetConstants();

		constants->mtxWorld = transform;
		constants->matRoughnes = sr.roughness;

		m_Constants.Update();
	}
}




////////////////////////////////////////////////////////////////////////////////////////

//				GBuffer Shader Pass

/*

1st Pass: Render objects to MINIMAL gbuffer
	- See GBUFFER-LAYOUT below
2nd Pass: Render each light volume and accumulate luminous emittance of surface in light buffer
3rd Pass: Forward shading pass using light accumulation buffer


## GBUFFER-LAYOUT

	#1 GBUF0 Depth D32
	#2 GBUF1 Normal RGB8 + Roughness A8

TODO: We probably need more material parameters later (metallicness, fresnel coefficients, ...)

*/

////////////////////////////////////////////////////////////////////////////////////////

S_API SResult GBufferShaderPass::Initialize(IRenderer* pRenderer)
{
	Clear();
	m_pRenderer = pRenderer;
	
	unsigned int screenRes[2] = { pRenderer->GetParams().resolution[0], pRenderer->GetParams().resolution[1] };

	m_pGBuffer.resize(NUM_GBUFFER_LAYERS);
	for (int i = 0; i < NUM_GBUFFER_LAYERS; ++i)
		m_pGBuffer[i] = pRenderer->CreateRT();

	m_pGBuffer[0]->InitializeDepthBufferAsTexture("$GBuffer0", screenRes);
	m_pGBuffer[1]->InitializeAsTexture(eFBO_R8G8B8A8, screenRes[0], screenRes[1], "$GBuffer1");

	ReloadShaders();

	m_Constants.Initialize(pRenderer);
	m_TerrainConstants.Initialize(pRenderer);

	return S_SUCCESS;
}

S_API void GBufferShaderPass::ReloadShaders()
{
	SShaderInfo si;

	if (m_pShader)
	{
		m_pShader->Clear();
		delete m_pShader;
	}

	si.filename = m_pRenderer->GetShaderPath(eSHADERFILE_GBUFFER);
	si.entry = "GBuffer";

	m_pShader = m_pRenderer->CreateShader();
	m_pShader->Load(m_pRenderer, si);

	if (m_pTerrainShader)
	{
		m_pTerrainShader->Clear();
		delete m_pTerrainShader;
	}

	si.filename = m_pRenderer->GetShaderPath(eSHADERFILE_GBUFFER);
	si.entry = "GBufferTerrain";

	m_pTerrainShader = m_pRenderer->CreateShader();
	m_pTerrainShader->Load(m_pRenderer, si);
}

S_API void GBufferShaderPass::Clear()
{
	m_BoundFBOs.clear();

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

	BindGBufferRTs();

	return S_SUCCESS;
}

S_API void GBufferShaderPass::BindGBufferRTs()
{
	//--
	// It may be bound as a texture for debugging...
	for (auto gbuffer : m_pGBuffer)
		m_pRenderer->UnbindTexture(gbuffer->GetTexture());

	m_pRenderer->UnbindTexture(m_pGBuffer[0]->GetDepthBufferTexture());
	//--

	if (m_BoundFBOs.empty())
		m_BoundFBOs = { m_pGBuffer[1] };

	m_pRenderer->BindRTCollection(m_BoundFBOs, m_pGBuffer[0], false, "GBufferShaderPass");
	m_pRenderer->BindConstantsBuffer(m_Constants.GetCB());
}

S_API void GBufferShaderPass::OnUnbind()
{
	// Unbind terrain heightmap
	m_pRenderer->BindVertexShaderTexture(0, 0);
}

S_API void GBufferShaderPass::OnEndFrame()
{
	BindGBufferRTs();
	m_pRenderer->ClearBoundRTs();
}

S_API void GBufferShaderPass::SetShaderResources(const SShaderResources& sr, const Mat44& transform)
{
	if (sr.illumModel == eILLUM_HELPER)
		return; // helpers cannot be rendered deferred

	m_pShader->Bind();

	m_pRenderer->EnableBackfaceCulling(sr.enableBackfaceCulling);

	// Bind textures
	m_pRenderer->BindTexture(IS_VALID_PTR(sr.normalMap) ? sr.normalMap : m_pRenderer->GetDummyNormalmap(), 0);

	// Set constants
	SMatObjConstants* constants = m_Constants.GetConstants();
	constants->mtxWorld = transform;
	constants->matRoughnes = sr.roughness;

	m_Constants.Update();
}

S_API void GBufferShaderPass::BindTerrainResources(const STerrainShaderResources& terrainShaderResources, bool constantsUpdated)
{
	m_pTerrainShader->Bind();

	m_pRenderer->BindVertexShaderTexture(terrainShaderResources.pHeightmap, 0);

	// no albedo required in gbuffer pass!
	m_pRenderer->BindTexture(terrainShaderResources.pLayerMask, 0);
	m_pRenderer->BindTexture(terrainShaderResources.pNormalmap, 1);
	m_pRenderer->BindTexture(terrainShaderResources.pRoughnessmap, 2);

	STerrainConstants* constants = m_TerrainConstants.GetConstants();
	if (constantsUpdated && constants)
	{
		*constants = terrainShaderResources.constants;
		m_TerrainConstants.Update();
	}

	m_pRenderer->BindConstantsBuffer(m_TerrainConstants.GetCB());
}

S_API ITexture* GBufferShaderPass::GetGBufferTexture(unsigned int i) const
{
	if (i == 0)
		return m_pGBuffer[i]->GetDepthBufferTexture();
	else if (i < m_pGBuffer.size())
		return m_pGBuffer[i]->GetTexture();
	else
		return 0;
}



////////////////////////////////////////////////////////////////////////////////////////

//				Light prepass shader pass

////////////////////////////////////////////////////////////////////////////////////////

S_API SResult DeferredLightShaderPass::Initialize(IRenderer* pRenderer)
{
	Clear();
	m_pRenderer = pRenderer;

	unsigned int screenRes[2] = { pRenderer->GetParams().resolution[0], pRenderer->GetParams().resolution[1] };

	m_pLBDiffuse = m_pRenderer->CreateRT();
	m_pLBDiffuse->InitializeAsTexture(eFBO_R16G16B16A16F, screenRes[0], screenRes[1], "$LightDiffuse");

	m_pLBSpecular = m_pRenderer->CreateRT();
	m_pLBSpecular->InitializeAsTexture(eFBO_F32, screenRes[0], screenRes[1], "$LightSpecular");

	m_RenderTargets.push_back(m_pLBDiffuse);
	m_RenderTargets.push_back(m_pLBSpecular);

	ReloadShaders();

	m_Constants.Initialize(pRenderer);

	return S_SUCCESS;
}

S_API void DeferredLightShaderPass::ReloadShaders()
{
	if (m_pShader)
	{
		m_pShader->Clear();
		delete m_pShader;
	}

	SShaderInfo si;
	si.filename = m_pRenderer->GetShaderPath(eSHADERFILE_ILLUM);
	si.entry = "LightPrepass";
	si.inputLayout = eSHADERINPUTLAYOUT_SIMPLE;

	m_pShader = m_pRenderer->CreateShader();
	m_pShader->Load(m_pRenderer, si);
}

S_API ITexture* DeferredLightShaderPass::GetDiffuseLightBufferTexture() const
{
	return (m_pLBDiffuse ? m_pLBDiffuse->GetTexture() : 0);
}

S_API ITexture* DeferredLightShaderPass::GetSpecularLightBufferTexture() const
{
	return (m_pLBSpecular ? m_pLBSpecular->GetTexture() : 0);
}

S_API void DeferredLightShaderPass::Clear()
{
	m_Constants.Clear();

	if (m_pLBDiffuse)
	{
		m_pLBDiffuse->Clear();
		delete m_pLBDiffuse;
		m_pLBDiffuse = 0;
	}

	if (m_pLBSpecular)
	{
		m_pLBSpecular->Clear();
		delete m_pLBSpecular;
		m_pLBSpecular = 0;
	}

	if (m_pShader)
	{
		m_pShader->Clear();
		delete m_pShader;
		m_pShader = 0;
	}

	m_pRenderer = 0;
}

S_API void DeferredLightShaderPass::OnEndFrame()
{
	m_pRenderer->UnbindTexture(m_pLBDiffuse->GetTexture());
	m_pRenderer->UnbindTexture(m_pLBSpecular->GetTexture());
//	m_pRenderer->UnbindTexture(m_pLBDiffuse->GetDepthBufferTexture());

	m_pRenderer->ClearRT(m_pLBDiffuse, true, false);
	m_pRenderer->ClearRT(m_pLBSpecular, true, false);
}

S_API SResult DeferredLightShaderPass::Bind()
{
	m_pShader->Bind();
	m_pRenderer->EnableDepthTest(false, false);
	m_pRenderer->BindConstantsBuffer(m_Constants.GetCB());

	m_pRenderer->UnbindTexture(m_pLBDiffuse->GetTexture());
	m_pRenderer->UnbindTexture(m_pLBSpecular->GetTexture());

	m_pRenderer->BindRTCollection(m_RenderTargets, 0, true);

	int i = 0;
	while (i < NUM_GBUFFER_LAYERS)
	{
		ITexture* pGBufferTexture = m_pGBufferPass->GetGBufferTexture(i);
		m_pRenderer->BindTexture(pGBufferTexture, i++);
	}

	ITexture* pShadowmapTexture = m_pShadowmapPass->GetShadowmap()->GetDepthBufferTexture();
	m_pRenderer->BindTexture(pShadowmapTexture, i++);

	return S_SUCCESS;
}

S_API void DeferredLightShaderPass::SetLightConstants(const SLightObjectConstants& light)
{
	SLightObjectConstants* constants = m_Constants.GetConstants();
	memcpy(constants, &light, sizeof(SLightObjectConstants));

	// buffer will be updated in SetShaderResources()
}

S_API void DeferredLightShaderPass::SetShaderResources(const SShaderResources& pShaderResources, const Mat44& transform)
{
	// transform should be the transformation matrix of the light volume
	SLightObjectConstants* constants = m_Constants.GetConstants();
	constants->mtxWorld = transform;

	m_Constants.Update();
}



////////////////////////////////////////////////////////////////////////////////////////

//				Deferred Shading Shader Pass

////////////////////////////////////////////////////////////////////////////////////////

S_API SResult ShadingShaderPass::Initialize(IRenderer* pRenderer)
{
	m_pRenderer = pRenderer;

	ReloadShaders();

	m_Constants.Initialize(pRenderer);
	m_TerrainConstants.Initialize(pRenderer);

	return S_SUCCESS;
}

S_API void ShadingShaderPass::ReloadShaders()
{
	SShaderInfo si;

	if (m_pShader)
	{
		m_pShader->Clear();
		delete m_pShader;
	}

	si.filename = m_pRenderer->GetShaderPath(eSHADERFILE_ILLUM);
	si.entry = "DeferredShading";
	si.inputLayout = eSHADERINPUTLAYOUT_DEFAULT;

	m_pShader = m_pRenderer->CreateShader();
	m_pShader->Load(m_pRenderer, si);

	if (m_pTerrainShader)
	{
		m_pTerrainShader->Clear();
		delete m_pTerrainShader;
	}

	si.filename = m_pRenderer->GetShaderPath(eSHADERFILE_ILLUM);
	si.entry = "DeferredShadingTerrain";

	m_pTerrainShader = m_pRenderer->CreateShader();
	m_pTerrainShader->Load(m_pRenderer, si);
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

	m_pRenderer = 0;
}

S_API SResult ShadingShaderPass::Bind()
{
	m_pShader->Bind();
	m_pRenderer->BindConstantsBuffer(m_Constants.GetCB());

	// We render directly to the swap chain RT
	// TODO: Render to custom RT to apply post-effects later
	m_pRenderer->BindSingleRT(m_pRenderer->GetTargetViewport());

	int i = 0;
	while (i < NUM_GBUFFER_LAYERS)
	{
		ITexture* pGBufferTexture = m_pGBufferPass->GetGBufferTexture(i);
		m_pRenderer->BindTexture(pGBufferTexture, i++);
	}

	m_pRenderer->BindTexture(m_pLightPass->GetDiffuseLightBufferTexture(), i++);
	m_pRenderer->BindTexture(m_pLightPass->GetSpecularLightBufferTexture(), i++);

	return S_SUCCESS;
}

S_API void ShadingShaderPass::SetShaderResources(const SShaderResources& sr, const Mat44& transform)
{
	if (sr.illumModel == eILLUM_HELPER)
		return; // helpers cannot be rendered deferred

	m_pRenderer->EnableBackfaceCulling(sr.enableBackfaceCulling);

	// Bind textures
	m_pRenderer->BindTexture(IS_VALID_PTR(sr.textureMap) ? sr.textureMap : m_pRenderer->GetDummyTexture(), 4);
	
	// Update object constants
	SShadingShaderConstants* constants = m_Constants.GetConstants();
	constants->mtxWorld = transform;
	constants->matMetalness = sr.metalness;

	m_Constants.Update();
}

S_API void ShadingShaderPass::BindTerrainResources(const STerrainShaderResources& terrainShaderResources, bool constantsUpdated)
{
	m_pTerrainShader->Bind();

	m_pRenderer->BindVertexShaderTexture(terrainShaderResources.pHeightmap, 0);

	m_pRenderer->BindTexture(terrainShaderResources.pTexturemap, 4);
	m_pRenderer->BindTexture(terrainShaderResources.pLayerMask, 5);
	m_pRenderer->BindTexture(terrainShaderResources.pColormap, 6);

	if (constantsUpdated)
	{
		// Update terrain constants
		STerrainShadingShaderConstants* constants = m_TerrainConstants.GetConstants();
		if (constants)
		{
			STerrainConstants* terrainConstants = static_cast<STerrainConstants*>(constants);
			*terrainConstants = terrainShaderResources.constants;
		}

		m_TerrainConstants.Update();
	}

	m_pRenderer->BindConstantsBuffer(m_TerrainConstants.GetCB());
}





////////////////////////////////////////////////////////////////////////////////////////

//				Particle Shader Pass

////////////////////////////////////////////////////////////////////////////////////////

S_API SResult ParticleShaderPass::Initialize(IRenderer* pRenderer)
{
	m_pRenderer = pRenderer;

	ReloadShaders();

	m_Constants.Initialize(pRenderer);
	return S_SUCCESS;
}

S_API void ParticleShaderPass::ReloadShaders()
{
	if (m_pShader)
	{
		m_pShader->Clear();
		delete m_pShader;
	}

	SShaderInfo si;
	si.entry = "ParticleEmitter";
	si.filename = m_pRenderer->GetShaderPath(eSHADERFILE_PARTICLES);
	si.inputLayout = eSHADERINPUTLAYOUT_PARTICLES;

	m_pShader = m_pRenderer->CreateShader();
	m_pShader->Load(m_pRenderer, si);
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

	IFBO* pFBO = m_pRenderer->GetTargetViewport()->GetBackBuffer();
	m_pRenderer->BindSingleRT(pFBO, true);
	m_pRenderer->BindDepthBufferAsTexture(pFBO, 1);

	m_pRenderer->BindConstantsBuffer(m_Constants.GetCB());

	return S_SUCCESS;
}

S_API void ParticleShaderPass::OnUnbind()
{
	m_pRenderer->UnbindTexture(1);
}

S_API void ParticleShaderPass::SetShaderResources(const SShaderResources& shaderResources, const Mat44& transform)
{
	m_pRenderer->BindTexture(shaderResources.textureMap, 0);

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

	ReloadShaders();

	m_Constants.Initialize(pRenderer);
	return S_SUCCESS;
}

S_API void GUIShaderPass::ReloadShaders()
{
	if (m_pShader)
	{
		m_pShader->Clear();
		delete m_pShader;
	}

	SShaderInfo si;
	si.entry = "GUI";
	si.filename = m_pRenderer->GetShaderPath(eSHADERFILE_GUI);
	si.inputLayout = eSHADERINPUTLAYOUT_DEFAULT;

	m_pShader = m_pRenderer->CreateShader();
	m_pShader->Load(m_pRenderer, si);
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
	ReloadShaders();
	return S_SUCCESS;
}

S_API void PosteffectShaderPass::ReloadShaders()
{
}

S_API void PosteffectShaderPass::Clear()
{

}

S_API SResult PosteffectShaderPass::Bind()
{
	return S_SUCCESS;
}

S_API void PosteffectShaderPass::SetShaderResources(const SShaderResources& sr, const Mat44& transform)
{
}


SP_NMSPACE_END

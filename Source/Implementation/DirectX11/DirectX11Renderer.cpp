//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2011-2015, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include <Implementation\DirectX11\DirectX11Renderer.h>
//#include <Implementation\DirectX11\DirectX11RenderPipeline.h>
#include <Implementation\DirectX11\DirectX11ResourcePool.h>
#include <Abstract\IGameEngine.h>
#include <Implementation\DirectX11\DirectX11Utilities.h>
#include <Implementation\DirectX11\DirectX11VertexBuffer.h>
#include <Implementation\DirectX11\DirectX11IndexBuffer.h>
#include <Implementation\DirectX11\DirectX11Texture.h>
#include <Implementation\DirectX11\DirectX11FBO.h>
#include <Util\SVertex.h>
#include <xnamath.h>

#include <initguid.h>
#include <dxgi1_3.h>
#include <dxgidebug.h>

SP_NMSPACE_BEG

using ::std::vector;

//////////////////////////////////////////////////////////////////////////////////////////////////
// DirectX11RenderDeviceCaps

// --------------------------------------------------------------------
S_API DirectX11RenderDeviceCaps::DirectX11RenderDeviceCaps()
	: m_MaxMSQuality(1) // lowest quality
{
}

// --------------------------------------------------------------------
S_API SResult DirectX11RenderDeviceCaps::Collect(IRenderer* pRenderer)
{
	SP_ASSERTR(pRenderer && pRenderer->GetType() == S_DIRECTX11, S_INVALIDPARAM);


	// TODO



	//DirectX11Renderer* pDXRenderer = (DirectX11Renderer*)pRenderer;		

	return S_NOTIMPLEMENTED;		

	//pDXRenderer->GetD3D11Device()->CheckMultisampleQualityLevels()
}







//////////////////////////////////////////////////////////////////////////////////////////////////
// DirectX11Renderer

S_API DirectX11Renderer::DirectX11Renderer()
: m_pEngine(0),
m_pResourcePool(0),
m_pD3DDevice(0),
m_pD3DDeviceContext(0),
//m_pRenderPipeline(0),
m_pAutoSelectedAdapter(0),
m_pTargetViewport(0),
m_pDXGIFactory(0),
m_pDSV(0),
m_nRenderTargets(0),
m_pDepthStencilState(0),
m_pPerSceneCB(nullptr),
m_pPerObjectCB(nullptr),
m_pTerrainCB(nullptr),
m_bInScene(false),
m_bDumpFrame(false),
m_bPerObjectCBBound(false),
m_SetPrimitiveType(PRIMITIVE_TYPE_UNKNOWN)
{	
};

// --------------------------------------------------------------------
S_API DirectX11Renderer::~DirectX11Renderer()
{
	Shutdown();
}
// --------------------------------------------------------------------
S_API IResourcePool* DirectX11Renderer::GetResourcePool()
{
	if (m_pResourcePool == 0)
	{
		m_pResourcePool = new DirectX11ResourcePool();
		m_pResourcePool->Initialize(m_pEngine, this);
	}
	
	return m_pResourcePool;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::AutoSelectAdapter()
{
	HRESULT hRes = S_OK;

	SSettingsDesc::ApplicationSet& appSettings = m_pEngine->GetSettings()->Get().app;


	// Create and Validate the DXGI Factory
	CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&m_pDXGIFactory);	
	SP_ASSERTR(m_pDXGIFactory, S_ERROR);


	// Load and save all possible adapters
	IDXGIAdapter* pAdapter;
	usint32 iBestAdapter = 0;	
	DXGI_ADAPTER_DESC bestAdapterDesc;
	bestAdapterDesc.DedicatedVideoMemory = 0;
	for (usint32 iAdapter = 0; m_pDXGIFactory->EnumAdapters(iAdapter, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++iAdapter)
	{
		DXGI_ADAPTER_DESC adapterDesc;
		pAdapter->GetDesc(&adapterDesc);

		/*
		wchar_t adapterTxt[1024];
		memset(adapterTxt, 0, 1024);		
		swprintf_s(adapterTxt, L"%s\nsharedSysMem: %.3f MB\ndedicatedSysMem: %.3f MB\ndedicatedVidMem: %.3f MB",
			adapterDesc.Description,
			(float)adapterDesc.SharedSystemMemory / (1024.0f * 1024.0f),
			(float)adapterDesc.DedicatedSystemMemory / (1024.0f * 1024.0f),
			(float)adapterDesc.DedicatedVideoMemory / (1024.0f * 1024.0f));

		MessageBoxW(0, adapterTxt, L"Found adapter", MB_ICONINFORMATION | MB_OK);
		*/
		
		if (bestAdapterDesc.DedicatedVideoMemory < adapterDesc.DedicatedVideoMemory)
		{
			iBestAdapter = iAdapter;
			memcpy(&bestAdapterDesc, &adapterDesc, sizeof(DXGI_ADAPTER_DESC));
		}
		
		m_vAdapters.push_back(pAdapter);
	}

	SP_ASSERTR(m_vAdapters.size() > 0, S_ERROR, "No adapters found!");


	// In fullscreen mode:
	//	Find adapter with matching size.
	// In Windowed mode:
	//	Find best adapter measured by memory size.

	DXGI_FORMAT desiredBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	if (!appSettings.bWindowed)
	{
		m_bFullscreen = true;
		bool bFound = false;		
		IDXGIOutput* pOutput;
		for (auto iAdapter = m_vAdapters.begin(); iAdapter != m_vAdapters.end(); ++iAdapter)
		{
			if (!(pAdapter = *iAdapter))
				continue;

			for (usint32 iOutput = 0; pAdapter->EnumOutputs(iOutput, &pOutput) != DXGI_ERROR_NOT_FOUND; ++iOutput)
			{
				usint32 nModes;
				DXGI_MODE_DESC* pModes = 0;
				hRes = pOutput->GetDisplayModeList(desiredBackBufferFormat, 0, &nModes, 0);

				if (nModes == 0)
					continue; // no matching modes found in this output

				pModes = new DXGI_MODE_DESC[nModes];
				hRes = pOutput->GetDisplayModeList(desiredBackBufferFormat, 0, &nModes, pModes);

				for (usint32 iMode = 0; iMode < nModes; ++iMode)
				{
					if (pModes[iMode].Width == appSettings.nXResolution && pModes[iMode].Height == appSettings.nYResolution)
					{
						// okay, found matching mode, output and adapter. So store it now
						hRes = pAdapter->GetDesc(&m_AutoSelectedAdapterDesc);
						m_AutoSelectedDisplayModeDesc = pModes[iMode];
						m_pAutoSelectedAdapter = pAdapter;

						bFound = true;
						break;
					}
				}

				SP_SAFE_DELETE_ARR(pModes, nModes);
				SP_SAFE_RELEASE(pOutput);

				if (bFound)
					break;
			}

			if (bFound)
				break;
		}
		SP_ASSERTR(bFound, S_NOTFOUND, "No matching adapter or display mode found!");
	}
	else
	{
		m_AutoSelectedAdapterDesc = bestAdapterDesc;
		m_pAutoSelectedAdapter = m_vAdapters[iBestAdapter];				
		m_bFullscreen = false;

		m_AutoSelectedDisplayModeDesc.Format = desiredBackBufferFormat;
		m_AutoSelectedDisplayModeDesc.Width = appSettings.nXResolution;
		m_AutoSelectedDisplayModeDesc.Height = appSettings.nYResolution;
		
		// In windowed mode RefreshRate, Scaling and ScanlineOrdering are ignored.
		m_AutoSelectedDisplayModeDesc.RefreshRate.Denominator = 1;
		m_AutoSelectedDisplayModeDesc.RefreshRate.Numerator = 0;		
	}


	return S_SUCCESS;
}

// --------------------------------------------------------------------
SResult DirectX11Renderer::GetAutoSelectedDisplayModeDesc(SDisplayModeDescription* pDesc)
{
	if (pDesc)
	{
		SSettingsDesc::ApplicationSet& appSettings = m_pEngine->GetSettings()->Get().app;

		if (appSettings.bWindowed)
		{
			pDesc->width = appSettings.nXResolution;
			pDesc->height = appSettings.nYResolution;
			pDesc->refreshRate = 0;
		}
		else
		{
			pDesc->width = m_AutoSelectedDisplayModeDesc.Width;
			pDesc->height = m_AutoSelectedDisplayModeDesc.Height;
			pDesc->refreshRate = m_AutoSelectedDisplayModeDesc.RefreshRate.Numerator
				/ m_AutoSelectedDisplayModeDesc.RefreshRate.Denominator;
		}

		return pDesc->IsValid() ? S_SUCCESS : S_ERROR;
	}

	return S_INVALIDPARAM;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::SetRenderStateDefaults(void)
{
	SP_ASSERTR(IsInited(), S_NOTINIT);	


	// we probably need the settings of our engine
	SSettingsDesc::RenderSet& renderSettings = m_pEngine->GetSettings()->Get().render;



	// setup proper depth stencil testing
	// Setup the proper depth testing
	
	m_depthStencilDesc.DepthEnable = true; // we probably won't change this, so this if fixed
	m_depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	m_depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

	m_depthStencilDesc.StencilEnable = true;
	m_depthStencilDesc.StencilReadMask = 0xff;
	m_depthStencilDesc.StencilWriteMask = 0xff;

	m_depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	m_depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	m_depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	m_depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	m_depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	m_depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	m_depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	m_depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;	

	if (Failure(m_pD3DDevice->CreateDepthStencilState(&m_depthStencilDesc, &m_pDepthStencilState)))
		return m_pEngine->LogE("Failed create depth stencil state!");
#ifdef _DEBUG
	else
		m_pEngine->LogD("Created depth stencil state.");
#endif

	m_pD3DDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);
#ifdef _DEBUG
	m_pEngine->LogD("Set Depth Stencil state to default one.");
#endif




	// In DX11 we fist need a RSState interface	
	memset((void*)&m_rsDesc, 0, sizeof(D3D11_RASTERIZER_DESC));
	m_rsDesc.AntialiasedLineEnable = false;	// ???
	m_rsDesc.CullMode = D3D11_CULL_NONE;
	m_rsDesc.DepthBias = 0;
	m_rsDesc.DepthBiasClamp = 0;
	m_rsDesc.SlopeScaledDepthBias = 0;
	m_rsDesc.DepthClipEnable = false;
	m_rsDesc.FillMode = renderSettings.bRenderWireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;	
	m_rsDesc.FrontCounterClockwise = (renderSettings.frontFaceType == eFF_CW);
	m_rsDesc.MultisampleEnable = renderSettings.antiAliasingQuality != eAAQUALITY_LOW;
	m_rsDesc.ScissorEnable = FALSE; // maybe change this to true someday	

	HRESULT hRes;
	if ((hRes = m_pD3DDevice->CreateRasterizerState(&m_rsDesc, &m_pRSState)))
	{
		return m_pEngine->LogE("Faield create Rasterizer State!");
	}

	m_pD3DDeviceContext->RSSetState(m_pRSState);


	// Setup default Sampler State
	// NOTE: Currently using same default sampler state for Texturemap and Normalmap!
	D3D11_SAMPLER_DESC defSamplerDesc;	
	defSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	defSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;	
	defSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;	
	defSamplerDesc.MinLOD = -FLT_MAX;
	defSamplerDesc.MaxLOD = FLT_MAX;
	defSamplerDesc.MipLODBias = 0.0f;
	defSamplerDesc.MaxAnisotropy = 1;	
	defSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	defSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	
	defSamplerDesc.BorderColor[0] = 0;
	defSamplerDesc.BorderColor[1] = 0;
	defSamplerDesc.BorderColor[2] = 0;
	defSamplerDesc.BorderColor[3] = 0;

	if (Failure(m_pD3DDevice->CreateSamplerState(&defSamplerDesc, &m_pDefaultSamplerState)))
		return m_pEngine->LogE("Failed create default Sampler State!");

	m_pD3DDeviceContext->PSSetSamplers(0, 1, &m_pDefaultSamplerState);
	m_pD3DDeviceContext->PSSetSamplers(1, 1, &m_pDefaultSamplerState);

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::InitDefaultViewport(HWND hWnd, int nW, int nH)
{
	SP_ASSERTR(m_pEngine && m_pD3DDevice && m_pD3DDeviceContext && m_pDXGIFactory, S_NOTINIT);	
	SP_ASSERTR(hWnd, S_INVALIDPARAM);

	SViewportDescription vpDesc;
	
	vpDesc.projectionDesc.fov = 60;
	vpDesc.projectionDesc.projectionType = S_PROJECTION_PERSPECTIVE;
	vpDesc.projectionDesc.bUseEngineZPlanes = true;


	vpDesc.width = nW;
	vpDesc.height = nH;
	vpDesc.useDepthStencil = true;	// we definetly want a depthstencil!
	vpDesc.hWnd = hWnd;	

	if (Failure(m_Viewport.Initialize(m_pEngine, vpDesc, false)))
		return S_ERROR;	

	// m_pD3DDeviceContext->RSSetViewports() is called in the SetTargetViewport after the call of this function.


	// okay. done
	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::CreateDX11Device()
{
	SP_ASSERTR(m_pEngine && m_pAutoSelectedAdapter, S_NOTINIT);

	// Setup creation flags
	usint32 createFlags = 0;	
	if (!m_pEngine->GetSettings()->Get().app.bMultithreaded) createFlags |= D3D11_CREATE_DEVICE_SINGLETHREADED;	
#ifdef _DEBUG
	createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// To be sure we only use correct feature levels in our dx11 device, we have to explicitly set them	
	// !!! Notice: To optionally support DX11.1, you have to add the missing D3D_FEATURE_LEVEL_11_1 !!!
	D3D_FEATURE_LEVEL pD3D11FeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,

		// D3D9 features used to support old-hardware compatibility
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};
	usint32 nFeatureLevels = 6;

	// Now create the device with the highest specs
	HRESULT hRes;
	hRes = D3D11CreateDevice(
		m_pAutoSelectedAdapter,
		D3D_DRIVER_TYPE_UNKNOWN,	// DX requires that, because we have a non-null existing adapter
		0,
		createFlags,
		pD3D11FeatureLevels, nFeatureLevels, D3D11_SDK_VERSION,
		&m_pD3DDevice, &m_D3DFeatureLevel, &m_pD3DDeviceContext);	// consider using deferred context for multithreading!
	
#ifdef _DEBUG
	const char nm[] = "SPD3D11Device";
	m_pD3DDevice->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(nm) - 1, nm);	
#endif

	//delete[] pD3D11FeatureLevels;
	//SP_SAFE_DELETE_ARR(pD3D11FeatureLevels, nFeatureLevels);

	// If everything failed, then don't go further
	if (Failure(hRes) || !m_pD3DDevice || !m_pD3DDeviceContext)
	{
		return m_pEngine->LogE("Failed create D3D11 Device!");
	}





	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::D3D11_CreateSwapChain(DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain)
{
	SP_ASSERTR(ppSwapChain && pDesc, S_INVALIDPARAM);
	SP_ASSERTR(m_pDXGIFactory, S_NOTINIT);

	m_pDXGIFactory->CreateSwapChain(m_pD3DDevice, pDesc, ppSwapChain);

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::D3D11_CreateRTV(ID3D11Resource* pBBResource, D3D11_RENDER_TARGET_VIEW_DESC* pDesc, ID3D11RenderTargetView** ppRTV)
{
	SP_ASSERTR(pBBResource && ppRTV, S_INVALIDPARAM);
	SP_ASSERTR(m_pD3DDevice, S_NOTINIT);

	if (Failure(m_pD3DDevice->CreateRenderTargetView(pBBResource, pDesc, ppRTV)))
		return S_ERROR;
	else
		return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API S_RENDERER_TYPE DirectX11Renderer::GetType(void)
{
	return S_DIRECTX11;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::Initialize(IGameEngine* pEngine, bool bIgnoreAdapter)
{		
	SP_ASSERTR(pEngine, S_INVALIDPARAM);	

	if (IsInited())
		Shutdown();	// shutdown before initializing again

	m_pEngine = pEngine;		

	// Auto Select Adapter and display mode
	if (Failure(AutoSelectAdapter()))
	{
		m_pEngine->LogE("Failed automatic selection of Video adapter and display mode. Shut down.");
		Shutdown();
		return S_ERROR;
	}
	// Now start the actual Interface to the initialized DirectX Device.
	// Will fail if neither hardware nor mixed nor software processing is working.
	if (Failure(CreateDX11Device()))
	{
		Shutdown();
		return pEngine->LogE("Failed Create DX11 Device!");
	}		
	// Set Default RenderStates and Texture Sampler States
	if (Failure(SetRenderStateDefaults()))
	{
		return pEngine->LogReport(S_ERROR, "Failed set render state defaults!");
	}





	// Initialize the default Viewport
	// This viewport is forced to be generated. Client is only able to add more SwapChains
	// !! Do NOT put this call before CreateDX11Device(). InitDefaultViewport depends on an existing device in DX11 !!	
	SSettingsDesc::ApplicationSet& applicationSettings = pEngine->GetSettings()->Get().app;
	
	if (Failure(InitDefaultViewport(applicationSettings.hWnd, applicationSettings.nXResolution, applicationSettings.nYResolution)))
	{
		Shutdown();
		return m_pEngine->LogE("Failed Init default viewport!");
	}		





	// initialize the render target collections
	// WARNING: Needs to be done before initialization of the render pipeline, beacuse the sections
	// might add FBO collections in order to initialize!	
	ZeroMemory(m_pRenderTargets, sizeof(DirectX11FBO*) * MAX_BOUND_RTS);

	if (Failure(SetTargetViewport((IViewport*)&m_Viewport)))
	{
		return pEngine->LogReport(S_ERROR, "Failed set Target Viewport!");
	}


	// Set the base primitive topology
	m_pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// Initialize the matrix Constants buffer
	if (Failure(InitConstantBuffers()))
		return S_ERROR;	
	
	// Do not render terrain immediately
	m_TerrainRenderDesc.bRender = false;


	// initialize the shaders
	char* pForwardFXFile = 0, *pTerrainFXFile = 0;
#ifdef _DEBUG
	pForwardFXFile = new char[500];
	pTerrainFXFile = new char[500];
	sprintf_s(pForwardFXFile, 500, "%s..\\Effects\\forward.fx", SOL_DIR);
	sprintf_s(pTerrainFXFile, 500, "%s..\\Effects\\terrain.fx", SOL_DIR);
#else
	pGBufferFXFile = "Effects\\forward.fx";
	pTerrainFXFile = "Effects\\terrain.fx";
#endif

	if (Failure(m_ForwardEffect.Initialize(m_pEngine, pForwardFXFile, "forward")))
		return S_ERROR;

	if (Failure(m_TerrainEffect.Initialize(m_pEngine, pTerrainFXFile, "terrain")))
		return S_ERROR;

#ifdef _DEBUG
	delete[] pForwardFXFile;
	delete[] pTerrainFXFile;
#endif

	// Create unset texture dummy
	m_DummyTexture.Initialize(m_pEngine, "notexture", false);
	if (Failure(m_DummyTexture.CreateEmpty(64, 64, 0, eTEXTURE_R8G8B8A8_UNORM, SColor(0.0f, 1.0f, 0.0f))))
		m_pEngine->LogE("Could not create empty dummy texture (notexture)!");	



	// Create (128,128,0) replacement normal map
	m_DummyNormalMap.Initialize(m_pEngine, "nonormalmap", false);
	if (Failure(m_DummyNormalMap.CreateEmpty(64, 64, 0, eTEXTURE_R8G8B8A8_UNORM, SColor(0.5f, 0.5f, 1.0f))))
		m_pEngine->LogE("Could not create empty dummy normal map (nonormalmap)!");


	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API bool DirectX11Renderer::IsInited(void)
{
	return (m_pDXGIFactory && m_pD3DDevice && m_pD3DDeviceContext);
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::Shutdown(void)
{
	m_DummyTexture.Clear();

	if (IS_VALID_PTR(m_pResourcePool))
	{
		m_pResourcePool->ClearAll();
		delete m_pResourcePool;		
	}
	m_pResourcePool = nullptr;

	SP_SAFE_RELEASE(m_pDXGIFactory);
	SP_SAFE_RELEASE_CLEAR_VECTOR(m_vAdapters);	

	m_TerrainEffect.Clear();
	m_ForwardEffect.Clear();	
	m_Viewport.Clear();

	SP_SAFE_RELEASE(m_pDefaultSamplerState);
	SP_SAFE_RELEASE(m_pDepthStencilState);
	SP_SAFE_RELEASE(m_pRSState);
	SP_SAFE_RELEASE(m_pPerSceneCB);
	SP_SAFE_RELEASE(m_pPerObjectCB);
	SP_SAFE_RELEASE(m_pTerrainCB);
	if (IS_VALID_PTR(m_pD3DDeviceContext))
	{		
		m_pD3DDeviceContext->ClearState();
		m_pD3DDeviceContext->Flush();
	}
			

	SP_SAFE_RELEASE(m_pD3DDevice);	
	SP_SAFE_RELEASE(m_pD3DDeviceContext);	

#ifdef _DEBUG	
	// Print live objects to detect memory leaks
	IDXGIDebug1* pDXGIDebug;
	DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), reinterpret_cast<void**>(&pDXGIDebug));
	pDXGIDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);	
#endif		

	m_RenderSchedule.Clear();

	return S_SUCCESS;
}

// --------------------------------------------------------------------
/*
S_API SResult DirectX11Renderer::AddRTCollectionFBO(usint32 index, IFBO* pFBO)
{
	SP_ASSERTR(m_pD3DDevice && m_pRenderTargetCollections && m_CurrentRenderTargetCollection.pvRenderTargets, S_NOTINIT);
	SP_ASSERTR(pFBO && pFBO->IsInitialized(), S_INVALIDPARAM);	

	// check if this FBO is not already in the current collection
	for (auto iFBO = m_CurrentRenderTargetCollection.pvRenderTargets->begin();
		iFBO != m_CurrentRenderTargetCollection.pvRenderTargets->end();
		++iFBO)
	{
		if (iFBO->pFBO == pFBO && iFBO->iIndex == index)
			return S_SUCCESS;
	}

	m_CurrentRenderTargetCollection.pvRenderTargets->push_back(SRenderTarget(pFBO, index));

	return S_SUCCESS;
}
*/
// --------------------------------------------------------------------
/*
S_API SResult DirectX11Renderer::StoreRTCollection(ERenderTargetCollectionID asId)
{
	SP_ASSERTR(m_pRenderTargetCollections, S_NOTINIT);

	// check if already there
	auto foundRTCollection = m_pRenderTargetCollections->find(asId);
	if (foundRTCollection != m_pRenderTargetCollections->end())
		m_pRenderTargetCollections->erase(foundRTCollection);

	// add to map
	m_pRenderTargetCollections->insert(std::pair<ERenderTargetCollectionID, SRenderTargetCollection>(asId, m_CurrentRenderTargetCollection));

	return S_SUCCESS;
}
*/

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::BindRTCollection(std::vector<IFBO*>& fboCollection, IFBO* depthFBO, const char* dump_name /*= 0*/)
{
	SP_ASSERTR(m_pD3DDevice, S_NOTINIT);

	if (fboCollection.size() == 0)
		return S_INVALIDPARAM;

	// check depth fbo and get DSV
	ID3D11DepthStencilView* pDSV = 0;
	if (IS_VALID_PTR(depthFBO))
	{
		DirectX11FBO* pDXDepthFBO = dynamic_cast<DirectX11FBO*>(depthFBO);
		if (IS_VALID_PTR(pDXDepthFBO))	
			pDSV = pDXDepthFBO->GetDSV();
	}	

	// Bind all FBOs
	ID3D11RenderTargetView** pRTVs = new ID3D11RenderTargetView*[fboCollection.size()];
	ZeroMemory(pRTVs, sizeof(ID3D11RenderTargetView*) * fboCollection.size());
	unsigned int nRTVCounter = 0;
	for (auto itFBO = fboCollection.begin(); itFBO != fboCollection.end(); itFBO++)
	{
		DirectX11FBO* pDXFBO = dynamic_cast<DirectX11FBO*>(*itFBO);
		if (!IS_VALID_PTR(pDXFBO))
			continue;

		m_pRenderTargets[nRTVCounter] = pDXFBO;

		pRTVs[itFBO - fboCollection.begin()] = pDXFBO->GetRTV();
		nRTVCounter++;
	}

	m_nRenderTargets = nRTVCounter;

	// Bind the rtvs
	m_pD3DDeviceContext->OMSetRenderTargets(nRTVCounter, pRTVs, pDSV);
	m_pDSV = pDSV;

	FrameDump("Bound RT Collection " + SString(dump_name ? dump_name : "") + " (" + SString::FromInteger(nRTVCounter) + " RTs)!");

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::BindSingleRT(IFBO* pFBO)
{
	SP_ASSERTR(IsInited(), S_NOTINIT);
	SP_ASSERTR(IS_VALID_PTR(pFBO), S_INVALIDPARAM);	

	/*
	SetViewportMatrices(m_pTargetViewport);
	UpdateConstantBuffer(CONSTANTBUFFER_PERSCENE);
	*/

	// check if already bound
	if (!BoundMultipleRTs() && GetBoundSingleRT() == pFBO)
		return S_SUCCESS;

	DirectX11FBO* pSPDXFBO = dynamic_cast<DirectX11FBO*>(pFBO);
	if (!IS_VALID_PTR(pSPDXFBO))
		return S_INVALIDPARAM;

	if (BoundMultipleRTs())
	{
		for (unsigned int i = 0; i < m_nRenderTargets; ++i)
			m_pRenderTargets[i] = 0;
	}

	m_nRenderTargets = 1;
	m_pRenderTargets[0] = pSPDXFBO;

	ID3D11RenderTargetView* pRTV = pSPDXFBO->GetRTV();
	ID3D11DepthStencilView* pDSV = pSPDXFBO->GetDSV();

	m_pD3DDeviceContext->OMSetRenderTargets(1, &pRTV, pDSV);
	m_pDSV = pDSV;

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::BindSingleRT(IViewport* pViewport)
{
	IFBO* pBackBuffer = 0;
	SP_ASSERTR(pViewport && (pBackBuffer = pViewport->GetBackBuffer()), S_INVALIDPARAM);
	SP_ASSERTR(m_pD3DDevice && m_pEngine && m_pD3DDeviceContext, S_NOTINIT);

	if (!pBackBuffer->IsInitialized())
		return S_INVALIDPARAM;

	// updates View matrices using target buffer matrices
	if (Failure(BindSingleRT(pBackBuffer)))
		return S_ERROR;

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API IFBO* DirectX11Renderer::GetBoundSingleRT()
{
	if (BoundMultipleRTs())
		FrameDump("Warning: Tried query Bound Single RT, but multiple RTs bound!");

	return (IFBO*)m_pRenderTargets[0];
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::SetVBStream(IVertexBuffer* pVB, unsigned int index)
{
	SP_ASSERTR(IsInited(), S_NOTINIT);
	SP_ASSERTR(pVB, S_INVALIDPARAM);

	ID3D11Buffer* pDXVB = ((DirectX11VertexBuffer*)pVB)->D3D11_GetBuffer();
	usint32 stride = sizeof(SVertex);
	usint32 offset = 0;

	m_pD3DDeviceContext->IASetVertexBuffers(index, 1, &pDXVB, &stride, &offset);

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::SetIBStream(IIndexBuffer* pIB)
{
	SP_ASSERTR(IsInited(), S_NOTINIT);

	if (IS_VALID_PTR(pIB))
	{
		//S_INDEXBUFFER_FORMAT ibFmt = pIB->GetFormat();
		DXGI_FORMAT ibFmtDX = DXGI_FORMAT_R16_UINT;
		/*switch (ibFmt)
		{
		case S_INDEXBUFFER_16: ibFmtDX = DXGI_FORMAT_R16_UINT; break;
		case S_INDEXBUFFER_32: ibFmtDX = DXGI_FORMAT_R32_UINT; break;
		default:
		return S_ERROR;
		}*/

		m_pD3DDeviceContext->IASetIndexBuffer(((DirectX11IndexBuffer*)pIB)->D3D11_GetBuffer(), ibFmtDX, 0);
	}
	else
	{		
		m_pD3DDeviceContext->IASetIndexBuffer(0, DXGI_FORMAT_UNKNOWN, 0);
	}

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::BindTexture(ITexture* pTex, usint32 lvl /*=0*/)
{
	SP_ASSERTR(IsInited(), S_NOTINIT);
	SP_ASSERTR(IS_VALID_PTR(pTex), S_INVALIDPARAM);

	ID3D11ShaderResourceView* pSRV = (pTex) ? ((DirectX11Texture*)pTex)->D3D11_GetSRV() : 0;
	SP_ASSERTRD(IS_VALID_PTR(pSRV), S_ERROR, "SRV of Texture not set or invalid!");

	m_pD3DDeviceContext->PSSetShaderResources(lvl, 1, &pSRV);

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::BindTexture(IFBO* pFBO, usint32 lvl)
{
	SP_ASSERTR(IsInited(), S_NOTINIT);
	SP_ASSERTR(pFBO, S_INVALIDPARAM);

	DirectX11FBO* pDXFBO = dynamic_cast<DirectX11FBO*>(pFBO);	
	SP_ASSERTR(pDXFBO, S_INVALIDPARAM);

	ID3D11ShaderResourceView* pSRV = pDXFBO->GetSRV();
	SP_ASSERTR(pSRV, S_ERROR);

	m_pD3DDeviceContext->PSSetShaderResources(lvl, 1, &pSRV);

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::CreateAdditionalViewport(IViewport** pViewport, const SViewportDescription& desc)
{
	SP_ASSERTR(m_pEngine && m_pD3DDevice && m_pD3DDeviceContext && m_pDXGIFactory, S_NOTINIT);
	SP_ASSERTR(pViewport, S_INVALIDPARAM);		


	// instanciate the viewport first and initialize it
	DirectX11Viewport* pDXViewport = (DirectX11Viewport*)(*pViewport);
	pDXViewport = new DirectX11Viewport();
	pDXViewport->Initialize(m_pEngine, desc, true);

	SSettingsDesc& engineSet = m_pEngine->GetSettings()->Get();




	// Initialize the depth-stencil view for this viewport
	if (Failure(pDXViewport->InitializeDepthStencilBuffer()))
	{
		return S_ERROR;
	}



	// okay. done.

	return S_SUCCESS;
}

















// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::BeginScene(void)
{
	if (m_bInScene)
		return S_ERROR;

	if (m_bDumpFrame)
		m_pEngine->LogD("Beginning rendering of scene... (Begin Frame)");	
	
	m_bInScene = true;	

	return S_SUCCESS;
}


// --------------------------------------------------------------------
/*
S_API SResult DirectX11Renderer::RenderGeometry(const SRenderDesc& dsc)
{
	if (!m_bInScene)
		return S_INVALIDSTAGE;	

	SRenderSlot* pSlot = m_RenderSchedule.Get();
	pSlot->keep = false;
	pSlot->renderDesc = dsc;	

	if (IS_VALID_PTR(dsc.material.textureMap))
		dsc.material.textureMap->GetType();

	return S_SUCCESS;
}
*/

// --------------------------------------------------------------------
/*
S_API SResult DirectX11Renderer::RenderTerrain(const STerrainRenderDesc& tdsc)
{
	if (!m_bInScene)
		return EngLog(S_INVALIDSTAGE, m_pEngine, "Failed DirectX11Renderer::RenderTerrain(): Scene rendering not started!");

	memcpy(&m_TerrainRenderDesc, &tdsc, sizeof(STerrainRenderDesc));
	FrameDump("Set terrain Render Desc.");

	return S_SUCCESS;
}
*/

// --------------------------------------------------------------------
S_API SRenderSlot* DirectX11Renderer::GetRenderSlot()
{
	if (!m_bInScene)
		return 0;

	return m_RenderSchedule.Get();
}

// --------------------------------------------------------------------
S_API void DirectX11Renderer::ReleaseRenderSlot(SRenderSlot** pSlot)
{
	m_RenderSchedule.Release(pSlot);
}

// --------------------------------------------------------------------
S_API STerrainRenderDesc* DirectX11Renderer::GetTerrainRenderDesc()
{
	return &m_TerrainRenderDesc;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::EndScene(void)
{
	if (!m_bInScene)
		return S_ERROR;

	m_bInScene = false;

	RETURN_ON_ERR(UnleashRenderSchedule());
	RETURN_ON_ERR(PresentTargetViewport());

	m_Settings.SetClearColor(SColor(.0f, 0.05f, .0f));
	RETURN_ON_ERR(BindSingleRT(m_pTargetViewport));
	RETURN_ON_ERR(ClearBoundRTs());
	
	m_bDumpFrame = false;
	return S_SUCCESS;
}








/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

//		UNLEASH RENDER SCHEDULE

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


S_API SResult DirectX11Renderer::UnleashRenderSchedule()
{
	if (m_bInScene)
	{
		FrameDump("Cannot Unleash Render SChedule: Not in scene!");
		return S_INVALIDSTAGE;
	}

	//ERenderPipeline previousTechnique = eRENDER_FORWARD;
	//RETURN_ON_ERR(BindSingleRT(m_pTargetViewport));




	// TODO: Decide whether extra terrain render desc is good.




	// Render Terrain
	if (m_TerrainRenderDesc.bRender)
	{
		bool bTerrainRenderState = true;	// true = success
		FrameDump("Rendering Terrain...");

		// Render terrain directly to backbuffer
		BindSingleRT(m_pTargetViewport);

		if (!(bTerrainRenderState = IS_VALID_PTR(m_TerrainRenderDesc.pDetailMap))) m_pEngine->LogE("Invalid detail map in Terrain render Desc!");
		if (!(bTerrainRenderState = IS_VALID_PTR(m_TerrainRenderDesc.pColorMap))) m_pEngine->LogE("Invalid color map in Terrin render Desc!");

		BindTexture(m_TerrainRenderDesc.pColorMap, 0);
		BindTexture(m_TerrainRenderDesc.pDetailMap, 1);

		m_pD3DDeviceContext->PSSetConstantBuffers(1, 0, nullptr);
		m_pD3DDeviceContext->VSSetConstantBuffers(1, 0, nullptr);

		if (m_TerrainRenderDesc.bUpdateCB)
			UpdateConstantBuffer(CONSTANTBUFFER_TERRAIN, &m_TerrainRenderDesc.constants);

		// bind terrain cb
		m_pD3DDeviceContext->PSSetConstantBuffers(1, 1, &m_pTerrainCB);
		m_pD3DDeviceContext->VSSetConstantBuffers(1, 1, &m_pTerrainCB);
		m_bPerObjectCBBound = false;

		// TODO: Draw Section based different lod levels

		// binds terrain cb
		DrawTerrain(m_TerrainRenderDesc.drawCallDesc);
		m_TerrainRenderDesc.bRender = false;
	}
	else
	{
		FrameDump("Terrain not scheduled to be rendered this frame.");
	}	





	if (m_RenderSchedule.GetUsedObjectCount() == 0)
	{
		FrameDump("Render schedule empty in UnleashRenderSchedule()");
		return S_SUCCESS;
	}

	FrameDump("Unleashing render schedule now...");
	FrameDump((unsigned int)m_RenderSchedule.GetUsedObjectCount(), SString("RenderScheduleSize"));

	unsigned int iRSIterator = 0;
	for (unsigned int iSlot = 0; iSlot < m_RenderSchedule.GetUsedObjectCount(); ++iSlot)
	{
		SRenderSlot* pSlot;
		if (iSlot >= 49)
			pSlot = m_RenderSchedule.GetNextUsedObject(iRSIterator);
		else
			pSlot = m_RenderSchedule.GetNextUsedObject(iRSIterator);
		
		if (pSlot == 0)
			break; // End of Schedule reached

		SRenderDesc* pDesc = &pSlot->renderDesc;		

		// Skip render slot without subsets
		if (pDesc->nSubsets == 0 || !IS_VALID_PTR(pDesc->pSubsets))
			continue;

		if (pDesc->renderPipeline == eRENDER_FORWARD)
		{			
			// Forward rendering: Rendering directly to backbuffer
			if (Failure(BindSingleRT(m_pTargetViewport)))
				return S_ERROR;

			
			// Set the viewport matrices
			SetViewportMatrices(m_pTargetViewport);
			UpdateConstantBuffer(CONSTANTBUFFER_PERSCENE);


			// Set the transformation matrices
			SetWorldMatrix(pDesc->transform);
			if (Failure(UpdateConstantBuffer(CONSTANTBUFFER_PEROBJECT)))
				return S_ERROR;


			// Make sure to bind the Object CB, which is not bound if
			// terrain has been rendered before.
			if (!m_bPerObjectCBBound)
			{
				m_pD3DDeviceContext->PSSetConstantBuffers(1, 1, &m_pPerObjectCB);
				m_pD3DDeviceContext->VSSetConstantBuffers(1, 1, &m_pPerObjectCB);
				m_bPerObjectCBBound = true;
			}



			// NOW RENDER THE SUBSETS
			DrawForwardSubsets(*pDesc);


		}
		else if (pDesc->renderPipeline == eRENDER_DEFERRED)
		{
			// first, render to gbuffer here. after the loop we'll then render lights and merge into backbuffer
			// TODO
		}		


		// Remove RenderSlot item if it should not be kept anymore
		if (!pSlot->keep)
		{
			m_RenderSchedule.Release(&pSlot);
			iRSIterator--;
		}

	}	

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::DrawForwardSubsets(const SRenderDesc& renderDesc)
{
	for (unsigned int iSubset = 0; iSubset < renderDesc.nSubsets; ++iSubset)
	{
		SRenderSubset& subset = renderDesc.pSubsets[iSubset];
	
		if (!subset.render)
			continue;

		// Set material
		ITexture* pTextureMap = IS_VALID_PTR(subset.material.textureMap) ? subset.material.textureMap : (ITexture*)&m_DummyTexture;
		ITexture* pNormalMap = IS_VALID_PTR(subset.material.normalMap) ? subset.material.normalMap : (ITexture*)&m_DummyNormalMap;
		
		BindTexture(pTextureMap, 0);
		BindTexture(pNormalMap, 1);


		DrawForward(subset.drawCallDesc);
	}

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::DrawForward(const SDrawCallDesc& desc)
{	
	// bind vertex data stream
	if (Failure(SetVBStream(desc.pVertexBuffer)))
		return S_ERROR;	

	// Make sure the correct primitive type is set, but don't change too often
	if (desc.primitiveType == PRIMITIVE_TYPE_LINES)
	{
		if (Failure(SetIBStream(0)))
			return S_ERROR;

		if (Failure(m_ForwardEffect.Enable()))
			return S_ERROR;

		if (m_SetPrimitiveType != PRIMITIVE_TYPE_LINES)
		{
			m_pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
			m_SetPrimitiveType = PRIMITIVE_TYPE_LINES;
		}

		m_pD3DDeviceContext->Draw(desc.iEndVBIndex - desc.iStartVBIndex + 1, desc.iStartVBIndex);
	}
	else
	{
		if (Failure(SetIBStream(desc.pIndexBuffer)))
			return S_ERROR;

		if (Failure(m_ForwardEffect.Enable()))
			return S_ERROR;

		if (m_SetPrimitiveType != PRIMITIVE_TYPE_TRIANGLELIST)
		{
			m_pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_SetPrimitiveType = PRIMITIVE_TYPE_TRIANGLELIST;
		}

		m_pD3DDeviceContext->DrawIndexed(desc.iEndIBIndex - desc.iStartIBIndex + 1, desc.iStartIBIndex, desc.iStartVBIndex);
	}

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::DrawTerrain(const SDrawCallDesc& dcd)
{
	if (Failure(SetVBStream(dcd.pVertexBuffer))) return S_ERROR;
	if (Failure(SetIBStream(dcd.pIndexBuffer))) return S_ERROR;

	if (Failure(m_TerrainEffect.Enable()))
		return m_pEngine->LogE("Enabling terrain effect failed");

	if (m_SetPrimitiveType != PRIMITIVE_TYPE_TRIANGLELIST)
	{
		m_pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_SetPrimitiveType = PRIMITIVE_TYPE_TRIANGLELIST;
	}

	m_pD3DDeviceContext->DrawIndexed(dcd.iEndIBIndex - dcd.iStartIBIndex + 1, dcd.iStartIBIndex, dcd.iStartVBIndex);

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::DrawDeferred(const SDrawCallDesc& desc)
{
	return S_ERROR; // not supported yet
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::DrawDeferredLighting()
{
	return S_ERROR; // not supported yet
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::MergeDeferred()
{
	return S_ERROR; // not supported yet
}



















// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::PresentTargetViewport(void)
{
	SP_ASSERTR(IsInited(), S_NOTINIT);

	DirectX11Viewport* pDXTargetViewport = (DirectX11Viewport*)m_pTargetViewport;
	SP_ASSERTR(pDXTargetViewport, S_ERROR);

	IDXGISwapChain* pSwapChain = *pDXTargetViewport->GetSwapChainPtr();
	
	unsigned int syncInterval = 0;
	SSettingsDesc& engSettingsDesc = m_pEngine->GetSettings()->Get();
	if (engSettingsDesc.render.bEnableVSync)
		syncInterval = engSettingsDesc.render.vsyncInterval;

	if (Failure(pSwapChain->Present(syncInterval, 0)))
		return S_ERROR;

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::ClearBoundRTs(void)
{
	SP_ASSERTR(IsInited(), S_NOTINIT);

	// Clear RTs
	for (unsigned int i = 0; i < m_nRenderTargets; ++i)
	{
		if (!IS_VALID_PTR(m_pRenderTargets[i]))
			continue;

		ID3D11RenderTargetView* pRTV = m_pRenderTargets[i]->GetRTV();
		if (!IS_VALID_PTR(pRTV))
			continue;

		m_pD3DDeviceContext->ClearRenderTargetView(pRTV, m_Settings.GetClearColorFloatArr());
	}

	// Clear Depth Buffer
	if (IS_VALID_PTR(m_pDSV))
		m_pD3DDeviceContext->ClearDepthStencilView(m_pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);	

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::SetTargetViewport(IViewport* pViewport)
{
	SP_ASSERTR(pViewport, S_INVALIDPARAM);

	m_pTargetViewport = pViewport;
	SetViewportMatrices(pViewport);
	if (m_pD3DDeviceContext)
	{
		DirectX11Viewport* pDXViewport = dynamic_cast<DirectX11Viewport*>(pViewport);
		if (!IS_VALID_PTR(pDXViewport))
			return S_INVALIDPARAM;

		D3D11_VIEWPORT* vp = pDXViewport->GetViewportDescPtr();
		
		m_pD3DDeviceContext->RSSetViewports(1, pDXViewport->GetViewportDescPtr());	
	}

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API IViewport* DirectX11Renderer::GetTargetViewport(void)
{
	return m_pTargetViewport;
}

// --------------------------------------------------------------------
S_API IViewport* DirectX11Renderer::GetDefaultViewport(void)
{
	return (IViewport*)&m_Viewport;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::InitConstantBuffers()
{
	// Per-Scene CB
	if (Failure(D3D11_CreateConstantsBuffer(&m_pPerSceneCB, sizeof(SPerSceneConstantBuffer))))
		return S_ERROR;

	m_pD3DDeviceContext->VSSetConstantBuffers(0, 1, &m_pPerSceneCB);
	m_pD3DDeviceContext->PSSetConstantBuffers(0, 1, &m_pPerSceneCB);
	m_pEngine->LogD("Created and set per scene CB");



	// Per-Object CB
	if (Failure(D3D11_CreateConstantsBuffer(&m_pPerObjectCB, sizeof(SPerObjectConstantBuffer))))
		return S_ERROR;
	
	m_pD3DDeviceContext->VSSetConstantBuffers(1, 1, &m_pPerObjectCB);
	m_pD3DDeviceContext->PSSetConstantBuffers(1, 1, &m_pPerObjectCB);
	m_bPerObjectCBBound = true;
	m_pEngine->LogD("Created and set per object CB");	



	// Terrain CB
	if (Failure(D3D11_CreateConstantsBuffer(&m_pTerrainCB, sizeof(STerrainConstantBuffer))))
		return m_pEngine->LogE("Failed create terrain CB");



	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::UpdateConstantBuffer(EConstantBufferType cb, const STerrainConstantBuffer* pTerrainCB)
{
	switch (cb)
	{
	case CONSTANTBUFFER_PERSCENE:
		SPerSceneConstantBuffer* pSceneBuffer;
		if (Failure(D3D11_LockConstantsBuffer(m_pPerSceneCB, (void**)&pSceneBuffer)))
			return S_ERROR;

		assert(IS_VALID_PTR(pSceneBuffer));
		memcpy((void*)pSceneBuffer, (void*)&m_PerSceneCB, sizeof(SPerSceneConstantBuffer));
		D3D11_UnlockConstantsBuffer(m_pPerSceneCB);
		break;


	case CONSTANTBUFFER_PEROBJECT:
		SPerObjectConstantBuffer* pObjectBuffer;
		if (Failure(D3D11_LockConstantsBuffer(m_pPerObjectCB, (void**)&pObjectBuffer)))
			return S_ERROR;

		assert(IS_VALID_PTR(pObjectBuffer));
		*pObjectBuffer = m_PerObjectCB;
		D3D11_UnlockConstantsBuffer(m_pPerObjectCB);
		break;


	case CONSTANTBUFFER_TERRAIN:
		assert(IS_VALID_PTR(pTerrainCB));
		STerrainConstantBuffer* pTerrainBuffer;		
		if (Failure(D3D11_LockConstantsBuffer(m_pTerrainCB, (void**)&pTerrainBuffer)))
			return S_ERROR;		

		assert(IS_VALID_PTR(pTerrainBuffer));
		memcpy((void*)pTerrainBuffer, (void*)pTerrainCB, sizeof(STerrainConstantBuffer));		
		D3D11_UnlockConstantsBuffer(m_pTerrainCB);
		break;
	}

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::D3D11_CreateConstantsBuffer(ID3D11Buffer** ppCB, usint32 byteSize)
{
	SP_ASSERTR(m_pD3DDevice, S_NOTINIT);
	SP_ASSERTR(ppCB, S_INVALIDPARAM);

	D3D11_BUFFER_DESC cbDesc;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.ByteWidth = byteSize;


	if (Failure(m_pD3DDevice->CreateBuffer(&cbDesc, 0, ppCB)))
		return m_pEngine->LogE("Failed Create constants buffer! See stdout for more info.");


	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::D3D11_LockConstantsBuffer(ID3D11Buffer* pCB, void** pData)
{
	SP_ASSERTR(m_pD3DDeviceContext, S_NOTINIT);
	SP_ASSERTR(pCB && pData, S_INVALIDPARAM);

	D3D11_MAPPED_SUBRESOURCE cbSubRes;	
	if (Failure(m_pD3DDeviceContext->Map(pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &cbSubRes)))
		return m_pEngine->LogE("Failed lock constants buffer (map failed)!");

	*pData = cbSubRes.pData;	

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::D3D11_UnlockConstantsBuffer(ID3D11Buffer* pCB)
{
	SP_ASSERTR(m_pD3DDeviceContext, S_NOTINIT);
	SP_ASSERTR(pCB, S_INVALIDPARAM);

	m_pD3DDeviceContext->Unmap(pCB, 0);

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::SetViewportMatrices(IViewport* pViewport)
{	
	IViewport* pV = (pViewport) ? pViewport : GetTargetViewport();




	// PROJECTION MATRIX


	m_PerSceneCB.mtxProjection = pV->GetProjectionMatrix();

	FrameDump(m_PerSceneCB.mtxProjection, "Projection matrix");





	// VIEW MATRIX


	pV->RecalculateCameraViewMatrix();
	m_PerSceneCB.mtxView = pV->GetCameraViewMatrix();
	FrameDump(m_PerSceneCB.mtxView, "View matrix");

	m_PerSceneCB.eyePosition = float4(pV->GetCamera()->position, 1.0f);	

	SCamera* pCamera = pV->GetCamera();
	FrameDump(pCamera->position, "Camera position");	
	FrameDump(pCamera->rotation, "Camera rotation");



	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::SetViewportMatrices(const SMatrix& mtxView, const SMatrix& mtxProj)
{
	m_PerSceneCB.mtxProjection = mtxProj;
	m_PerSceneCB.mtxView = mtxView;
	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::SetWorldMatrix(const STransformationDesc& transform)
{
	m_PerObjectCB.mtxTransform = SMatrixTranspose(transform.BuildTRS());
	//m_PerObjectCB.mtxTransform = transform.BuildTRS();
	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::UpdateRasterizerState()
{	
	SP_SAFE_RELEASE(m_pRSState);
	if (Failure(m_pD3DDevice->CreateRasterizerState(&m_rsDesc, &m_pRSState)))
		return EngLog(S_ERROR, m_pEngine, "Failed recreate rasterizer state for updating rasterizer state.");

	m_pD3DDeviceContext->RSSetState(m_pRSState);
	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::UpdateDepthStencilState()
{
	SP_SAFE_RELEASE(m_pDepthStencilState);
	if (Failure(m_pD3DDevice->CreateDepthStencilState(&m_depthStencilDesc, &m_pDepthStencilState)))
		return EngLog(S_ERROR, m_pEngine, "Failed recreate depth stencil state for updating rasterizer state");

	m_pD3DDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);
	return S_SUCCESS;
}


// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::UpdateCullMode(EFrontFace ff)
{
	SP_ASSERTR(IsInited(), S_NOTINIT);	

	// update the rasterizer settings
	if (m_pEngine->GetSettings()->SetFrontFaceType(ff))
	{
		m_rsDesc.FrontCounterClockwise = (ff == eFF_CCW);
		return UpdateRasterizerState();
	}
	

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::EnableBackfaceCulling(bool state)
{
	SP_ASSERTR(IsInited(), S_NOTINIT);

	if ((m_rsDesc.CullMode == D3D11_CULL_NONE && !state) ||
		(m_rsDesc.CullMode != D3D11_CULL_NONE && state))
	{
		return S_SUCCESS;
	}

	// update rasterizer state
	m_rsDesc.CullMode = (state) ? D3D11_CULL_BACK : D3D11_CULL_NONE;

	return UpdateRasterizerState();
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::UpdatePolygonType(S_PRIMITIVE_TYPE type)
{
	SP_ASSERTR(IsInited(), S_NOTINIT);

	D3D11_PRIMITIVE_TOPOLOGY targetTP;
	switch (type)
	{
	case S_PRIM_RENDER_LINELIST: targetTP = D3D11_PRIMITIVE_TOPOLOGY_LINELIST; break;
	case S_PRIM_RENDER_POINTLIST: targetTP = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST; break;
	case S_PRIM_RENDER_TRIANGLESTRIP: targetTP = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP; break;
	default:
		targetTP = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}

	D3D11_PRIMITIVE_TOPOLOGY currentTP;
	m_pD3DDeviceContext->IAGetPrimitiveTopology(&currentTP);

	if (currentTP != targetTP)
		m_pD3DDeviceContext->IASetPrimitiveTopology(targetTP);	

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API void DirectX11Renderer::EnableWireframe(bool state /*=true*/)
{
	D3D11_FILL_MODE wireframeMode = state ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
	if (m_rsDesc.FillMode != wireframeMode)
	{
		m_rsDesc.FillMode = wireframeMode;
		UpdateRasterizerState();
	}
}

// --------------------------------------------------------------------
S_API void DirectX11Renderer::EnableDepthTest(bool state)
{
	if (m_depthStencilDesc.DepthEnable != state)
	{
		m_depthStencilDesc.DepthEnable = state;
		UpdateDepthStencilState();
	}
}


// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::UpdateSettings(const SSettingsDesc& dsc)
{
	return S_SUCCESS;
}


SP_NMSPACE_END
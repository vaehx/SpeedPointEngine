//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2011-2014, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include <Implementation\DirectX11\DirectX11Renderer.h>
#include <Implementation\DirectX11\DirectX11RenderPipeline.h>
#include <SSpeedPointEngine.h>
#include <Implementation\DirectX11\DirectX11Utilities.h>

SP_NMSPACE_BEG

using std::vector;

//////////////////////////////////////////////////////////////////////////////////////////////////
// DirectX11RenderDeviceCaps

// --------------------------------------------------------------------
S_API DirectX11RenderDeviceCaps::DirectX11RenderDeviceCaps()
	: m_MaxMSQuality(0) // lowest quality
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

DirectX11Renderer::DirectX11Renderer()
: m_pEngine(0),
m_pD3DDevice(0),
m_pD3DDeviceContext(0),
m_pRenderPipeline(0),
m_pAutoSelectedAdapter(0),
m_pTargetViewportBackBuffer(0),
m_pDXGIFactory(0),
m_pRenderTargetCollections(0)
{
};

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::AutoSelectAdapter(usint32 nW, usint32 nH)
{
	HRESULT hRes = S_OK;


	// Create and Validate the DXGI Factory
	CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&m_pDXGIFactory);	
	SP_ASSERTR(m_pDXGIFactory, S_ERROR);


	// Load and save all possible adapters
	IDXGIAdapter* pAdapter;
	for (usint32 iAdapter;
		m_pDXGIFactory->EnumAdapters(iAdapter, &pAdapter) != DXGI_ERROR_NOT_FOUND;
		++iAdapter)
	{		
		m_vAdapters.push_back(pAdapter);
	}

	SP_ASSERTR(m_vAdapters.size() > 0, S_ERROR, "No adapters found!");


	// find matching adapter and display mode	
	bool bFound = false;
	DXGI_FORMAT desiredBackBufferFormat =
		DXGI_FORMAT_R32G32B32A32_FLOAT;	// possibly change this to custom one depending on engine settings

	IDXGIOutput* pOutput;	
	for (auto iAdapter = m_vAdapters.begin(); iAdapter != m_vAdapters.end(); ++iAdapter)
	{
		if (!(pAdapter = *iAdapter))
			continue;

		for (usint32 iOutput; pAdapter->EnumOutputs(iOutput, &pOutput) != DXGI_ERROR_NOT_FOUND; ++iOutput)
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
				if (pModes[iMode].Width == nW && pModes[iMode].Height == nH)
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



	// You might also want to get some information about the vendor, the dedicated video mem or something else.
	// Do this by accessing m_AutoSelectedAdapterDesc.
	// For now we don't need this and thereby skip this.



	return S_SUCCESS;
}

// --------------------------------------------------------------------
SResult DirectX11Renderer::GetDisplayModeDesc(SDisplayModeDescription* pDesc)
{
	if (pDesc)
	{
		pDesc->width = m_AutoSelectedDisplayModeDesc.Width;
		pDesc->height = m_AutoSelectedDisplayModeDesc.Height;
		pDesc->refreshRate = m_AutoSelectedDisplayModeDesc.RefreshRate.Numerator
			/ m_AutoSelectedDisplayModeDesc.RefreshRate.Denominator;

		return pDesc->IsValid() ? S_SUCCESS : S_ERROR;
	}

	return S_INVALIDPARAM;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::SetRenderStateDefaults(void)
{
	SP_ASSERTR(IsInited(), S_NOTINIT);
	/*
	pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_PHONG );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, false );
	pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x20202020 );
	pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	pd3dDevice->SetRenderState( D3DRS_STENCILENABLE, false );

	pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
	*/

	// we probably need the settings of our engine
	SSettings::RenderSet& renderSettings = m_pEngine->GetSettings().render;

	// In DX11 we fist need a RSState interface
	D3D11_RASTERIZER_DESC rsDesc;
	memset((void*)&rsDesc, 0, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.AntialiasedLineEnable = false;	// ???
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.DepthBias = 0;
	rsDesc.DepthBiasClamp = 0;
	rsDesc.SlopeScaledDepthBias = 0;
	rsDesc.DepthClipEnable = true;	
	rsDesc.FillMode = renderSettings.bRenderWireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
	rsDesc.FrontCounterClockwise = (renderSettings.frontFaceType == eFF_CW);
	rsDesc.MultisampleEnable = renderSettings.bEnableMSAA;
	rsDesc.ScissorEnable = FALSE; // maybe change this to true someday	

	HRESULT hRes;
	if ((hRes = m_pD3DDevice->CreateRasterizerState(&rsDesc, &m_pDefaultRSState)))
	{
		return m_pEngine->LogE("Faield create Rasterizer State!");
	}

	m_pD3DDeviceContext->RSSetState(m_pDefaultRSState);	

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::InitDefaultViewport(HWND hWnd, int nW, int nH)
{
	SP_ASSERTR(m_pEngine && m_pD3DDevice && m_pD3DDeviceContext && m_pDXGIFactory, S_NOTINIT);	
	SP_ASSERTR(hWnd, S_INVALIDPARAM);

	SSettings& engineSet = m_pEngine->GetSettings();

	// Make sure to initialize the default viewport
	m_Viewport.Initialize(m_pEngine, false);

	// Setup swap chain description
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferCount = 1;	// only one buffer. render targets will be handled externally
	swapChainDesc.BufferDesc = m_AutoSelectedDisplayModeDesc;
	if (!engineSet.render.bEnableVSync)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.Flags = 0;
	swapChainDesc.OutputWindow = hWnd;	
	swapChainDesc.SampleDesc = GetD3D11MSAADesc(
		swapChainDesc.BufferDesc.Format,
		m_pD3DDevice,
		m_Settings.GetMSAACount(),
		m_Settings.GetMSAAQuality());	
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // i saw no purpose to change this to custom val as we want to support MSAA
	swapChainDesc.Windowed = engineSet.app.bWindowed;


	// Create the swapchain
	IDXGISwapChain* pSwapChain;
	if (Failure(m_pDXGIFactory->CreateSwapChain(m_pD3DDevice, &swapChainDesc, &pSwapChain)))
	{
		return m_pEngine->LogE("Failed create swap chain in InitDefaultViewport!");
	}
	*m_Viewport.GetSwapChainPtr() = pSwapChain;

	
	// Now create an RTV for this swapchain	
	ID3D11Resource* pBBResource;
	if (Failure(pSwapChain->GetBuffer(0, __uuidof(pBBResource), reinterpret_cast<void**>(&pBBResource))))
	{
		return m_pEngine->LogE("Failed retrieve BackBuffer resource of SwapChain in InitDefaultViewport!");
	}
	if (Failure(m_pD3DDevice->CreateRenderTargetView(pBBResource, 0, m_Viewport.GetRTVPtr())))
	{
		return m_pEngine->LogE("Failed create RTV for swapchain in InitDefaultViewport!");
	}

	// Set this swapchain as render target immediately
	m_pD3DDeviceContext->OMSetRenderTargets(1, m_Viewport.GetRTVPtr(), 0);


	// okay. done
	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::CreateDX11Device()
{
	SP_ASSERTR(m_pEngine && m_pAutoSelectedAdapter, S_NOTINIT);

	// Setup creation flags
	usint32 createFlags = 0;	
	if (!m_pEngine->GetSettings().app.bMultithreaded) createFlags |= D3D11_CREATE_DEVICE_SINGLETHREADED;	
#ifdef _DEBUG
	createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// To be sure we only use correct feature levels in our dx11 device, we have to explicitly set them	
	// !!! Notice: To optionally support DX11.1, you have to add the missing D3D_FEATURE_LEVEL_11_1 !!!
	D3D_FEATURE_LEVEL* pD3D11FeatureLevels = new D3D_FEATURE_LEVEL[]
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,

		// D3D9 features used to support old-hardware compatibility
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};
	usint32 nFeatureLevels = 6;

	// Now create the device with the highest specs
	HRESULT hRes;
	hRes = D3D11CreateDevice(
		m_pAutoSelectedAdapter,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		createFlags,
		pD3D11FeatureLevels, nFeatureLevels, D3D11_SDK_VERSION,
		&m_pD3DDevice, &m_D3DFeatureLevel, &m_pD3DDeviceContext);	// consider using deferred context for multithreading!

	// If that failed, try with Ref driver type
	if (Failure(hRes))
	{
		hRes = D3D11CreateDevice(
			m_pAutoSelectedAdapter,
			D3D_DRIVER_TYPE_REFERENCE,
			0,
			createFlags,
			pD3D11FeatureLevels, nFeatureLevels, D3D11_SDK_VERSION,
			&m_pD3DDevice, &m_D3DFeatureLevel, &m_pD3DDeviceContext);	// consider using deferred context for multithreading!
	}

	SP_SAFE_DELETE_ARR(pD3D11FeatureLevels, nFeatureLevels);

	// If everything failed, then don't go further
	if (Failure(hRes) || !m_pD3DDevice || !m_pD3DDeviceContext)
	{
		return m_pEngine->LogE("Failed create D3D11 Device!");
	}


	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API S_RENDERER_TYPE DirectX11Renderer::GetType(void)
{
	return S_DIRECTX11;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::Initialize(SpeedPointEngine* pEngine, HWND hWnd, int nW, int nH, bool bIgnoreAdapter)
{		
	SP_ASSERTR(pEngine && hWnd, S_INVALIDPARAM);	

	if (IsInited())
		Shutdown();	// shutdown before initializing again

	pEngine = pEngine;	





	// Auto Select Adapter and display mode
	if (Failure(AutoSelectAdapter(nW, nH)))
	{
		Shutdown();
		return m_pEngine->LogE("Failed automatic selection of Video adapter and display mode. Shut down.");
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
	if (Failure(InitDefaultViewport(hWnd, nW, nH)))
	{
		Shutdown();
		return S_ERROR;
	}
	// First update of Viewport Projection
	m_Viewport.Set3DProjection(S_PROJECTION_PERSPECTIVE, 50.0f, 0, 0);






	// Initialize the render pipeline
	m_pRenderPipeline = (IRenderPipeline*)new DirectX11RenderPipeline();
	if (Failure(m_pRenderPipeline->Initialize(pEngine, (IRenderer*)this)))
	{
		return pEngine->LogReport(S_ERROR, "Could not initialize DX9 Render Pipeline");
	}
	// !!! pRenderPipeline->SetFramePipeline() will be called by SFramePipeline::Initialize() !!!
	if (Failure(m_pRenderPipeline->SetTargetViewport((IViewport*)&m_Viewport)))
	{
		return pEngine->LogReport(S_ERROR, "Failed set Target Viewport!");
	}


	// okay. done.


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
	SP_SAFE_RELEASE(m_pDXGIFactory);
	SP_SAFE_RELEASE_CLEAR_VECTOR(m_vAdapters);
	
	if (m_pRenderTargetCollections)
		m_pRenderTargetCollections->clear();

	SP_SAFE_RELEASE(m_pD3DDevice);
	SP_SAFE_RELEASE(m_pD3DDeviceContext);

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::AddBindedFBO(usint32 index, IFBO* pFBO)
{
	SP_ASSERTR(m_pD3DDevice && m_pRenderTargetCollections && m_CurrentRenderTargetCollection.pvRenderTargets, S_NOTINIT);
	SP_ASSERTR(pFBO && pFBO->IsInitialized(), S_INVALIDPARAM);

	// check if this FBO is not already in the current collection
	for (auto iFBO = m_CurrentRenderTargetCollection.pvRenderTargets->begin();
		iFBO != m_CurrentRenderTargetCollection.pvRenderTargets->end();
		++iFBO)
	{
		if ((*iFBO).pFBO == pFBO && (*iFBO).iIndex == index)
			return S_SUCCESS;
	}

	m_CurrentRenderTargetCollection.pvRenderTargets->push_back(SRenderTarget(pFBO, index));

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::BindFBOs(ERenderTargetCollectionID collectionID, bool bStore)
{
	SP_ASSERTR(m_pD3DDevice && m_pRenderTargetCollections && m_CurrentRenderTargetCollection.pvRenderTargets, S_NOTINIT);

	if (bStore)
	{
		// add that collection to our map
		pair<map<ERenderTargetCollectionID, SRenderTargetCollection>::iterator, bool> ret;
		ret = m_pRenderTargetCollections->insert(pair<ERenderTargetCollectionID, SRenderTargetCollection>(collectionID, m_CurrentRenderTargetCollection));
		if (!ret.second)
		{
			if (m_pEngine)
				return m_pEngine->LogE("Failed add Render target collection in BindFBOs: collection with given id already exists.");
			else
				return S_ERROR;
		}
	}
	else
	{
		// retrive the collection
		auto itCollection = m_pRenderTargetCollections->find(collectionID);		
		SP_ASSERTR(itCollection != m_pRenderTargetCollections->end(), S_NOTFOUND);

		// get the render targets
		vector<SRenderTarget>* pvRenderTargets = (*itCollection).second.pvRenderTargets;
		if (pvRenderTargets) return S_ERROR;

		usint32 nRenderTargets = pvRenderTargets->size();

		// transform to an array
		ID3D11RenderTargetView** pRenderTargets = new ID3D11RenderTargetView*[nRenderTargets];
		usint32 nAddedRenderTargets = 0;
		for (auto itRenderTarget = pvRenderTargets->begin(); itRenderTarget != pvRenderTargets->end(); ++itRenderTarget)
		{			
			if (!itRenderTarget->pFBO->IsInitialized())
				continue;

			DirectX11FBO* pDXFBO = (DirectX11FBO*)itRenderTarget->pFBO;
			pRenderTargets[nAddedRenderTargets] = pDXFBO->GetRTV();
			++nAddedRenderTargets;
		}

		m_pD3DDeviceContext->OMSetRenderTargets(nAddedRenderTargets, pRenderTargets, 0);
	}

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::CreateAdditionalViewport(IViewport** pViewport)
{
	SP_ASSERTR(m_pEngine && m_pD3DDevice && m_pD3DDeviceContext && m_pDXGIFactory, S_NOTINIT);
	SP_ASSERTR(pViewport, S_INVALIDPARAM);

	// instanciate the viewport first
	DirectX11Viewport* pDXViewport = (DirectX11Viewport*)(*pViewport);
	pDXViewport = new DirectX11Viewport();
	pDXViewport->Initialize(m_pEngine, true);

	SSettings& engineSet = m_pEngine->GetSettings();

	// Setup swap chain description
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferCount = 1;	// only one buffer. render targets will be handled externally
	swapChainDesc.BufferDesc = m_AutoSelectedDisplayModeDesc;
	if (!engineSet.render.bEnableVSync)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.Flags = 0;
	swapChainDesc.OutputWindow = m_Viewport.GetWindow();
	swapChainDesc.SampleDesc = GetD3D11MSAADesc(
		swapChainDesc.BufferDesc.Format,
		m_pD3DDevice,
		m_Settings.GetMSAACount(),
		m_Settings.GetMSAAQuality());
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // i saw no purpose to change this to custom val as we want to support MSAA
	swapChainDesc.Windowed = engineSet.app.bWindowed;


	// Create the swapchain
	IDXGISwapChain* pSwapChain;
	if (Failure(m_pDXGIFactory->CreateSwapChain(m_pD3DDevice, &swapChainDesc, &pSwapChain)))
	{
		return m_pEngine->LogE("Failed create additional swap chain in CreateAdditionalViewport!");
	}
	*pDXViewport->GetSwapChainPtr() = pSwapChain;


	// Now create an RTV for this swapchain	
	ID3D11Resource* pBBResource;
	if (Failure(pSwapChain->GetBuffer(0, __uuidof(pBBResource), reinterpret_cast<void**>(&pBBResource))))
	{
		return m_pEngine->LogE("Failed retrieve BackBuffer resource of SwapChain in CreateAdditionalViewport!");
	}
	if (Failure(m_pD3DDevice->CreateRenderTargetView(pBBResource, 0, pDXViewport->GetRTVPtr())))
	{
		return m_pEngine->LogE("Failed create RTV for additional swapchain in CreateAdditionalViewport!");
	}

	// okay. done.

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API IRenderPipeline* DirectX11Renderer::GetRenderPipeline(void)
{
	return (IRenderPipeline*)m_pRenderPipeline;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::BeginScene(void)
{

}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::EndScene(void)
{

}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::SetTargetViewport(IViewport* pViewport)
{
	IFBO* pBackBuffer = 0;
	SP_ASSERTR(pViewport && (pBackBuffer = pViewport->GetBackBuffer()), S_INVALIDPARAM);
	SP_ASSERTR(m_pD3DDevice && m_pEngine && m_pD3DDeviceContext, S_NOTINIT);

	if (!pBackBuffer->IsInitialized())
		return S_INVALIDPARAM;

	m_pTargetViewportBackBuffer = pBackBuffer;

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API IViewport* DirectX11Renderer::GetTargetViewport(void)
{

}

// --------------------------------------------------------------------
S_API IViewport* DirectX11Renderer::GetDefaultViewport(void)
{

}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::UpdateViewportMatrices(IViewport* pViewport)
{

}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::RenderSolid(ISolid* pSolid, bool bTextured)
{
	
}



SP_NMSPACE_END
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
#include <Implementation\DirectX11\DirectX11VertexBuffer.h>
#include <Implementation\DirectX11\DirectX11IndexBuffer.h>

SP_NMSPACE_BEG

using std::vector;

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

DirectX11Renderer::DirectX11Renderer()
: m_pEngine(0),
m_pD3DDevice(0),
m_pD3DDeviceContext(0),
m_pRenderPipeline(0),
m_pAutoSelectedAdapter(0),
m_pTargetViewportBackBuffer(0),
m_pTargetViewport(0),
m_pDXRenderTarget(0),
m_pDXGIFactory(0),
m_pRenderTargetCollections(0),
m_iCurRTCollection(eRENDERTARGETS_NONE),
m_pDepthStencilState(0),
m_pDXMatrixCB(0)
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
SResult DirectX11Renderer::GetAutoSelectedDisplayModeDesc(SDisplayModeDescription* pDesc)
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

	// we probably need the settings of our engine
	SSettings::RenderSet& renderSettings = m_pEngine->GetSettings().render;



	// setup proper depth stencil testing
	// Setup the proper depth testing

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	depthStencilDesc.DepthEnable = true; // we probably won't change this, so this if fixed
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xff;
	depthStencilDesc.StencilWriteMask = 0xff;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	if (Failure(m_pD3DDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthStencilState)))
	{
		return m_pEngine->LogE("Failed create depth stencil state!");
	}
	m_pD3DDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);


	

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

	SViewportDescription vpDesc;
	
	
	vpDesc.fov = 60.0f;	// maybe you want to change this sometimes...


	vpDesc.width = nW;
	vpDesc.height = nH;
	vpDesc.useDepthStencil = true;	// we definetly want a depthstencil!
	vpDesc.hWnd = hWnd;

	if (Failure(m_Viewport.Initialize(m_pEngine, vpDesc, false)))
		return S_ERROR;


	// We still ned to setup the viewport in order tht d3d can map clip space coordinates to the render target space
	D3D11_VIEWPORT* pDXViewportDesc = m_Viewport.GetViewportDescPtr();
	pDXViewportDesc->Width = (float)nW;
	pDXViewportDesc->Height = (float)nH;
	pDXViewportDesc->MaxDepth = 1.0f;
	pDXViewportDesc->MinDepth = 0.0f;
	pDXViewportDesc->TopLeftX = 0.0f;
	pDXViewportDesc->TopLeftY = 0.0f;	

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






	// Initialize the render pipeline
	m_pRenderPipeline = (IRenderPipeline*)new DirectX11RenderPipeline();
	if (Failure(m_pRenderPipeline->Initialize(pEngine, (IRenderer*)this)))
	{
		return pEngine->LogReport(S_ERROR, "Could not initialize DX9 Render Pipeline");
	}
	// !!! pRenderPipeline->SetFramePipeline() will be called by SFramePipeline::Initialize() !!!


	if (Failure(SetTargetViewport((IViewport*)&m_Viewport)))
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
		vector<SRenderTarget>* pvRenderTargets = itCollection->second.pvRenderTargets;
		if (pvRenderTargets) return S_ERROR;

		usint32 nRenderTargets = pvRenderTargets->size();

		// Reset the old ptrs for single render targets
		m_pTargetViewport = 0;
		m_pTargetViewportBackBuffer = 0;
		m_pDXRenderTarget = 0;

		if (nRenderTargets > 0)
		{
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
			m_iCurRTCollection = collectionID;
		}	
		else
		{
			// Reset all targets. Will lead into an error!
			m_pD3DDeviceContext->OMSetRenderTargets(0, 0, 0);			
			m_iCurRTCollection = eRENDERTARGETS_NONE;

			m_pEngine->LogW("Render Target Collection to bind was empty. Resetting all Render Targets! Might lead into several Renderer Errors!");
		}
	}

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::BindFBO(IFBO* pFBO)
{
	SP_ASSERTR(IsInited(), S_NOTINIT);
	SP_ASSERTR(pFBO, S_INVALIDPARAM);

	ID3D11RenderTargetView* pDXFBO = ((DirectX11FBO*)pFBO)->GetRTV();
	m_pD3DDeviceContext->OMSetRenderTargets(1, &pDXFBO, 0);
	m_pDXRenderTarget = pDXFBO;
	m_pTargetViewportBackBuffer = pFBO;
	m_pTargetViewport = 0; // !
	m_iCurRTCollection = eRENDERTARGETS_NONE;

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::SetVBStream(IVertexBuffer* pVB, unsigned int index)
{
	SP_ASSERTR(IsInited(), S_NOTINIT);

	ID3D11Buffer* pDXVB = ((DirectX11VertexBuffer*)pVB)->Get
	m_pD3DDeviceContext->IASetVertexBuffers(index, 1, )
}


// --------------------------------------------------------------------
virtual SResult SetIBStream(IIndexBuffer* pIB);

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::CreateAdditionalViewport(IViewport** pViewport, const SViewportDescription& desc)
{
	SP_ASSERTR(m_pEngine && m_pD3DDevice && m_pD3DDeviceContext && m_pDXGIFactory, S_NOTINIT);
	SP_ASSERTR(pViewport, S_INVALIDPARAM);		


	// instanciate the viewport first and initialize it
	DirectX11Viewport* pDXViewport = (DirectX11Viewport*)(*pViewport);
	pDXViewport = new DirectX11Viewport();
	pDXViewport->Initialize(m_pEngine, desc, true);

	SSettings& engineSet = m_pEngine->GetSettings();




	// Initialize the depth-stencil view for this viewport
	if (Failure(pDXViewport->InitializeDepthStencilBuffer()))
	{
		return S_ERROR;
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
	// actually nothing really to do here.
	// - RTs are cleared explicitly with ClearRenderTargets()

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::EndScene(void)
{
	// actually nothing to do here in DX11
	// the Present method is called in the specific Rendering Pipeline technology stage.

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::ClearRenderTargets(void)
{
	SP_ASSERTR(IsInited(), S_NOTINIT);

	if (m_iCurRTCollection != eRENDERTARGETS_NONE)
	{
		// go through all render targets in the current collection
		auto itCollection = m_pRenderTargetCollections->find(m_iCurRTCollection);
		if (itCollection == m_pRenderTargetCollections->end() || !itCollection->second.pvRenderTargets)
			return S_ERROR;

		ID3D11RenderTargetView* pDXRTV;
		ID3D11DepthStencilView*	pDXDSV;
		for (auto itRT = itCollection->second.pvRenderTargets->begin(); itRT != itCollection->second.pvRenderTargets->end(); ++itRT)
		{
			if (!itRT->pFBO)
				continue;

			pDXRTV = ((DirectX11FBO*)itRT->pFBO)->GetRTV();
			if (pDXRTV)				
				m_pD3DDeviceContext->ClearRenderTargetView(pDXRTV, m_Settings.GetClearColorFloatArr());			

			pDXDSV = ((DirectX11FBO*)itRT->pFBO)->GetDSV();
			if (pDXDSV)
				m_pD3DDeviceContext->ClearDepthStencilView(pDXDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
		}
	}
	else
	{
		if (m_pDXRenderTarget)
		{
			m_pD3DDeviceContext->ClearRenderTargetView(m_pDXRenderTarget, m_Settings.GetClearColorFloatArr());
		}
	}

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::SetRenderTarget(IViewport* pViewport)
{
	// here, we assume that the viewport backbuffer is the only render target	

	IFBO* pBackBuffer = 0;
	SP_ASSERTR(pViewport && (pBackBuffer = pViewport->GetBackBuffer()), S_INVALIDPARAM);
	SP_ASSERTR(m_pD3DDevice && m_pEngine && m_pD3DDeviceContext, S_NOTINIT);

	if (!pBackBuffer->IsInitialized())
		return S_INVALIDPARAM;

	if (Failure(BindFBO(pBackBuffer)))
		return S_ERROR;

	m_pTargetViewport = pViewport;
	m_pTargetViewportBackBuffer = pBackBuffer;
	m_pDXRenderTarget = ((DirectX11FBO*)pBackBuffer)->GetRTV();
	m_iCurRTCollection = eRENDERTARGETS_NONE;

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::SetRenderTarget(DirectX11FBO* pFBO)
{
	SP_ASSERTR(pFBO, S_INVALIDPARAM);
	SP_ASSERTR(m_pD3DDeviceContext, S_NOTINIT);

	ID3D11RenderTargetView* pDXFBO = pFBO->GetRTV();
	m_pD3DDeviceContext->OMSetRenderTargets(1, &pDXFBO, 0);	
	m_pDXRenderTarget = pDXFBO;
	m_pTargetViewport = 0;
	m_pTargetViewportBackBuffer = (IFBO*)pFBO;
	m_iCurRTCollection = eRENDERTARGETS_NONE;

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::SetTargetViewport(IViewport* pViewport)
{
	SP_ASSERTR(pViewport, S_INVALIDPARAM);

	if (Failure(SetRenderTarget(pViewport)))
		return S_ERROR;

	// make also sure to synch the render pipe
	if (m_pRenderPipeline)
		m_pRenderPipeline->SetTargetViewport(pViewport);

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
S_API SResult DirectX11Renderer::SetupMatrixCB()
{
	if (Failure(D3D11_CreateConstantsBuffer(&m_pDXMatrixCB, sizeof(SDefMtxCB))))
		return S_ERROR;

	m_pD3DDeviceContext->VSSetConstantBuffers(0, 1, &m_pDXMatrixCB);

	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::UpdateMatrixCB()
{
	SDefMtxCB* pBuffer;
	if (Failure(D3D11_LockConstantsBuffer(m_pDXMatrixCB, (void**)&pBuffer)))
		return S_ERROR;

	if (!pBuffer)
	{
		pBuffer->mtxProjection = m_Matrices.mtxProjection;
		pBuffer->mtxView = m_Matrices.mtxView;
		pBuffer->mtxWorld = m_Matrices.mtxWorld;
	}

	return D3D11_UnlockConstantsBuffer(m_pDXMatrixCB);
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
		return m_pEngine->LogE("Failed Create constants buffer!");


	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::D3D11_LockConstantsBuffer(ID3D11Buffer* pCB, void** pData)
{
	SP_ASSERTR(m_pD3DDeviceContext, S_NOTINIT);
	SP_ASSERTR(pCB && pData, S_INVALIDPARAM);

	D3D11_MAPPED_SUBRESOURCE cbSubRes;	
	if (Failure(m_pD3DDeviceContext->Map(pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &cbSubRes)))
		return m_pEngine->LogE("Failed Map Constant buffer!");

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
	m_Matrices.mtxProjection = pViewport->GetProjectionMatrix();
	m_Matrices.mtxView = pViewport->GetCameraViewMatrix();	
	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::SetWorldMatrix(STransformable* t)
{
	m_Matrices.mtxWorld = t->GetWorldMatrix();
	return S_SUCCESS;
}

// --------------------------------------------------------------------
S_API SResult DirectX11Renderer::RenderSolid(ISolid* pSolid, bool bTextured)
{
	return m_pRenderPipeline->RenderSolidGeometry(pSolid, bTextured);
}



SP_NMSPACE_END
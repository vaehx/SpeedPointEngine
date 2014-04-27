// SpeedPoint DirectX9 Renderer

#include <Implementation\DirectX9\SDirectX9Renderer.h>
#include <Implementation\DirectX9\SDirectX9RenderPipeline.h>
#include <Implementation\DirectX9\SDirectX9Utilities.h>
#include <SSpeedPointEngine.h>

namespace SpeedPoint
{
	// **********************************************************************************

	S_API S_RENDERER_TYPE SDirectX9Renderer::GetType( void )
	{
		return S_DIRECTX9;
	}

	// **********************************************************************************

	S_API SResult SDirectX9Renderer::AutoSelectAdapter( int nW, int nH )
	{
		if( NULL == pDirect3D ) return S_ABORTED;				

		BOOL	bAdapterModeFound	= FALSE;
		int	iNumAdapters		= pDirect3D->GetAdapterCount();
		int	iNumAdapterModes	= 0;		

		D3DDISPLAYMODE		pAdapterModeN;
		D3DDISPLAYMODEFILTER	fDisplayModeFilter;
		for( int iAdapter = 0; iAdapter < iNumAdapters; iAdapter++ )
		{		
			if( FAILED( pDirect3D->GetAdapterIdentifier( iAdapter, 0, &setSettings.iAdapterIdentifier ) ) )
			{
				return S_ERROR;
			}
		
			// Convert SpeedPoint Backbuffer format to d3dformat
			D3DFORMAT fmtBuffer;
			switch( setSettings.iBackBufferFormat )
			{
			case S_BACKBUFFER16:
				{
					fmtBuffer = D3DFMT_R5G6B5;
					break;
				}
			case S_BACKBUFFER32:
				{
					fmtBuffer = D3DFMT_X8R8G8B8;
					break;
				}
			default:
				return pEngine->LogReport( S_ERROR, "Wrong BackBuffer format!" );
			}
		
			if( 0 >= ( iNumAdapterModes = pDirect3D->GetAdapterModeCount( iAdapter, fmtBuffer ) ) )
			{
				continue;	// that's not good
			}	
			
			for( int iAdapterMode = 0; iAdapterMode < iNumAdapterModes; iAdapterMode++ )
			{
				// Set DisplayModeEx Params
				fDisplayModeFilter.Format		= fmtBuffer;
				fDisplayModeFilter.Size			= sizeof( D3DDISPLAYMODEFILTER );
				fDisplayModeFilter.ScanLineOrdering	= D3DSCANLINEORDERING_INTERLACED;//(D3DSCANLINEORDERING)pSettings_->iFullscreenScanline;					
						
				if( FAILED( pDirect3D->EnumAdapterModes( iAdapter, fmtBuffer, iAdapterMode, &pAdapterModeN ) ) )
				{					
					return pEngine->LogReport( S_ERROR, "Failed Enumerate Adapter Modes!" );
				}			
		
				if( (nW == SP_TRIVIAL || pAdapterModeN.Width == nW) &&
					(nH == SP_TRIVIAL || pAdapterModeN.Height == nH) &&
					(setSettings.nRefreshRate == SP_TRIVIAL || pAdapterModeN.RefreshRate == setSettings.nRefreshRate) )
				{				
					bAdapterModeFound = TRUE;
					break;
				}
			}
		
			if( bAdapterModeFound )
			{			
				// If Adapter Mode found, take this
				setSettings.iAdapterIndex = iAdapter;		
				m_AutoSelectedDisplayMode = pAdapterModeN;
		
				return pEngine->LogReport( S_SUCCESS, "Good DX adapter mode found." );
			}		
		}
		
		// Reset adapter index
		setSettings.iAdapterIndex = SP_TRIVIAL;

		// Nothing Found, return Fail
		return pEngine->LogReport( S_ERROR, "Hardware does not support this video adapter mode. Check resolutions in settings!" );		
	}

	// **********************************************************************************

	S_API SResult SDirectX9Renderer::CollectDeviceCaps( void )
	{
		if( pDirect3D == NULL ) return S_ABORTED;

		if( FAILED( pDirect3D->GetDeviceCaps( setSettings.iAdapterIndex, setSettings.tyDeviceType, &cpsDeviceCaps ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Could not get DX Device caps" );
		}

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9Renderer::InitDefaultViewport ( HWND hWnd, int nW, int nH, DWORD* dwBehaviourFlags_ )
	{
		if( pDirect3D == NULL )
			return pEngine->LogReport( S_ABORTED, "Tried to init default viewport without initialized Direct3D Interface" );

		if( hWnd != NULL )
		{		
			// Initialize the viewport
			vpViewport.Initialize( pEngine );

			// -----------------------------------------------------------------------------------
			// Assign Settings
			vpViewport.hWnd = hWnd;
			vpViewport.nXResolution = nW;
			vpViewport.nYResolution = nH;

			// Assign auto selected display mode
			vpViewport.d3dDisplayMode = m_AutoSelectedDisplayMode;
	
			// Evaluate Vertex Processing Method
			if( dwBehaviourFlags_ != NULL )
			{				
				if( setSettings.iProcessingMode != SP_TRIVIAL )
				{
					switch( setSettings.iProcessingMode )
					{
					case S_PROCESS_HARDWARE:
						*dwBehaviourFlags_ = D3DCREATE_HARDWARE_VERTEXPROCESSING;
						break;
	
					case S_PROCESS_SOFTWARE:
						*dwBehaviourFlags_ = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
						break;
	
					case S_PROCESS_MIXED:
						*dwBehaviourFlags_ = D3DCREATE_MIXED_VERTEXPROCESSING;
						break;
	
					default:
						return pEngine->LogReport( S_ERROR, "Failed evaluate Processing Mode!" );
					}
				}
				else
				{
					*dwBehaviourFlags_ = D3DCREATE_HARDWARE_VERTEXPROCESSING;
				}
			}
	
			// -----------------------------------------------------------------------------------
	
			// Present Parameters
			ZeroMemory( &vpViewport.d3dPresentParameters, sizeof( vpViewport.d3dPresentParameters ) );
	
			// Decide whether Window- or Fullscreen mode.
			vpViewport.d3dPresentParameters.Windowed		= pEngine->GetSettings().bWindowed;
			if( !vpViewport.d3dPresentParameters.Windowed )
			{
				vpViewport.d3dPresentParameters.FullScreen_RefreshRateInHz = vpViewport.d3dDisplayMode.RefreshRate;
			}			
	
			// Define Backbuffer Format and Sizes
			// Note: Will use Fullscreen Params if not windowed.
			vpViewport.d3dPresentParameters.BackBufferFormat	= vpViewport.d3dDisplayMode.Format;
			vpViewport.d3dPresentParameters.BackBufferCount		= 1;
			vpViewport.d3dPresentParameters.BackBufferWidth		= vpViewport.nXResolution;
			vpViewport.d3dPresentParameters.BackBufferHeight	= vpViewport.nYResolution;			

			// Swap Effect. Typically not changed. Use Defaults.
			vpViewport.d3dPresentParameters.SwapEffect		= setSettings.iSwapEffect;

			// Set MultiSample Type
			vpViewport.d3dPresentParameters.MultiSampleType		= (D3DMULTISAMPLE_TYPE)setSettings.iMultiSampleType;
			
			// Set the Multisample quality (before, determine maximum quality)
			DWORD dwMaxQualityLevel;
			
//~~~~~~~~~~
// TODO: Check if HAL is available, otherwise switch to REF or change the multi sample quality afterwards
// As Default, the device Type is 0. This would throw an error in DX, so we temporarily set it to HAL
			setSettings.tyDeviceType = D3DDEVTYPE_HAL;
//~~~~~~~~~~
			
			if( FAILED( pDirect3D->CheckDeviceMultiSampleType(
				setSettings.iAdapterIndex,
				setSettings.tyDeviceType,
				vpViewport.d3dPresentParameters.BackBufferFormat,
				pEngine->GetSettings().bWindowed,
				vpViewport.d3dPresentParameters.MultiSampleType,
				&dwMaxQualityLevel ) ) )
			{
				dwMaxQualityLevel = 0;
			}
			
			switch( setSettings.iMultiSampleQuality )
			{
			case S_MULTISAMPLEQUALITY_HIGH:
				vpViewport.d3dPresentParameters.MultiSampleQuality = dwMaxQualityLevel;
				break;
			case S_MULTISAMPLEQUALITY_MEDIUM:
				vpViewport.d3dPresentParameters.MultiSampleQuality = (DWORD)(dwMaxQualityLevel * 0.5);
				break;
			default:
				vpViewport.d3dPresentParameters.MultiSampleQuality = 0;
			}			
	
			// Presentation Interval. V-Sync? if yes, decide how often to vsync
			switch( setSettings.iPresentQuality )
			{
			case S_PRESENT_HIGHQUALITY:
				if( cpsDeviceCaps.PresentationIntervals & D3DPRESENT_INTERVAL_FOUR )
					vpViewport.d3dPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_FOUR;
				else if( cpsDeviceCaps.PresentationIntervals & D3DPRESENT_INTERVAL_THREE )
					vpViewport.d3dPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_THREE;
				else if( cpsDeviceCaps.PresentationIntervals & D3DPRESENT_INTERVAL_TWO )
					vpViewport.d3dPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_TWO;
				else if( cpsDeviceCaps.PresentationIntervals & D3DPRESENT_INTERVAL_ONE )
					vpViewport.d3dPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
				else
					vpViewport.d3dPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

				break;

			case S_PRESENT_MEDIUMQUALITY:
				if( cpsDeviceCaps.PresentationIntervals & D3DPRESENT_INTERVAL_TWO )
					vpViewport.d3dPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_TWO;
				else
					vpViewport.d3dPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

				break;

			default:
				if( cpsDeviceCaps.PresentationIntervals & D3DPRESENT_INTERVAL_IMMEDIATE )
					vpViewport.d3dPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
				else
					vpViewport.d3dPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
			}	
	
			// Depth Stencil Format and if it is used at all			
			if( vpViewport.d3dPresentParameters.EnableAutoDepthStencil = setSettings.bAutoDepthStencil )
			{
				D3DFORMAT fmtDesiredDepthStencilFormat;
				switch( setSettings.iDepthStencilQuality )
				{
				case S_DEPTHSTENCIL_HIGHQUALITY:
					fmtDesiredDepthStencilFormat = D3DFMT_D32;
				default:
					fmtDesiredDepthStencilFormat = D3DFMT_D16;
				}

				if( pDirect3D->CheckDeviceFormat( setSettings.iAdapterIndex, setSettings.tyDeviceType, vpViewport.d3dDisplayMode.Format,
					D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, fmtDesiredDepthStencilFormat ) )
				{
					vpViewport.d3dPresentParameters.AutoDepthStencilFormat = fmtDesiredDepthStencilFormat;
				}
				else
				{
					vpViewport.d3dPresentParameters.AutoDepthStencilFormat = D3DFMT_D16;			
				}
			}
	
			// Set Device Window
			vpViewport.d3dPresentParameters.hDeviceWindow = vpViewport.hWnd;

			return pEngine->LogReport( S_SUCCESS, "Initialized default DX Viewport properly" );
		}
	
		return pEngine->LogReport( S_ERROR, "Window Handle for default DX Viewport is not valid." );
	
	}

	// **********************************************************************************

	S_API SResult SDirectX9Renderer::CreateDX9Device( DWORD * dwBehaviourFlags_ )
	{
	
		DWORD dwFlags = *dwBehaviourFlags_;
		HRESULT hr = E_FAIL;
	
		if( dwBehaviourFlags_ != NULL )
		{
			// Clear Device if already initialized
			if( pd3dDevice != NULL )
			{
				pd3dDevice->Release();
				pd3dDevice = NULL;
			}			
						
			DWORD dwDefFlags = (( pEngine->GetSettings().bMultithreaded ) ? D3DCREATE_MULTITHREADED : 0);
	
			// Predefine Params			
			UINT iAdapter = ( ( setSettings.iAdapterIndex ) ? D3DADAPTER_DEFAULT : setSettings.iAdapterIndex );
			D3DDISPLAYMODE* pFullscreenDisplayMode = &vpViewport.d3dDisplayMode;				
	
			// Create the device with several settings to get it to work
			S_BACKBUFFER_FORMAT iPreBackBufferFormat = setSettings.iBackBufferFormat;
			D3DFORMAT fmtPreBackBufferFormat = vpViewport.d3dDisplayMode.Format;

			setSettings.tyDeviceType = D3DDEVTYPE_HAL;

			if( FAILED( hr = pDirect3D->CheckDeviceType( setSettings.iAdapterIndex, D3DDEVTYPE_HAL,
				vpViewport.d3dDisplayMode.Format, vpViewport.d3dDisplayMode.Format, pEngine->GetSettings().bWindowed ) ) )
			{				
				// Check with lower backbuffer format
				if( setSettings.iBackBufferFormat == S_BACKBUFFER16 )
					vpViewport.d3dDisplayMode.Format = D3DFMT_X1R5G5B5;
				else
				{
					setSettings.iBackBufferFormat = S_BACKBUFFER16;
					vpViewport.d3dDisplayMode.Format = D3DFMT_R5G6B5;
				}

				hr = pDirect3D->CheckDeviceType( setSettings.iAdapterIndex, D3DDEVTYPE_HAL,
					vpViewport.d3dDisplayMode.Format, vpViewport.d3dDisplayMode.Format, pEngine->GetSettings().bWindowed );				
			}

			if( FAILED( hr ) )
			{
				// Go one step down and try with another 16byte Backbuffer format
				if( iPreBackBufferFormat != setSettings.iBackBufferFormat )
				{
					vpViewport.d3dDisplayMode.Format = D3DFMT_X1R5G5B5;
					hr = pDirect3D->CheckDeviceType( setSettings.iAdapterIndex, D3DDEVTYPE_HAL,
						vpViewport.d3dDisplayMode.Format, vpViewport.d3dDisplayMode.Format, pEngine->GetSettings().bWindowed );
				}
			}
			
			if( FAILED( hr ) )
			{
				// Hardware acceleration cannot be used. Try REF
				setSettings.tyDeviceType = D3DDEVTYPE_REF;

				vpViewport.d3dDisplayMode.Format = fmtPreBackBufferFormat;
				setSettings.iBackBufferFormat = iPreBackBufferFormat;

				if( FAILED( hr = pDirect3D->CheckDeviceType( setSettings.iAdapterIndex, D3DDEVTYPE_REF,
					vpViewport.d3dDisplayMode.Format, vpViewport.d3dDisplayMode.Format, pEngine->GetSettings().bWindowed ) ) )
				{
					// go one step down
					if( setSettings.iBackBufferFormat == S_BACKBUFFER32 )
					{
						setSettings.iBackBufferFormat = S_BACKBUFFER16;
						vpViewport.d3dDisplayMode.Format = D3DFMT_R5G6B5;
					}
					else
					{
						vpViewport.d3dDisplayMode.Format = D3DFMT_X1R5G5B5;
					}

					hr = pDirect3D->CheckDeviceType( setSettings.iAdapterIndex, D3DDEVTYPE_REF,
						vpViewport.d3dDisplayMode.Format, vpViewport.d3dDisplayMode.Format, pEngine->GetSettings().bWindowed );
				}

				if( FAILED( hr ) )
				{
					// try last settings
					if( setSettings.iBackBufferFormat != iPreBackBufferFormat )
					{
						vpViewport.d3dDisplayMode.Format = D3DFMT_X1R5G5B5;

						hr = pDirect3D->CheckDeviceType( setSettings.iAdapterIndex, D3DDEVTYPE_REF,
							vpViewport.d3dDisplayMode.Format, vpViewport.d3dDisplayMode.Format, pEngine->GetSettings().bWindowed );
					}
				}
			}			

			if( FAILED( hr ) )
				return pEngine->LogReport( S_ERROR, "Video device driver does meet system requirements" );

			/// --- NOW CREATE THE DEVICE ---

			// First try with given Processing Mode
			if( FAILED( hr = pDirect3D->CreateDevice( iAdapter, D3DDEVTYPE_HAL, vpViewport.hWnd,
				dwFlags | dwDefFlags, &vpViewport.d3dPresentParameters, &pd3dDevice ) ) )
			{	
				// Try with Mixed VertexProcessing
				hr = pDirect3D->CreateDevice( iAdapter, D3DDEVTYPE_HAL, vpViewport.hWnd,
					D3DCREATE_MIXED_VERTEXPROCESSING | dwDefFlags, &vpViewport.d3dPresentParameters, &pd3dDevice );
			}	
	
			if( FAILED( hr ) )
			{
				// Try with Software VertexProcessing
				hr = pDirect3D->CreateDevice( iAdapter, D3DDEVTYPE_HAL, vpViewport.hWnd,
					D3DCREATE_SOFTWARE_VERTEXPROCESSING | dwDefFlags, &vpViewport.d3dPresentParameters, &pd3dDevice );					
			}

			if( FAILED( hr ) )
			{
				// Try with Default Adapter
				hr = pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, vpViewport.hWnd,
					D3DCREATE_SOFTWARE_VERTEXPROCESSING | dwDefFlags, &vpViewport.d3dPresentParameters, &pd3dDevice );

				// Failed at the end
				if( FAILED( hr ) )					
					return pEngine->LogReport( S_ERROR, "FAILED CREATE DIRECTX9 DEVICE!" );
			}
	
			// Set BackBuffer of default viewport
			if( FAILED( pd3dDevice->GetRenderTarget( 0, &vpViewport.pBackBuffer ) ) )
				return pEngine->LogReport( S_ERROR, "Could not retrieve BackBuffer surface of default Render Target!" );

			// Bye
			return pEngine->LogReport( S_SUCCESS, "Initialized D3D Device properly" );
	
		}
	
		return pEngine->LogReport( S_ERROR, "Video device driver does not meet system requirements" );
	
	}

	// **********************************************************************************

	S_API SResult SDirectX9Renderer::SetRenderStateDefaults( void )
	{
		if( pd3dDevice == NULL ) 
			return pEngine->LogReport( S_ABORTED, "Tried set render state defaults with not-initialized renderer" );

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

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9Renderer::Initialize( SpeedPointEngine* eng, HWND hWnd, int nW, int nH, bool bIgnoreAdapter )
	{
/////////////////////////
//////// TODO: Add error messages / log entries
		if( eng == NULL || hWnd == NULL ) return S_ABORTED;			

		if( IsInited() ) Shutdown();		
		pEngine = eng;

		DWORD dwFlags = -1;		
	
		// Create the Direct3D Device
		// It will fail if no device found or no matching
		// DirectX Version was found in the target Computer
		if ( NULL == ( pDirect3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Could not create Direct3D Interface" );
		}		
	
		// Auto Select Adapters
		// This will Test all Video modes with given Settings.
		// Will Fail if no matching Video mode was found.
		if( !bIgnoreAdapter && Failure( AutoSelectAdapter ( nW, nH ) ) )
		{
			pDirect3D->Release();
			return pEngine->LogReport( S_ERROR, "Failed automatic selection of Video Adapter" );
		}
		else if( bIgnoreAdapter )
		{
			// Simply take those settings given
			D3DDISPLAYMODE dmo;
			pDirect3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &dmo );

			vpViewport.d3dDisplayMode.Format	= dmo.Format;
			vpViewport.d3dDisplayMode.Width		= vpViewport.nXResolution;
			vpViewport.d3dDisplayMode.Height	= vpViewport.nYResolution;

			setSettings.iAdapterIndex = D3DADAPTER_DEFAULT;
		}
	
		// Initialize the default Viewport
		// This viewport is forced to be generated. One is only able
		// to add more Viewports but then using other functions
		if( Failure( InitDefaultViewport( hWnd, nW, nH, &dwFlags ) ) )
		{
			pDirect3D->Release();
			return S_ERROR;
		}		
	
		// Now start the actual Interface to the
		// initialized DirectX Device. Will fail if neither hardware
		// nor mixed nor software processing is working.
		if( Failure( CreateDX9Device( &dwFlags ) ) )
		{
			pDirect3D->Release();
			return pEngine->LogReport( S_ERROR, "Failed Create DX9 Device!" );			
		}

		// Get The Device Caps from created DX Device
		if( Failure( CollectDeviceCaps() ) )
		{
			return pEngine->LogReport( S_ERROR, "Could not retrieve Device caps!" );
		}
		
		// Set Default Vertex Format. This Vertex format is FIXED for the whole engine
		if( FAILED( pd3dDevice->SetFVF( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2 ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed set DX9 FVF" );
		}
	
		// Set Default RenderStates and Texture Sampler States
		if( Failure( SetRenderStateDefaults() ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed set render state defaults!" );;
		}			

		// First update of Viewport Projection
		if( Failure( vpViewport.Set3DProjection( S_PROJECTION_PERSPECTIVE, 50.0f, 0, 0 ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed set 3D Projection" );
		}

		// Initialize the render pipeline
		pRenderPipeline = (SRenderPipeline*)new SDirectX9RenderPipeline();
		if( Failure( pRenderPipeline->Initialize( pEngine, (SRenderer*)this ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Could not initialize DX9 Render Pipeline" );
		}

		// !!! pRenderPipeline->SetFramePipeline() will be called by SFramePipeline::Initialize() !!!

		if( Failure( pRenderPipeline->SetTargetViewport( (SViewport*)&vpViewport ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed set Target Viewport!" );
		}

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9Renderer::CreateAdditionalViewport( SViewport** pViewport )
	{
		if( !IsInited() || pViewport == NULL ) return S_ABORTED;

		// Prepare Present Parameters
		SDirectX9Viewport* pDXViewport = new SDirectX9Viewport();
		*pViewport = (SViewport*)pDXViewport;
		pDXViewport->d3dPresentParameters.Windowed			= true;
		pDXViewport->d3dPresentParameters.BackBufferCount		= 1;
		pDXViewport->d3dPresentParameters.BackBufferFormat		= vpViewport.d3dPresentParameters.BackBufferFormat;
		pDXViewport->d3dPresentParameters.BackBufferWidth		= pDXViewport->GetSize().cx;
		pDXViewport->d3dPresentParameters.BackBufferHeight		= pDXViewport->GetSize().cy;
		pDXViewport->d3dPresentParameters.EnableAutoDepthStencil	= setSettings.bAutoDepthStencil;
		pDXViewport->d3dPresentParameters.hDeviceWindow			= pDXViewport->hWnd;
		pDXViewport->d3dPresentParameters.MultiSampleQuality		= vpViewport.d3dPresentParameters.MultiSampleQuality;
		pDXViewport->d3dPresentParameters.MultiSampleType		= vpViewport.d3dPresentParameters.MultiSampleType;
		pDXViewport->d3dPresentParameters.PresentationInterval		= vpViewport.d3dPresentParameters.PresentationInterval;
		pDXViewport->d3dPresentParameters.SwapEffect			= vpViewport.d3dPresentParameters.SwapEffect;
		
		// Create the additional Swap Chain
		if( FAILED( pd3dDevice->CreateAdditionalSwapChain( &pDXViewport->d3dPresentParameters, &pDXViewport->pSwapChain ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed to create additional swapchain for addition viewport" );
		}		
		
		// Get the backbuffer...
		if( FAILED( pDXViewport->pSwapChain->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pDXViewport->pBackBuffer ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Could not retrieve BackBuffer of recently created additional SwapChain" );
		}		

		// ... and the according display mode
		pDXViewport->pSwapChain->GetDisplayMode( &pDXViewport->d3dDisplayMode );

		return pEngine->LogReport( S_SUCCESS, "Created Additional Viewport successfully." );
	}

	// **********************************************************************************

	S_API SResult SDirectX9Renderer::SetTargetViewport( SViewport* pViewport )
	{
		if( !IsInited() || pViewport == NULL || pRenderPipeline == NULL ) return S_ABORTED;

		// Convert to DX Viewport
		SDirectX9Viewport* pDXViewport = (SDirectX9Viewport*)pViewport;

		if( pDXViewport->pBackBuffer == NULL ) return S_ERROR;

		if( FAILED( pd3dDevice->SetRenderTarget( 0, pDXViewport->pBackBuffer ) ) )
		{
			return pEngine->LogReport( S_ERROR, "Failed set active DX9 render target!" );
		}
		
		pRenderPipeline->SetTargetViewport( pViewport );

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SViewport* SDirectX9Renderer::GetTargetViewport( void )
	{
		if( pRenderPipeline == NULL ) return NULL;

		return pRenderPipeline->GetTargetViewport();
	}

	// **********************************************************************************

	S_API SViewport* SDirectX9Renderer::GetDefaultViewport( void )
	{
		return &vpViewport;
	}

	// **********************************************************************************

	S_API SResult SDirectX9Renderer::UpdateViewportMatrices( SViewport* pViewport )
	{
		if( !IsInited() || pViewport == NULL ) return S_ABORTED;

		// Convert to dx viewport
		SDirectX9Viewport* pDXViewport = (SDirectX9Viewport*)pViewport;

		pd3dDevice->SetTransform( D3DTS_PROJECTION, &SMatrixToDXMatrix(pDXViewport->mProjection) );
		pd3dDevice->SetTransform( D3DTS_VIEW, &SMatrixToDXMatrix(pDXViewport->mView) );

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9Renderer::Shutdown( void )
	{
		if( pRenderPipeline )
		{
			if( Failure( pRenderPipeline->Clear() ) )
				return S_ERROR;
		}

		vpViewport.Clear();

		if( pd3dDevice ) pd3dDevice->Release();
		pd3dDevice = NULL;

		if( pDirect3D ) pDirect3D->Release();
		pDirect3D = NULL;		

		pEngine = NULL;

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API bool SDirectX9Renderer::IsInited( void )
	{
		return pd3dDevice != NULL && pDirect3D != NULL && pRenderPipeline != NULL;
	}

	// **********************************************************************************

	S_API SRenderPipeline* SDirectX9Renderer::GetRenderPipeline( void )
	{
		return pRenderPipeline;
	}

	// **********************************************************************************

	S_API SResult SDirectX9Renderer::BeginScene(void)
	{
		if (!IsInited()) return S_ABORTED;

		if (Failure(pd3dDevice->BeginScene()))
			return pEngine->LogE("Failed Begin DX Scene!");		

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9Renderer::EndScene( void )
	{
		if (!IsInited()) return S_ABORTED;

		if (Failure(pd3dDevice->EndScene()))
			return pEngine->LogReport(S_ERROR, "Failed End DX Scene!");

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9Renderer::RenderSolid(SSolid* pSolid, bool bTextured)
	{
		return pRenderPipeline->RenderSolidGeometry(pSolid, bTextured);
	}
}
// ******************************************************************************************

//	SpeedPoint DirectX9 Settings

// ******************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\SRendererSettings.h>
#include <Util\SColor.h>
#include <d3d9.h>

namespace SpeedPoint
{
	// SpeedPoint DirectX9 Settings
	class S_API SDirectX9Settings : public SRendererSettings
	{
	public:
		SDirectX9Renderer*	pDXRenderer;

		int			nRefreshRate;		// in Hz
		int			iAdapterIndex;
		D3DADAPTER_IDENTIFIER9	iAdapterIdentifier;
		S_MULTISAMPLE_TYPE	iMultiSampleType;
		S_MULTISAMPLE_QUALITY	iMultiSampleQuality;
		S_PROCESSING_MODE	iProcessingMode;
		S_PRESENT_QUALITY	iPresentQuality;
		S_BACKBUFFER_FORMAT	iBackBufferFormat;
		S_DEPTHSTENCIL_QUALITY	iDepthStencilQuality;
		bool			bAutoDepthStencil;
		D3DDEVTYPE		tyDeviceType;
		D3DSWAPEFFECT		iSwapEffect;
		D3DCOLOR		cAlphaColorKey;

		// Default constructor
		SDirectX9Settings()
			: nRefreshRate( 60 ),
			iAdapterIndex( SP_TRIVIAL ),
			iSwapEffect( D3DSWAPEFFECT_COPY ),
			cAlphaColorKey( D3DCOLOR_XRGB( 255, 0, 255 ) ),
			iBackBufferFormat( S_BACKBUFFER32 ),
			iProcessingMode( S_PROCESS_HARDWARE ),
			iMultiSampleType( S_MULTISAMPLE_NONE ),
			iMultiSampleQuality( S_MULTISAMPLEQUALITY_MEDIUM ),
			iDepthStencilQuality( S_DEPTHSTENCIL_LOWQUALITY ),
			iPresentQuality( S_PRESENT_LOWQUALITY ),
			bAutoDepthStencil( true ) {};

		// Copy constructor
		SDirectX9Settings( const SDirectX9Settings& o )
			: nRefreshRate( o.nRefreshRate ),
			iAdapterIndex( o.iAdapterIndex ),
			iAdapterIdentifier( o.iAdapterIdentifier ),
			iMultiSampleType( o.iMultiSampleType ),
			iMultiSampleQuality( o.iMultiSampleQuality ),
			iProcessingMode( o.iProcessingMode ),
			iBackBufferFormat( o.iBackBufferFormat ),
			tyDeviceType( o.tyDeviceType ),
			iSwapEffect( o.iSwapEffect ),
			cAlphaColorKey( o.cAlphaColorKey ) {}

		// Initialize
		SResult SetRenderer( SRenderer* pRenderer );

		// Set the alpha color key for textures
		SResult SetAlphaColorKey( SColor colKey );

		// Set the multi sampling type
		SResult SetMultiSampleType( S_MULTISAMPLE_TYPE type );

		// Set the multi sampling quality
		SResult SetMultiSampleQuality( S_MULTISAMPLE_QUALITY quality );

		// Set the processing mode
		SResult SetProcessingMode( S_PROCESSING_MODE mode );
	};
}
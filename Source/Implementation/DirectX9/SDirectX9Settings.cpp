// SpeedPoint DirectX9 Renderer Settings

#include <Implementation\DirectX9\SDirectX9Renderer.h>
#include <Implementation\DirectX9\SDirectX9Settings.h>
#include <Util\SColor.h>

namespace SpeedPoint
{
	// **********************************************************************************
	
	S_API SResult SDirectX9Settings::SetRenderer( SRenderer* pRenderer )
	{
		if( pRenderer == NULL ) return S_ABORTED;

		pDXRenderer = (SDirectX9Renderer*)pRenderer;

		return S_SUCCESS;
	}
	
	// **********************************************************************************

	S_API SResult SDirectX9Settings::SetAlphaColorKey( SColor colKey )
	{
		cAlphaColorKey = D3DCOLOR_XRGB( (int)( colKey.r * 255.0f ), (int)( colKey.g * 255.0f ), (int)( colKey.b * 255.0f ) );
		return S_SUCCESS;
	}

	// **********************************************************************************
	
	S_API SResult SDirectX9Settings::SetMultiSampleType( S_MULTISAMPLE_TYPE type )
	{
		iMultiSampleType = type;
		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9Settings::SetMultiSampleQuality( S_MULTISAMPLE_QUALITY quality )
	{
		iMultiSampleQuality = quality;
		return S_SUCCESS;
	}
	
	// **********************************************************************************

	S_API SResult SDirectX9Settings::SetProcessingMode( S_PROCESSING_MODE mode )
	{
		iProcessingMode = mode;
		return S_SUCCESS;
	}
}
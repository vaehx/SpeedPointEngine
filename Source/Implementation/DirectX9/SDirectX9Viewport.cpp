// SpeedPoint DirectX9 Viewport

#include <Implementation\DirectX9\SDirectX9Viewport.h>
#include <Implementation\DirectX9\SDirectX9FrameBuffer.h>
#include <Implementation\DirectX9\SDirectX9Utilities.h>
#include <SpeedPoint.h>
#include <Util\SMatrix.h>

namespace SpeedPoint
{

	// **********************************************************************************
	
	S_API SResult SDirectX9Viewport::Initialize( SpeedPointEngine* eng )
	{
		Clear(); // make sure to be cleared, before initialization a second time		
		SP_ASSERTR((pEngine = eng), S_ERROR);

		return S_SUCCESS;
	}
	
	// **********************************************************************************

	S_API bool SDirectX9Viewport::IsAddition()
	{
		SP_ASSERTR(pEngine, false);
		
		// if the swap chain is not null, it is initialized as a swapchain and
		// thereby an additional viewport
		return pSwapChain != 0;
	}

	// **********************************************************************************

	S_API SIZE SDirectX9Viewport::GetSize( void )
	{
		SIZE sz;
		sz.cx = nXResolution;
		sz.cy = nYResolution;
		return sz;
	}

	// **********************************************************************************

	S_API SResult SDirectX9Viewport::SetSize( int nX, int nY )
	{
		nXResolution = nX;
		nYResolution = nY;

		// TODO: Change resolution of BackBuffer too

		return S_SUCCESS;
	}

	// **********************************************************************************
		
	S_API SVector2 SDirectX9Viewport::GetOrthographicVolume( void )
	{
		SVector2 res;
		res.x = fOrthoW;
		res.y = fOrthoH;
		return res;
	}

	// **********************************************************************************

	S_API float SDirectX9Viewport::GetPerspectiveFOV( void )
	{
		return fFOV;
	}

	// **********************************************************************************
		
	S_API SResult SDirectX9Viewport::Set3DProjection( S_PROJECTION_TYPE type, float fPerspDegFOV, float fow, float foh )
	{
		fFOV = asinf( fPerspDegFOV );
		fOrthoW = fow;
		fOrthoH = foh;				

		D3DXMATRIX mProj;

		SSettings engineSettings = pEngine->GetSettings();
		switch( type )
		{
		case S_PROJECTION_PERSPECTIVE:
			{
/// TODO: Eliminate calls of deprecated DirectX framework
				D3DXMatrixPerspectiveFovRH(
					&mProj,
					fFOV,
					(float)nXResolution / (float)nYResolution,
					engineSettings.render.fClipNear,
					engineSettings.render.fClipFar );				

				break;
			}
		case S_PROJECTION_ORTHOGRAPHIC:
			{
/// TODO: Eliminate calls of deprecated DirectX framework
				D3DXMatrixOrthoRH( &mProj, fOrthoW, fOrthoH,
					engineSettings.render.fClipNear, engineSettings.render.fClipFar );

				break;
			}
		}

		// Convert D3DXMATRIX to SMatrix
		mProjection = DXMatrixToSMatrix( mProj );

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SMatrix4 SDirectX9Viewport::GetProjectionMatrix()
	{
		return mProjection;
	}

	// **********************************************************************************

	S_API SResult SDirectX9Viewport::RecalculateCameraViewMatrix(SCamera* tempCam)
	{
		SCamera* c = pCamera;
		if (tempCam)
		{
			c = tempCam;
		}

		c->RecalculateViewMatrix();

		mView = c->GetViewMatrix();

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SMatrix4 SDirectX9Viewport::GetCameraViewMatrix()
	{
		return mView;
	}

	// **********************************************************************************
				
	S_API IFrameBuffer* SDirectX9Viewport::GetBackBuffer( void )
	{
		SDirectX9FrameBuffer* pDXRes = new SDirectX9FrameBuffer();
		pDXRes->pEngine = pEngine;
		pDXRes->pSurface = pBackBuffer;
		pDXRes->pTexture = NULL;		
		return (IFrameBuffer*)pDXRes;
	}

	// **********************************************************************************
		
	S_API SResult SDirectX9Viewport::SetCamera( SCamera* cam )
	{
		pCamera = cam;
		return S_SUCCESS;
	}

	// **********************************************************************************
		
	S_API SCamera* SDirectX9Viewport::GetCamera( void )
	{
		return pCamera;
	}

	// **********************************************************************************

	S_API SResult SDirectX9Viewport::Clear(void)
	{
		if (pBackBuffer) pBackBuffer->Release();
		if (pSwapChain) pSwapChain->Release();

		pBackBuffer = 0;
		pSwapChain = 0;
		hWnd = 0;
		pCamera = 0;
		pEngine = 0;
		d3dDisplayMode = D3DDISPLAYMODE();
		nXResolution = 0;
		nYResolution = 0;
		fOrthoW = 20.0f;
		fOrthoH = 20.0f;
		fFOV = 100.0f;

		SMatrixIdentity(&mProjection);

		return S_SUCCESS;
	}

}
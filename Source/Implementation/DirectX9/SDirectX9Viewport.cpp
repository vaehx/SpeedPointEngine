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
		SP_ASSERTR(!(pEngine = eng));

		return S_SUCCESS;
	}
	
	// **********************************************************************************

	S_API bool SDirectX9Viewport::IsAddition()
	{
		SP_ASSERTR(!pEngine, false);
		
		// if the swap chain is not null, it is initialized as a swapchain and
		// thereby an additional viewport
		return pSwapChain;
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

		switch( type )
		{
		case S_PROJECTION_PERSPECTIVE:
			{
				D3DXMatrixPerspectiveFovRH(
					&mProj,
					fFOV,
					(float)nXResolution / (float)nYResolution,
					pEngine->GetSettings()->fClipNear,
					pEngine->GetSettings()->fClipFar );				

				break;
			}
		case S_PROJECTION_ORTHOGRAPHIC:
			{
				D3DXMatrixOrthoRH( &mProj, fOrthoW, fOrthoH,
					pEngine->GetSettings()->fClipNear, pEngine->GetSettings()->fClipFar );

				break;
			}
		}

		// Convert D3DXMATRIX to SMatrix
		mProjection = DXMatrixToSMatrix( mProj );

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SDirectX9Viewport::RecalculateViewMatrix( SCamera* tempCam )
	{
		SCamera* c = pCamera;
		if( tempCam )
		{
			c = tempCam;
		}

		D3DXVECTOR3 vEyePt( c->vPosition.x, c->vPosition.y, c->vPosition.z );
		D3DXVECTOR3 vLookAt( c->vPosition.x + sin(c->vRotation.y)*cos(c->vRotation.x),
				     c->vPosition.y + sin(c->vRotation.x),
				     c->vPosition.z + cos(c->vRotation.y)*cos(c->vRotation.x));
		D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
	
		D3DXMATRIX mCam;
		D3DXMatrixLookAtRH( &mCam, &vEyePt, &vLookAt, &vUpVec );		

		mView = DXMatrixToSMatrix( mCam );

		return S_SUCCESS;
	}

	// **********************************************************************************
				
	S_API SFrameBuffer* SDirectX9Viewport::GetBackBuffer( void )
	{
		SDirectX9FrameBuffer* pDXRes = new SDirectX9FrameBuffer();
		pDXRes->pEngine = pEngine;
		pDXRes->pSurface = pBackBuffer;
		pDXRes->pTexture = NULL;		
		return (SFrameBuffer*)pDXRes;
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
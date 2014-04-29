// ********************************************************************************************

//	SpeedPoint Renderer Viewport

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include <Util\SVector2.h>

namespace SpeedPoint
{
	// SpeedPoint Renderer Viewport (abstract)
	class S_API IViewport
	{
	public:
		// Initialize this viewport with an Engine instance
		virtual SResult Initialize( SpeedPointEngine* pEngine ) = 0;

		// check whether this viewport is an additional one
		virtual bool IsAddition() = 0;

		// Get the size of this viewport
		virtual SIZE GetSize( void ) = 0;

		// Set the size of this viewport
		virtual SResult SetSize( int nX, int nY ) = 0;

		// Get the orthographic view volume size
		virtual SVector2 GetOrthographicVolume( void ) = 0;

		// Get the Field Of View of the perspective matrix
		virtual float GetPerspectiveFOV( void ) = 0;

		// Recalculate the projection matrix of this viewport
		virtual SResult Set3DProjection( S_PROJECTION_TYPE type, float fPerspDegFOV, float fOrthoW, float fOrthoH ) = 0;

		// Get the Projection matrix of this viewport
		virtual SMatrix4 GetProjectionMatrix() = 0;

		// Recalculate view matrix of the camera of this viewport
		virtual SResult RecalculateCameraViewMatrix( SCamera* tempCam ) = 0;

		// Get view matrix of the camera of this viewport
		virtual SMatrix4 GetCameraViewMatrix() = 0;

		/////// TODO: More viewport related settings

		// Get a pointer to the backbuffer framebuffer object
		virtual SFrameBuffer* GetBackBuffer( void ) = 0;

		// Set the pointer of the camera.
		virtual SResult SetCamera( SCamera* pCamera ) = 0;

		// Get the pointer to the camera
		virtual SCamera* GetCamera( void ) = 0;

		// Clearout the backbuffer + viewport
		virtual SResult Clear( void ) = 0;
	};
}
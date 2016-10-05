// ********************************************************************************************

//	SpeedPoint Renderer Viewport

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include "SPrerequisites.h"
#include "Vector2.h"
#include "Matrix.h"
#include "Camera.h"

SP_NMSPACE_BEG


struct S_API IFBO;
struct S_API IGameEngine;


struct S_API SProjectionDesc
{
	unsigned int fov;
	S_PROJECTION_TYPE projectionType;	
	float nearZ, farZ;
	bool bUseEngineZPlanes;
	float orthoW, orthoH;

	SProjectionDesc()
		: bUseEngineZPlanes(true)
	{
	}

	SProjectionDesc(const SProjectionDesc& dsc)
		: fov(dsc.fov),
		projectionType(dsc.projectionType),
		orthoW(dsc.orthoW),
		orthoH(dsc.orthoH),
		nearZ(dsc.nearZ),
		farZ(dsc.farZ),
		bUseEngineZPlanes(dsc.bUseEngineZPlanes)
	{
	}
};

struct S_API SViewportDescription
{
	usint32 width;
	usint32 height;
	SProjectionDesc projectionDesc;

	HWND hWnd;	// the window to render to

	bool useDepthStencil;

	SViewportDescription()
		: width(1024),
		height(768),		
		useDepthStencil(true)		
	{
	}

	SViewportDescription(const SViewportDescription& o)
		: width(o.width),
		height(o.height),		
		useDepthStencil(o.useDepthStencil),
		projectionDesc(o.projectionDesc)
	{
	}
};



// SpeedPoint Renderer Viewport (abstract)
struct S_API IViewport
{
public:
	virtual ~IViewport()
	{
	}

	// Summary:
	//	Initialize this viewport with an Engine instance
	virtual SResult Initialize(IGameEngine* pEngine, const SViewportDescription& desc, bool bIsAdditional = true) = 0;

	// check whether this viewport is an additional one
	virtual bool IsAdditional() = 0;

	// Get the size of this viewport
	virtual SIZE GetSize( void ) = 0;

	// Set the size of this viewport
	virtual SResult SetSize( int nX, int nY ) = 0;

	virtual SResult EnableVSync(bool enable = true) = 0;

	// Get the orthographic view volume size
	virtual SVector2 GetOrthographicVolume( void ) = 0;
	
	virtual unsigned int GetPerspectiveFOV( void ) = 0;
	
	virtual SResult SetProjectionByDesc(const SProjectionDesc& desc) = 0;
	virtual SProjectionDesc GetProjectionDesc() const = 0;
	virtual const SMatrix4& GetProjectionMatrix() const = 0;
	
	virtual SResult RecalculateCameraViewMatrix() = 0;		
	virtual SMatrix4& GetCameraViewMatrix() = 0;
	
	virtual HWND GetWindow() = 0;		
	virtual void SetWindow(HWND hWnd) = 0;		

	// Get a pointer to the backbuffer framebuffer object
	virtual IFBO* GetBackBuffer() = 0;

	// Set the pointer of the camera.
	virtual SResult SetCamera( SCamera* pCamera ) = 0;

	// Get the pointer to the camera
	virtual SCamera* GetCamera() = 0;

	virtual SCamera* GetDefaultCamera() = 0;

	// Clearout the backbuffer + viewport
	virtual SResult Clear( void ) = 0;
};

SP_NMSPACE_END
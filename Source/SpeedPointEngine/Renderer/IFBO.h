// **************************************************************************************

//	SpeedPoint FrameBuffer Object

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// **************************************************************************************

#pragma once

#include <Common\SPrerequisites.h>

SP_NMSPACE_BEG

struct S_API IRenderer;
struct S_API ITexture;


enum S_API EFBOType
{
	eFBO_R8G8B8A8,
	eFBO_F32, // 32bit float Red Channel
	eFBO_F16 // 16bit float Red Channel
};



// SpeedPoint FrameBufferObject (abstract)
struct S_API IFBO
{
public:
	virtual ~IFBO()
	{
	}

	// To allow this FBO to be used as a texture, use InitializeAsTexture() instead
	virtual SResult Initialize(EFBOType type, IRenderer* pRenderer, unsigned int width, unsigned int height) = 0;
	
	// Summary:
	//	Allows this FBO to be bound as a texture
	// Arguments:
	//	specification - Specification of the ITexture in the resource pool of the renderer.
	virtual SResult InitializeAsTexture(EFBOType type, IRenderer* pRenderer, unsigned int width, unsigned int height, const string& specification) = 0;

	// Summary:
	//	Creates a hardware depth buffer for this initialized FBO
	virtual SResult InitializeDepthBuffer() = 0;

	// Summary:
	//	Creates a hardware depth buffer for this initialized FBO and allows it to be used as a texture.
	// Arguments:
	//	specification - Specification of the ITexture in the resource pool of the renderer
	virtual SResult InitializeDepthBufferAsTexture(const string& specification) = 0;

	virtual bool IsInitialized() const = 0;
	virtual IRenderer* GetRenderer() = 0;
	virtual ITexture* GetTexture() const = 0;
	virtual ITexture* GetDepthBufferTexture() const = 0;

	// Clear buffers
	virtual void Clear(void) = 0;		
};


typedef struct S_API IFBO IFrameBufferObject;


SP_NMSPACE_END
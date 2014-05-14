// ********************************************************************************************

//	SpeedPoint Renderer

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

//	Note: Each Renderer instance has its own render pipeline.
//		And this render pipeline cannot be changed!

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>
using std::vector;

namespace SpeedPoint
{
	class S_API IRenderDeviceCapabilities
	{
	public:
		virtual SResult Collect(IRenderer* pRenderer) = 0;
		virtual unsigned int GetMaximumMSQuality() = 0;
	};

	struct S_API SDisplayModeDescription
	{
		usint32 width;
		usint32 height;		
		usint32 refreshRate;

		SDisplayModeDescription()
			: width(0), height(0), refreshRate(0)
		{
		}

		bool IsValid()
		{
			return (width > 0 && height > 0 && refreshRate > 0);
		}
	};


	enum S_API ERenderTargetCollectionID
	{
		eRENDERTARGETS_GBUFFER,
		eRENDERTARGETS_LBUFFER,
		eRENDERTARGETS_BACKBUFFER
	};


	struct S_API SRenderTarget
	{
		IFBO* pFBO;
		usint32 iIndex;

		SRenderTarget()
			: pFBO(0)
		{
		}

		SRenderTarget(IFBO* ppFBO, usint32 piIndex)
			: pFBO(ppFBO),
			iIndex(piIndex)
		{
		}
	};
	struct S_API SRenderTargetCollection
	{
		vector<SRenderTarget>* pvRenderTargets;

		SRenderTargetCollection()
		{
			pvRenderTargets = new vector<SRenderTarget>();
		}

		~SRenderTargetCollection()
		{
			if (pvRenderTargets)
			{
				pvRenderTargets->clear();
				delete pvRenderTargets;
			}
		}
	};



	// SpeedPoint Renderer (abstract)
	class S_API IRenderer
	{
	public:
		// Get the type of this renderer
		virtual S_RENDERER_TYPE GetType( void ) = 0;

		// Intialize the renderer
		virtual SResult Initialize( SpeedPointEngine* pEngine, HWND hWnd, int nW, int nH, bool bIgnoreAdapter ) = 0;

		// Arguments:
		//	pDesc - ptr to your instance of SDisplayModeDescription structure
		virtual SResult GetDisplayModeDesc(SDisplayModeDescription* pDesc) = 0;

		virtual bool AdapterModeSupported(usint32 nW, usint32 nH) = 0;

		// Set the current target viewport
		virtual SResult SetTargetViewport( IViewport* pViewport ) = 0;

		// Set a Frame buffer object as render target
		// This will only remember the ptr to the fbo. You have to manually call BindFBOs() to make them actually work
		virtual SResult AddBindedFBO(usint32 index, IFBO* pFBO) = 0;		

		// Binds the currently added binded FBOs and stores this collection with given id.
		// By that you only have to initialize them once and then simply call them again if you want
		// Arguments:
		//	bStore - set this to true if you just want to finish adding FBOs to a collection and store this collection
		//		If bStore is false, these added FBOs are untouched and the function looks for an existing collection with given index
		virtual SResult BindFBOs(ERenderTargetCollectionID collectionID, bool bStore = false) = 0;

		// Get the current target viewport
		virtual IViewport* GetTargetViewport( void ) = 0;

		// Get the default viewport
		virtual IViewport* GetDefaultViewport( void ) = 0;

		// Update projection and view matrix to those of the given viewport
		virtual SResult UpdateViewportMatrices( IViewport* pViewport ) = 0;

		// Create an an addition viewport
		virtual SResult CreateAdditionalViewport( IViewport** pViewport ) = 0;

		// Clearout everything (viewports, buffers, stop render Pipeline thread and task buffer)
		virtual SResult Shutdown( void ) = 0;

		// Check if this Renderer is initialized properly
		virtual bool IsInited( void ) = 0;

		// Get a pointer to the instance of the render pipeline
		virtual IRenderPipeline* GetRenderPipeline( void ) = 0;	
	
		virtual SResult BeginScene( void ) = 0;		
		virtual SResult EndScene( void ) = 0;	

		virtual IRendererSettings* GetSettings() = 0;
	};
}
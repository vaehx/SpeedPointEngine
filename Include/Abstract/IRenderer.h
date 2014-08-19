// ********************************************************************************************

//	SpeedPoint Renderer

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

//	Note: Each Renderer instance has its own render pipeline.
//		And this render pipeline cannot be changed!

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include <Util\SMatrix.h> // for Matrix CB
#include "IViewport.h"	// for SViewportDescription
#include "IGeometrical.h" // for STransformation

using std::vector;

SP_NMSPACE_BEG

struct S_API IRenderer;
struct S_API IRenderPipeline;
struct S_API IVertexBuffer;
struct S_API IIndexBuffer;
class S_API STransformable;
struct S_API ITexture;
struct S_API IGameEngine;
struct S_API IRendererSettings;
enum S_API EFrontFace;
struct S_API IResourcePool;
struct S_API IVisibleObject;




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
	eRENDERTARGETS_NONE,
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

	SRenderTargetCollection(const SRenderTargetCollection& o)
	{
		pvRenderTargets = new vector<SRenderTarget>();
		*pvRenderTargets = *o.pvRenderTargets;
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



struct S_API SDefMtxCB
{
	SMatrix4 mtxWorld;
	SMatrix4 mtxView;
	SMatrix4 mtxProjection;
};


// Summary:
//	Used in IRenderer::Draw()
struct S_API SDrawCallDesc
{
	IVertexBuffer* pVertexBuffer;
	IIndexBuffer* pIndexBuffer;
	usint32 iStartIBIndex;
	usint32 iEndIBIndex;
	usint32 iStartVBIndex;
	usint32 iEndVBIndex;

	STransformation transform;

	SDrawCallDesc()
		: pVertexBuffer(0),
		pIndexBuffer(0)
	{
	}

	SDrawCallDesc(const SDrawCallDesc& o)
		: pVertexBuffer(o.pVertexBuffer),
		pIndexBuffer(o.pIndexBuffer),
		iStartIBIndex(o.iStartIBIndex),
		iEndIBIndex(o.iEndIBIndex),
		iStartVBIndex(o.iStartVBIndex),
		iEndVBIndex(o.iEndVBIndex),
		transform(o.transform)
	{
	}
};


enum S_API ERenderPipelineTechnique
{
	eRENDER_FORWARD,
	eRENDER_DEFERRED
};


struct S_API SRenderDesc
{
	ERenderPipelineTechnique technique;
	SDrawCallDesc drawCallDesc;
	IGeometrical* pGeometrical;	
};


///////////////////////////////////////////////////////////////

// SpeedPoint Renderer
struct S_API IRenderer
{
public:
	virtual ~IRenderer()
	{
	}

	// Get the type of this renderer
	virtual S_RENDERER_TYPE GetType( void ) = 0;

	// Intialize the renderer
	virtual SResult Initialize(IGameEngine* pEngine, HWND hWnd, int nW, int nH, bool bIgnoreAdapter) = 0;

	// Arguments:
	//	pDesc - ptr to your instance of SDisplayModeDescription structure. This instance will be filled by the function
	virtual SResult GetAutoSelectedDisplayModeDesc(SDisplayModeDescription* pDesc) = 0;

	virtual bool AdapterModeSupported(usint32 nW, usint32 nH) = 0;

	// Set the current target viewport
	virtual SResult SetTargetViewport( IViewport* pViewport ) = 0;

	// Set a Frame buffer object as render target
	// This will only remember the ptr to the fbo. You have to manually call BindFBOs() to make them actually work
	virtual SResult AddRTCollectionFBO(usint32 index, IFBO* pFBO) = 0;		

	// Will update the collection if existing
	virtual SResult StoreRTCollection(ERenderTargetCollectionID asId) = 0;

	// Binds the currently added binded FBOs and stores this collection with given id.
	// By that you only have to initialize them once and then simply call them again if you want
	// Arguments:
	//	bStore - set this to true if you just want to finish adding FBOs to a collection and store this collection
	//		If bStore is false, these added FBOs are untouched and the function looks for an existing collection with given index
	virtual SResult BindRTCollection(ERenderTargetCollectionID collectionID) = 0;

	// Binds a single FrameBuffer Object (mainly used for binding a single backbuffer of a viewport in post-rendering section)
	virtual SResult BindSingleFBO(IFBO* pFBO) = 0;
	virtual SResult BindSingleFBO(IViewport* pViewport) = 0;

	virtual SResult SetVBStream(IVertexBuffer* pVB, unsigned int index = 0) = 0;
	virtual SResult SetIBStream(IIndexBuffer* pIB) = 0;

	// Arguments:
	//	Pass nullptr as pTex to empty this texture level
	virtual SResult BindTexture(ITexture* pTex, usint32 lvl = 0) = 0;
	virtual SResult BindTexture(IFBO* pFBO, usint32 lvl = 0) = 0;

	// Get the current target viewport
	virtual IViewport* GetTargetViewport( void ) = 0;

	// Get the default viewport
	virtual IViewport* GetDefaultViewport( void ) = 0;

	// Summary:
	//	Update projection and View matrix in Constants Buffer to those of the given viewport
	// Arguments:
	//	If pViewport is 0 then the current target-viewport is used (default 0)
	virtual SResult SetViewportMatrices(IViewport* pViewport = 0) = 0;
	virtual SResult SetViewportMatrices(const SMatrix& mtxView, const SMatrix& mtxProj) = 0;

	virtual SResult SetWorldMatrix(const STransformation& transform) = 0;

	// Create an an addition viewport
	virtual SResult CreateAdditionalViewport(IViewport** pViewport, const SViewportDescription& desc) = 0;

	// Clearout everything (viewports, buffers, stop render Pipeline thread and task buffer)
	virtual SResult Shutdown( void ) = 0;

	// Check if this Renderer is initialized properly
	virtual bool IsInited( void ) = 0;	

	virtual SResult BeginScene(void) = 0;		
	virtual SResult EndScene(void) = 0;	

	virtual SResult PresentTargetViewport(void) = 0;

	virtual SResult ClearBoundRTs(void) = 0;

	virtual IRendererSettings* GetSettings() = 0;

	virtual SResult UpdateCullMode(EFrontFace cullmode) = 0;
	virtual SResult EnableBackfaceCulling(bool state = true) = 0;
	virtual SResult UpdatePolygonType(S_PRIMITIVE_TYPE type) = 0;


	// Summary:
	//	Get the specific resource pool. Will instanciate one if not done yet
	virtual IResourcePool* GetResourcePool() = 0;


	// Summary:
	//	Schedules an object to be rendered using given render desc
	virtual SResult RenderGeometry(const SRenderDesc& dsc) = 0;


	// Summary:
	//	Draws the given geometry desc to the GBuffer and its depth buffer
	virtual SResult DrawDeferred(const SDrawCallDesc& desc) = 0;
	virtual SResult DrawDeferredLighting() = 0;
	virtual SResult MergeDeferred() = 0;

	// Summary:
	//	Draws the given geometry desc directly to the back buffer and the depth buffer
	virtual SResult DrawForward(const SDrawCallDesc& desc) = 0;


	// This function will return nullptr as IRenderer is just an interface!
	// Use implementation instead!
	static IRenderer* GetInstance()
	{
		return nullptr;
	}
};

SP_NMSPACE_END
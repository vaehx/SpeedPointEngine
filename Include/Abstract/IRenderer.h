// ********************************************************************************************

//	SpeedPoint Renderer Interface

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include "Matrix.h" // for Matrix CB
#include "IViewport.h"	// for SViewportDescription
#include "IGeometry.h" // for STransformation
#include "IObject.h"
#include "Material.h"
#include "IGameEngine.h"
#include "IFont.h" // for FontSize

using std::vector;

SP_NMSPACE_BEG

struct S_API IRenderer;
struct S_API IRenderPipeline;
struct S_API IVertexBuffer;
struct S_API IIndexBuffer;
class S_API STransformable;
struct S_API ITexture;
struct S_API IRendererSettings;
enum S_API EFrontFace;
struct S_API IResourcePool;
struct S_API IVisibleObject;




// Obsolete
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


// Obsolete.
struct S_API SRenderTargetCollection
{
	vector<SRenderTarget> pvRenderTargets;

	SRenderTargetCollection()
	{	
	}

	SRenderTargetCollection(const SRenderTargetCollection& o)
	{
		pvRenderTargets = o.pvRenderTargets;
	}

	~SRenderTargetCollection()
	{
	   pvRenderTargets.clear();
	}
};





enum S_API EConstantBufferType
{
	CONSTANTBUFFER_PERSCENE,
	CONSTANTBUFFER_PEROBJECT,
	CONSTANTBUFFER_TERRAIN
};


// Notice:
// - Constant Buffers must have a size which is a multiple of 16. That means you should
//   only use SMatrix4 or SVector4 (=float4) data types in here. Otherwise, make shure
//   to add proper padding bytes.

struct S_API SPerSceneConstantBuffer
{

	// mtxView and mtxProjection could also be in a separate CB, but we want to
	// save memory bandwidth. Also it might be that the camera or projection changes
	// per scene.

	SMatrix4 mtxView;
	SMatrix4 mtxProjection;

    // Pos used instead of Dir, to avoid struggling around with angles when calculating
    // sun traveling due to TOD.   sun dir = normalize(-sunPosition)
	float4 sunPosition;

	float4 eyePosition;

	SPerSceneConstantBuffer& operator = (const SPerSceneConstantBuffer& b)
	{
		mtxView = b.mtxView;
		mtxProjection = b.mtxProjection;
		sunPosition = b.sunPosition;
		eyePosition = b.eyePosition;
		return *this;
	}
};

struct S_API SPerObjectConstantBuffer
{
	SMatrix4 mtxTransform;

	SPerObjectConstantBuffer& operator = (const SPerObjectConstantBuffer& b)
	{
		mtxTransform = b.mtxTransform;
		return *this;
	}
};

struct S_API STerrainConstantBuffer
{
	float dmTexRatioU, dmTexRatioV;
	float fTerrainDMFadeRadius;
	float fTerrainMaxHeight;
	unsigned int vtxHeightMapSz[2];


	float struct_padding[2];

	STerrainConstantBuffer& operator = (const STerrainConstantBuffer& b)
	{
		dmTexRatioU = b.dmTexRatioU;
		dmTexRatioV = b.dmTexRatioV;
		fTerrainDMFadeRadius = b.fTerrainDMFadeRadius;
		fTerrainMaxHeight = b.fTerrainMaxHeight;
		vtxHeightMapSz[0] = b.vtxHeightMapSz[0];
		vtxHeightMapSz[1] = b.vtxHeightMapSz[1];
		return *this;
	}
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

	//STransformationDesc transform;

	EPrimitiveType primitiveType;

	SDrawCallDesc()
		: pVertexBuffer(0),
		pIndexBuffer(0),
		primitiveType(PRIMITIVE_TYPE_TRIANGLELIST)
	{
	}

	SDrawCallDesc(const SDrawCallDesc& o)
		: pVertexBuffer(o.pVertexBuffer),
		pIndexBuffer(o.pIndexBuffer),
		iStartIBIndex(o.iStartIBIndex),
		iEndIBIndex(o.iEndIBIndex),
		iStartVBIndex(o.iStartVBIndex),
		iEndVBIndex(o.iEndVBIndex),
		//transform(o.transform),
		primitiveType(o.primitiveType)
	{
	}
};


enum S_API ERenderPipeline
{
	eRENDER_FORWARD,
	eRENDER_DEFERRED
};

struct SRenderSubset
{
	SMaterial material;	// copied, no pointer used
	SDrawCallDesc drawCallDesc;
	bool render; // true to render, false to skip
};

struct S_API SRenderDesc
{
	ERenderPipeline renderPipeline;
	SRenderSubset* pSubsets;
	unsigned int nSubsets;

	//SMaterial material;
	STransformationDesc transform;
	//IGeometry* pGeometry;
};

struct S_API STerrainRenderDesc
{
	SDrawCallDesc *pDrawCallDescs;
	unsigned int nDrawCallDescs;
	STransformationDesc transform;	
	ITexture* pColorMap;
	ITexture* pDetailMap;
	ITexture* pVtxHeightMap;
	STerrainConstantBuffer constants;
	bool bUpdateCB;
	bool bRender;

	STerrainRenderDesc()
		: pDrawCallDescs(nullptr),
		nDrawCallDescs(0),
		pColorMap(nullptr),
		pDetailMap(nullptr),
		bUpdateCB(true),
		bRender(true),
		pVtxHeightMap(nullptr)
	{
	}
};

struct S_API SRenderSlot
{
	SRenderDesc renderDesc;
	bool keep; // true if slot may no be released after rendered

	SRenderSlot()
		: keep(true)
	{
	}
};


// Font Render Slot used in the Font renderer to render font.
// Pass it to the actual Renderer which will keep a queue of all
// Font Render Slots. These are then drawn after all other drawing,
// in the UI passes.
struct S_API SFontRenderSlot
{
	char* text; // only pass a charbuffer that is created with new[]!
	SColor color;
	unsigned int screenPos[2];
	bool alignRight;
	bool keep;
	EFontSize fontSize;

	SFontRenderSlot()
		: text(0),
		color(1.0f, 1.0f, 1.0f),
		keep(false),
		alignRight(false),
		fontSize(eFONTSIZE_NORMAL)
	{
		screenPos[0] = 0;
		screenPos[1] = 0;
	}

	SFontRenderSlot(const SFontRenderSlot& frs)
	{
		copy_from(frs);
	}

	SFontRenderSlot& operator = (const SFontRenderSlot& frs)
	{
		copy_from(frs);
	}

	void copy_from(const SFontRenderSlot& frs)
	{
		text = 0;
		if (IS_VALID_PTR(frs.text))
			sp_strcpy(&text, frs.text);

		color = frs.color;
		screenPos[0] = frs.screenPos[0];
		screenPos[1] = frs.screenPos[1];
		keep = frs.keep;
		alignRight = frs.alignRight;
		fontSize = frs.fontSize;
	}

	~SFontRenderSlot()
	{
		if (IS_VALID_PTR(text))
			delete[] text;

		text = 0;
	}
};


///////////////////////////////////////////////////////////////

#define MAX_BOUND_RTS 8

// SpeedPoint Renderer
struct S_API IRenderer
{
public:
	virtual ~IRenderer()
	{
	}

	// Get the type of this renderer
	virtual S_RENDERER_TYPE GetType( void ) = 0;

	// Intialize the renderer. Settings are taken from engine settings.
	virtual SResult Initialize(IGameEngine* pEngine, bool bIgnoreAdapter) = 0;

	virtual IGameEngine* GetEngine() = 0;

	virtual IFontRenderer* InitFontRenderer() = 0;

	// Arguments:
	//	pDesc - ptr to your instance of SDisplayModeDescription structure. This instance will be filled by the function
	virtual SResult GetAutoSelectedDisplayModeDesc(SDisplayModeDescription* pDesc) = 0;

	virtual bool AdapterModeSupported(usint32 nW, usint32 nH) = 0;
	
	virtual SResult SetTargetViewport( IViewport* pViewport ) = 0;		
	virtual IViewport* GetTargetViewport(void) = 0;

	// Get the default viewport
	virtual IViewport* GetDefaultViewport(void) = 0;

	// Create an an addition viewport
	virtual SResult CreateAdditionalViewport(IViewport** pViewport, const SViewportDescription& desc) = 0;

	/*
	// Will update the collection if existing
	virtual SResult StoreRTCollection(ERenderTargetCollectionID asId) = 0;
	*/
	


	// Summary:
	//	Binds given collection of render targets.
	// Arguments:
	//	depthFBO - The DepthStencil Buffer of this FBO will be bound for rendering
	virtual SResult BindRTCollection(std::vector<IFBO*>& fboCollection, IFBO* depthFBO, const char* dump_name = 0) = 0;

	virtual bool BoundMultipleRTs() const = 0;

	// Binds a single FrameBuffer Object (mainly used for binding a single backbuffer of a viewport in post-rendering section)
	virtual SResult BindSingleRT(IFBO* pFBO) = 0;
	virtual SResult BindSingleRT(IViewport* pViewport) = 0;

	virtual IFBO* GetBoundSingleRT() = 0;



	virtual SResult SetVBStream(IVertexBuffer* pVB, unsigned int index = 0) = 0;
	virtual SResult SetIBStream(IIndexBuffer* pIB) = 0;

	// Arguments:
	//	Pass nullptr as pTex to empty this texture level
	virtual SResult BindTexture(ITexture* pTex, usint32 lvl = 0, bool vs = false) = 0;
	virtual SResult BindTexture(IFBO* pFBO, usint32 lvl = 0) = 0;		
	

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
	virtual void EnableWireframe(bool state = true) = 0;
	virtual void EnableDepthTest(bool state = true) = 0;


	// Summary:
	//	Get the specific resource pool. Will instanciate one if not done yet
	virtual IResourcePool* GetResourcePool() = 0;

	virtual SRenderSlot* GetRenderSlot() = 0;

	// *pSlot is set to 0 after release
	virtual void ReleaseRenderSlot(SRenderSlot** pSlot) = 0;

	virtual STerrainRenderDesc* GetTerrainRenderDesc() = 0;

	
	// *pFRS is set to 0 ptr after releases
	virtual void ReleaseFontRenderSlot(SFontRenderSlot** pFRS) = 0;

	virtual SFontRenderSlot* GetFontRenderSlot() = 0;



	/*
	// Summary:
	//	Schedules an object to be rendered using given render desc once.
	//	Render schedule slot is freed after rendered
	virtual SResult RenderGeometry(const SRenderDesc& dsc) = 0;
	*/

	/*
	// Summary:
	//	Set up terrain render description that is used when rendering the terrain (immediately
	//	before unleashing render schedule.
	//	Render schedule slot is freed after rendered
	virtual SResult RenderTerrain(const STerrainRenderDesc& tdsc) = 0;
	*/


	// Summary:
	//	Draws the given geometry desc to the GBuffer and its depth buffer
	virtual SResult DrawDeferred(const SDrawCallDesc& desc) = 0;
	virtual SResult DrawDeferredLighting() = 0;
	virtual SResult MergeDeferred() = 0;

	// Summary:
	//	Draws the given geometry desc directly to the back buffer and the depth buffer
	virtual SResult DrawForward(const SDrawCallDesc& desc) = 0;


	// Summary:
	//	Updates settings of the device if needed and flagged in SettingsDesc mask
	virtual SResult UpdateSettings(const SSettingsDesc& dsc) = 0;


	virtual void DumpFrameOnce() = 0;


	// This function will return nullptr as IRenderer is just an interface!
	// Use implementation instead!
	static IRenderer* GetInstance()
	{
		return nullptr;
	}


protected:
	// Summary:
	//	Update projection and View matrix in Constants Buffer to those of the given viewport
	// Arguments:
	//	If pViewport is 0 then the current target-viewport is used (default 0)
	virtual SResult SetViewportMatrices(IViewport* pViewport = 0) = 0;
	virtual SResult SetViewportMatrices(const SMatrix& mtxView, const SMatrix& mtxProj) = 0;

	virtual SResult SetWorldMatrix(const STransformationDesc& transform) = 0;
};

SP_NMSPACE_END
// ********************************************************************************************

//	SpeedPoint Renderer Interface

// ********************************************************************************************

#pragma once

#include "IViewport.h"	// for SViewportDescription
#include "IFont.h" // for FontSize
#include "IRenderAPI.h"
#include "IShader.h"
#include "IConstantsBuffer.h"
#include <Common\Matrix.h> // for Matrix CB
#include <Common\SPrerequisites.h>

using std::vector;

SP_NMSPACE_BEG

struct S_API IRenderer;
struct S_API IRenderPipeline;
struct S_API IVertexBuffer;
struct S_API IIndexBuffer;
struct S_API ITypelessInstanceBuffer;
class S_API STransformable;
struct S_API ITexture;
struct S_API IRendererSettings;
struct S_API IResourcePool;
struct S_API IVisibleObject;

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





enum S_API EIllumModel
{
	eILLUM_HELPER,	// cannot be rendered with deferred pipeline!
	eILLUM_SKYBOX,
	eILLUM_PHONG,
	eILLUM_BLINNPHONG,
	eILLUM_COOKTORRANCE
};

struct S_API SShaderResources
{
	ITexture* textureMap;	// aggregation, color for full unlit roughness
	ITexture* normalMap;
	ITexture* ambientOcclusionMap;
	float3 diffuse;
	float3 emissive;

	EIllumModel illumModel;

	ITexture* roughnessMap;
	float roughness;	// if the glossiness Map is set it is used instead of the global gloss factor

	bool enableBackfaceCulling;

	SShaderResources()
		: textureMap(0),
		normalMap(0),
		ambientOcclusionMap(0),
		diffuse(0, 0, 0),
		emissive(0, 0, 0),
		roughnessMap(0),
		roughness(1.0f),
		illumModel(eILLUM_BLINNPHONG),
		enableBackfaceCulling(true)
	{
	}

	SShaderResources(const SShaderResources& src)
	{
		CopyFrom(src);
	}

	SShaderResources& operator = (const SShaderResources& src)
	{
		CopyFrom(src);
		return *this;
	}

	void CopyFrom(const SShaderResources& src)
	{
		textureMap = src.textureMap;
		normalMap = src.normalMap;
		ambientOcclusionMap = src.ambientOcclusionMap;
		emissive = src.emissive;
		diffuse = src.diffuse;
		illumModel = src.illumModel;
		roughnessMap = src.roughnessMap;
		roughness = src.roughness;
		enableBackfaceCulling = src.enableBackfaceCulling;
	}
};





// !! deprecated !!
enum S_API EConstantBufferType
{
	CONSTANTBUFFER_PERSCENE,
	CONSTANTBUFFER_PEROBJECT,
	CONSTANTBUFFER_TERRAIN
};


enum S_API ERenderPipeline
{
	eRENDER_FORWARD,
	eRENDER_DEFERRED
};

struct SRenderSubset
{
	SShaderResources shaderResources;
	SDrawCallDesc drawCallDesc;
	bool render; // true to render, false to skip
	bool bOnce;	// set bRender to false if drawcall passed the pipeline
	
	// All pure black (0,0,0) pixels will be transparent, blending enabled
	bool enableAlphaTest;

	SRenderSubset()
		: bOnce(false),
		enableAlphaTest(false)
	{
	}
};

enum ETextureSampling
{
	eTEX_SAMPLE_BILINEAR,
	eTEX_SAMPLE_POINT
};

// Do not copy with memcpy or std::copy!
struct S_API SRenderDesc
{
	ERenderPipeline renderPipeline;

	SRenderSubset* pSubsets;
	unsigned int nSubsets;

	//STransformationDesc transform;
	SMatrix transform;

	SMatrix viewMtx; // custom view-mtx, only used if bCustomViewProjMtx is set to true
	SMatrix projMtx; // custom proj-mtx, only used if bCustomViewProjMtx is set to true
	bool bCustomViewProjMtx; // if false, uses current viewport view- and projection matrix

	bool bDepthStencilEnable;
	bool bInverseDepthTest; // use GREATER function for depth test

	ETextureSampling textureSampling;

	SRenderDesc()
		: bCustomViewProjMtx(false),
		bDepthStencilEnable(true),
		bInverseDepthTest(false),
		textureSampling(eTEX_SAMPLE_BILINEAR),
		pSubsets(0)
	{
	}

	SRenderDesc(const SRenderDesc& rd)
	{
		Copy(rd);
	}

	~SRenderDesc()
	{
		Clear();
	}

	inline SRenderDesc& operator =(const SRenderDesc& rd)
	{
		Copy(rd);
		return *this;
	}

	void Copy(const SRenderDesc& rd)
	{
		Clear();

		renderPipeline = rd.renderPipeline;
		nSubsets = rd.nSubsets;
		transform = rd.transform;
		viewMtx = rd.viewMtx;
		projMtx = rd.projMtx;
		bCustomViewProjMtx = rd.bCustomViewProjMtx;
		bDepthStencilEnable = rd.bDepthStencilEnable;
		bInverseDepthTest = rd.bInverseDepthTest;
		textureSampling = rd.textureSampling;
		pSubsets = 0;

		// copy subsets array
		if (rd.nSubsets > 0 && IS_VALID_PTR(rd.pSubsets))
		{
			pSubsets = new SRenderSubset[rd.nSubsets];
			memcpy(pSubsets, rd.pSubsets, sizeof(SRenderSubset) * rd.nSubsets);
		}
	}

	void Clear()
	{
		if (IS_VALID_PTR(pSubsets))
			delete[] pSubsets;

		pSubsets = 0;
		nSubsets = 0;
	}
};

struct S_API SInstancedRenderDesc
{
	Mat44 transform;
	SShaderResources shaderResources;
	IVertexBuffer* pVertexBuffer;
	ITypelessInstanceBuffer* pInstanceBuffer;
	bool render; // false to skip this instance batch during render
	bool enableDepthTest;
	bool enableDepthWrite;

	SInstancedRenderDesc()
		: render(true),
		enableDepthTest(true),
		enableDepthWrite(true)
	{
	}
};


// Summary:
//	Draw call information for a terrain lod level
struct S_API STerrainDrawCallDesc : SDrawCallDesc
{	
	bool bRender;

	STerrainDrawCallDesc()
		: SDrawCallDesc(),
		bRender(false)
	{
	}

	STerrainDrawCallDesc(const STerrainDrawCallDesc& o)
		: SDrawCallDesc(o),
		bRender(o.bRender)
	{
	}
};

struct S_API STerrainRenderDesc
{
	STerrainDrawCallDesc *pDrawCallDescs;
	unsigned int nDrawCallDescs;
	STransformationDesc transform;	
	ITexture* pColorMap;
	ITexture* pVtxHeightMap;
	
	ITexture** pLayerMasks;	// numLayers
	ITexture** pDetailMaps; // numLayers
	unsigned int nLayers;

	STerrainConstants constants;	
	bool bUpdateCB;
	bool bRender;

	STerrainRenderDesc()
		: pDrawCallDescs(nullptr),
		nDrawCallDescs(0),
		pColorMap(nullptr),
		bUpdateCB(true),
		bRender(false),
		pVtxHeightMap(nullptr),
		nLayers(0),
		pLayerMasks(0),
		pDetailMaps(0)
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
	string text;
	SColor color;
	unsigned int screenPos[2];
	bool alignRight;
	bool keep;
	EFontSize fontSize;

	SFontRenderSlot()
		: color(1.0f, 1.0f, 1.0f),
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
		text = frs.text;
		color = frs.color;
		screenPos[0] = frs.screenPos[0];
		screenPos[1] = frs.screenPos[1];
		keep = frs.keep;
		alignRight = frs.alignRight;
		fontSize = frs.fontSize;
	}
};


struct S_API SLightDesc
{	
	SColor intensity; // spectral intensity

	// Deferred Shading:
	SRenderDesc renderDesc; // contains transformation matrix

	// Forward:
	float radius;
	float range; // for spot lights
	Vec3f position;
	Vec3f direction;
};


///////////////////////////////////////////////////////////////

struct S_API SRenderBudgetTimer
{
	unsigned int timerId;
	unsigned int numUsed;

	SRenderBudgetTimer()
		: numUsed(0)
	{
	}
};

///////////////////////////////////////////////////////////////

enum EShaderFileType
{
	eSHADERFILE_SKYBOX,

	eSHADERFILE_FORWARD_HELPER,	// Forward helper effect
	eSHADERFILE_FORWARD,

	eSHADERFILE_FONT,	// forward font effect

	eSHADERFILE_DEFERRED_ZPASS,	// ZPass for Deferred Shading
	eSHADERFILE_DEFERRED_SHADING,	// shading pass for Deferred Shading
	eSHADERFILE_TERRAIN,	// Deferred Shading Terrain
	eSHADERFILE_GUI,
	eSHADERFILE_PARTICLES
};

///////////////////////////////////////////////////////////////

#define MAX_BOUND_RTS 8

struct S_API SRendererInitParams
{
	unsigned int resolution[2];
	HWND hWnd;
	bool multithreaded;
	bool windowed;
	bool enableVSync;
	unsigned int vsyncInterval;
	string shaderResourceDirectory; // must NOT end with a slash
	EFrontFace frontFace;
	EAntiAliasingQuality antiAliasingQuality;
	EAntiAliasingCount antiAliasingCount;
	SColor clearColor;

	SRendererInitParams()
		: hWnd(0),
		multithreaded(false),
		windowed(false),
		enableVSync(true),
		vsyncInterval(1),
		shaderResourceDirectory("Effects"),
		frontFace(eFF_CW),
		antiAliasingQuality(eAAQUALITY_LOW),
		antiAliasingCount(eAACOUNT_NONE),
		clearColor(0, 0, 0)
	{
		resolution[0] = 1024;
		resolution[1] = 768;
	}
};

// SpeedPoint Renderer
struct S_API IRenderer
{
public:
	virtual ~IRenderer()
	{
	}

	// Get the type of this renderer
	virtual S_RENDERER_TYPE GetType( void ) = 0;

	virtual const SRendererInitParams& GetParams() const = 0;

	// Intialize the renderer. Settings are taken from engine settings.
	virtual SResult Initialize(const SRendererInitParams& params) = 0;

	// Will return 0 before Initialize() was called!
	virtual IFontRenderer* GetFontRenderer() const = 0;

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
	
	// Factory method to create a new RenderTarget
	virtual IFBO* CreateRT() const = 0;

	// Summary:
	//	Binds given collection of render targets.
	// Arguments:
	//	depthFBO - The DepthStencil Buffer of this FBO will be bound for rendering
	virtual SResult BindRTCollection(const std::vector<IFBO*>& fboCollection, IFBO* depthFBO, const char* dump_name = 0) = 0;

	virtual bool BoundMultipleRTs() const = 0;

	// Binds a single FrameBuffer Object (mainly used for binding a single backbuffer of a viewport in post-rendering section)
	virtual SResult BindSingleRT(IFBO* pFBO) = 0;
	virtual SResult BindSingleRT(IViewport* pViewport) = 0;

	virtual IFBO* GetBoundSingleRT() = 0;



	virtual SResult SetVBStream(IVertexBuffer* pVB, unsigned int index = 0) = 0;
	virtual SResult SetIBStream(IIndexBuffer* pIB) = 0;
	virtual SResult SetInstanceStream(ITypelessInstanceBuffer* pInstanceBuffer, unsigned int index = 1) = 0;

	// Arguments:
	//	Pass nullptr as pTex to empty this texture level
	virtual SResult BindVertexShaderTexture(ITexture* pTex, usint32 lvl = 0) = 0;
	virtual SResult BindTexture(ITexture* pTex, usint32 lvl = 0) = 0;
	virtual SResult BindTexture(IFBO* pFBO, usint32 lvl = 0) = 0;
	
	virtual ITexture* GetDummyTexture() const = 0;
	

	// Clearout everything (viewports, buffers, stop render Pipeline thread and task buffer)
	virtual SResult Shutdown( void ) = 0;

	// Check if this Renderer is initialized properly
	virtual bool IsInited( void ) = 0;	

	virtual IShader* CreateShader() const = 0;

	virtual void BindShaderPass(EShaderPassType type) = 0;
	virtual IShaderPass* GetCurrentShaderPass() const = 0;

	virtual string GetShaderPath(EShaderFileType type) const = 0;

	virtual SResult BeginScene(void) = 0;
	virtual SResult EndScene(void) = 0;

	virtual SResult Render(const SRenderDesc& renderDesc) = 0;
	virtual SResult RenderInstanced(const SInstancedRenderDesc& renderDesc) = 0;
	virtual SResult RenderTerrain(const STerrainRenderDesc& terrainRenderDesc) = 0;

	virtual SResult RenderDeferredLight(const SLightDesc& light) = 0;

	virtual SResult PresentTargetViewport(void) = 0;

	virtual SResult ClearBoundRTs(void) = 0;

	virtual SResult UpdateCullMode(EFrontFace cullmode) = 0;
	virtual SResult EnableBackfaceCulling(bool state = true) = 0;
	virtual SResult UpdatePolygonType(S_PRIMITIVE_TYPE type) = 0;
	virtual void EnableWireframe(bool state = true) = 0;
	virtual void EnableDepthTest(bool enableDepthTest = true, bool enableDepthWrite = true) = 0;



	virtual IConstantsBuffer* CreateConstantsBuffer() const = 0;

	// vs - set to true to bind the CB to vertex shader too. If false, any bound CB to VS is unbound.	
	virtual void BindConstantsBuffer(const IConstantsBuffer* cb, bool vs = false) = 0;

	virtual SSceneConstants* GetSceneConstants() const = 0;
	virtual void SetSunPosition(const Vec3f& pos) = 0;


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

	// Summary:
	//	Draws the given geometry desc directly to the back buffer and the depth buffer
	virtual SResult Draw(const SDrawCallDesc& desc) = 0;


	ILINE virtual void DumpFrameOnce() = 0;
	ILINE virtual bool DumpingThisFrame() const = 0;


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
	virtual void SetViewProjMatrix(IViewport* pViewport = 0) = 0;
	virtual void SetViewProjMatrix(const SMatrix& mtxView, const SMatrix& mtxProj) = 0;
};


///////////////////////////////////////////////////////////////


template<typename T>
class ConstantsBufferHelper
{
private:
	IConstantsBuffer* m_pCB;

public:
	ConstantsBufferHelper()
		: m_pCB(0)
	{
	}

	SResult Initialize(IRenderer* pRenderer)
	{
		Clear();
		m_pCB = pRenderer->CreateConstantsBuffer();
		return m_pCB->Initialize(pRenderer, sizeof(T));
	}

	void Clear()
	{
		delete m_pCB;
		m_pCB = 0;
	}

	void Update()
	{
		if (!m_pCB)
			return;

		m_pCB->Update();
	}

	// May return 0 if not initialized!
	T* GetConstants() const
	{
		if (!m_pCB)
			return 0;

		return (T*)m_pCB->GetData();
	}

	const IConstantsBuffer* GetCB() const
	{
		return m_pCB;
	}
};


SP_NMSPACE_END
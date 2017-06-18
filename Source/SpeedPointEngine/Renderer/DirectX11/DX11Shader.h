//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2016 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\IShader.h"
#include "..\IRenderer.h"
#include "..\IConstantsBuffer.h"
#include <Common\SPrerequisites.h>
#include <vector>
#include "DX11.h"

using std::vector;

SP_NMSPACE_BEG

class S_API DX11Renderer;
struct S_API IRenderer;
struct S_API IFBO;

// DX11 implementation of a shader (PS + VS)
class S_API DX11Shader : public IShader
{
private:
	DX11Renderer* m_pDXRenderer;

	ID3D11InputLayout* m_pVSInputLayout;

	ID3D11PixelShader* m_pPixelShader;
	ID3D11VertexShader* m_pVertexShader;

public:
	DX11Shader();
	virtual ~DX11Shader();

	virtual SResult Load(IRenderer* pRenderer, const SShaderInfo& info);	
	virtual void Clear();

	virtual SResult Bind();
};



// TODO: GET THIS OUT OF THE DIRECTX11 IMPLEMENTATION PROJECT AND MOVE IT INTO A MORE GENERAL RENDERER

///////////////////////////////////////////////////////////////////////////////////
class S_API ShadowmapShaderPass : public IShaderPass
{
private:
	IRenderer* m_pRenderer;
	IFBO* m_pShadowmap;
	IShader* m_pShader;
	ConstantsBufferHelper<SObjectConstants> m_Constants;

public:
	ShadowmapShaderPass();

	virtual ~ShadowmapShaderPass()
	{
		Clear();
	}

	virtual SResult Initialize(IRenderer* pRenderer);
	virtual void ReloadShaders();
	virtual void Clear();
	virtual void OnEndFrame();
	virtual SResult Bind();
	virtual void SetShaderResources(const SShaderResources& pShaderResources, const Mat44& transform);

	IFBO* GetShadowmap() const;
};




struct S_API SMatObjConstants : SObjectConstants
{
	float matRoughnes;
	float __padding[3];
};

///////////////////////////////////////////////////////////////////////////////////
class S_API ForwardShaderPass : public IShaderPass
{
public:
	ForwardShaderPass()
		: m_pRenderer(0),
		m_pShader(0)		
	{
	}

	virtual ~ForwardShaderPass()
	{
		Clear();
	}	

	// IShaderPass:
	virtual SResult Initialize(IRenderer* pRenderer);
	virtual void ReloadShaders();
	virtual void Clear();
	virtual SResult Bind();
	virtual void SetShaderResources(const SShaderResources& pShaderResources, const Mat44& transform);

private:
	IRenderer* m_pRenderer;
	IShader* m_pHelperShader;
	IShader* m_pShader;
	IShader* m_pSkyboxShader;
	ConstantsBufferHelper<SMatObjConstants> m_Constants;
};


///////////////////////////////////////////////////////////////////////////////////
class S_API GBufferShaderPass : public IShaderPass
{
public:
private:
#define NUM_GBUFFER_LAYERS 2
	vector<IFBO*> m_pGBuffer;
	vector<IFBO*> m_BoundFBOs;

	IShader* m_pShader;
	IShader* m_pTerrainShader;
	ConstantsBufferHelper<SMatObjConstants> m_Constants;
	IRenderer* m_pRenderer;

	void BindGBufferRTs();

public:
	GBufferShaderPass()
		: m_pRenderer(0),
		m_pShader(0)
	{
	}

	virtual ~GBufferShaderPass()
	{
		Clear();
	}

	// IShaderPass:
	virtual SResult Initialize(IRenderer* pRenderer);
	virtual void ReloadShaders();
	virtual void Clear();

	virtual SResult Bind();
	virtual void OnUnbind();
	virtual void OnEndFrame();
	virtual void SetShaderResources(const SShaderResources& pShaderResources, const Mat44& transform);
	virtual void BindTerrainResources(ITexture* pHeightmap, ITexture* pLayerMask, ITexture* pColormap, const SShaderResources& shaderResources);

	ITexture* GetGBufferTexture(unsigned int i) const;
	ITexture* GetDepthBufferTexture() const;
};

///////////////////////////////////////////////////////////////////////////////////
class S_API DeferredLightShaderPass : public IShaderPass
{
public:
	#pragma pack(16)
	struct SLightObjectConstants : public SObjectConstants
	{
		Mat44 mtxViewportProjInv;
		Vec3f lightPos;
		float lightMaxDistance;
		Vec3f lightDirection; // for spot, directional. Points away from light source
		float lightDecay;
		Vec3f lightIntensity;
		unsigned int lightType; // 0 = sun, 1 = point
	};

private:
	GBufferShaderPass* m_pGBufferPass;
	ShadowmapShaderPass* m_pShadowmapPass;
	IRenderer* m_pRenderer;
	IShader* m_pShader;
	IFBO* m_pLightBuffer;
	ConstantsBufferHelper<SLightObjectConstants> m_Constants;

public:
	DeferredLightShaderPass(GBufferShaderPass* pGBufferPass, ShadowmapShaderPass* pShadowmapPass)
		: m_pGBufferPass(pGBufferPass),
		m_pShadowmapPass(pShadowmapPass),
		m_pRenderer(0),
		m_pShader(0)
	{
	}

	virtual ~DeferredLightShaderPass() { Clear(); }

	virtual SResult Initialize(IRenderer* pRenderer);
	virtual void ReloadShaders();
	virtual void Clear();
	virtual void OnEndFrame();
	virtual SResult Bind();
	virtual void SetShaderResources(const SShaderResources& pShaderResources, const Mat44& transform);
	
	// Call this before Renderer::Render(), which will call SetShaderResources()
	void SetLightConstants(const SLightObjectConstants& constants);

	ITexture* GetLightBufferTexture() const;
};

///////////////////////////////////////////////////////////////////////////////////
class S_API ShadingShaderPass : public IShaderPass
{
private:
	GBufferShaderPass* m_pGBufferPass;
	ShadowmapShaderPass* m_pShadowmapPass;
	DeferredLightShaderPass* m_pLightPass;
	IRenderer* m_pRenderer;
	IShader* m_pShader;
	IShader* m_pTerrainShader;
	ConstantsBufferHelper<SObjectConstants> m_Constants;

public:
	ShadingShaderPass(GBufferShaderPass* pGBufferPass, DeferredLightShaderPass* pLightPass, ShadowmapShaderPass* pShadowmapPass)
		: m_pGBufferPass(pGBufferPass),
		m_pLightPass(pLightPass),
		m_pShadowmapPass(pShadowmapPass),
		m_pRenderer(0)
	{
	}

	virtual ~ShadingShaderPass()
	{
		Clear();
	}

	virtual SResult Initialize(IRenderer* pRenderer);
	virtual void ReloadShaders();
	virtual void Clear();

	virtual SResult Bind();
	virtual void SetShaderResources(const SShaderResources& pShaderResources, const Mat44& transform);
	virtual void BindTerrainResources(ITexture* pHeightmap, ITexture* pLayerMask, ITexture* pColormap, const SShaderResources& shaderResources);
};

///////////////////////////////////////////////////////////////////////////////////
struct S_API SParticleEmitterConstants : public SObjectConstants
{
	unsigned __int32 time; // us passed since emitter creation, 4B
	unsigned __int32 particleLifetime; // us for a particle to live, 4B
	float particleSize; // 4B
	float particleSpeed; // 4B
};

class S_API ParticleShaderPass : public IShaderPass
{
private:
	IRenderer* m_pRenderer;
	IShader* m_pShader;
	ConstantsBufferHelper<SParticleEmitterConstants> m_Constants;

public:
	ParticleShaderPass()
		: m_pRenderer(0)
	{
	}

	virtual ~ParticleShaderPass()
	{
		Clear();
	}

	virtual SResult Initialize(IRenderer* pRenderer);
	virtual void ReloadShaders();
	virtual void Clear();
	virtual SResult Bind();
	virtual void OnUnbind();
	virtual void SetShaderResources(const SShaderResources& pShaderResources, const Mat44& transform);

	void SetConstants(const SParticleEmitterConstants& constants);
};

///////////////////////////////////////////////////////////////////////////////////
class S_API GUIShaderPass : public IShaderPass
{
private:
	IRenderer* m_pRenderer;
	IShader* m_pShader;
	ConstantsBufferHelper<SObjectConstants> m_Constants;

public:
	GUIShaderPass()
		: m_pRenderer(0)
	{
	}

	virtual ~GUIShaderPass()
	{
		Clear();
	}

	virtual SResult Initialize(IRenderer* pRenderer);
	virtual void ReloadShaders();
	virtual void Clear();

	virtual SResult Bind();
	virtual void SetShaderResources(const SShaderResources& shaderResources, const Mat44& transform);
};

///////////////////////////////////////////////////////////////////////////////////
class S_API PosteffectShaderPass : public IShaderPass
{
public:
	PosteffectShaderPass()
	{
	}

	virtual ~PosteffectShaderPass()
	{
		Clear();
	}

	virtual SResult Initialize(IRenderer* pRenderer);
	virtual void ReloadShaders();
	virtual void Clear();

	virtual SResult Bind();
	virtual void SetShaderResources(const SShaderResources& pShaderResources, const Mat44& transform);
};




SP_NMSPACE_END

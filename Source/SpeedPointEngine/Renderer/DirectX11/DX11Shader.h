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
	virtual void Clear();
	virtual void OnEndFrame();
	virtual SResult Bind();
	virtual void SetShaderResources(const SShaderResources& pShaderResources, const SMatrix4& transform);

	IFBO* GetShadowmap() const;
};




struct S_API SMatObjConstants : SObjectConstants
{
	float matAmbient;		// 4 Byte
	float3 matEmissive;		// 3 * 4 = 12 Byte
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
	virtual void Clear();
	virtual SResult Bind();
	virtual void SetShaderResources(const SShaderResources& pShaderResources, const SMatrix4& transform);

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
	/*
	
	GBUFFER-LAYOUT:
	A - Albedo, N - Normal

	GBUF0 [ Ar | Ag | Ab | Spec ]
	GBUF1 [ Nx | Ny | Nz | ?? ]	

	*/
#define NUM_GBUFFER_LAYERS 2
	vector<IFBO*> m_pGBuffer;

	IShader* m_pShader;
	ConstantsBufferHelper<SMatObjConstants> m_Constants;
	IRenderer* m_pRenderer;

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

	const vector<IFBO*>& GetGBuffer() const;

	// IShaderPass:
	virtual SResult Initialize(IRenderer* pRenderer);
	virtual void Clear();

	virtual SResult Bind();
	virtual void SetShaderResources(const SShaderResources& pShaderResources, const SMatrix4& transform);
};

///////////////////////////////////////////////////////////////////////////////////
class S_API ShadingShaderPass : public IShaderPass
{
public:
	struct SShadingPassConstants : SObjectConstants
	{
	};

private:
	GBufferShaderPass* m_pGBufferPass;
	ShadowmapShaderPass* m_pShadowmapPass;
	IRenderer* m_pRenderer;
	IShader* m_pShader;
	ConstantsBufferHelper<SShadingPassConstants> m_Constants;

public:
	ShadingShaderPass(GBufferShaderPass* pGBufferPass, ShadowmapShaderPass* pShadowmapPass)
		: m_pGBufferPass(pGBufferPass),
		m_pShadowmapPass(pShadowmapPass),
		m_pRenderer(0)
	{
	}

	virtual ~ShadingShaderPass()
	{
		Clear();
	}

	virtual SResult Initialize(IRenderer* pRenderer);
	virtual void Clear();

	virtual SResult Bind();
	virtual void SetShaderResources(const SShaderResources& pShaderResources, const SMatrix4& transform);
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
	virtual void Clear();
	virtual SResult Bind();
	virtual void OnUnbind();
	virtual void SetShaderResources(const SShaderResources& pShaderResources, const SMatrix4& transform);

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
	virtual void Clear();

	virtual SResult Bind();
	virtual void SetShaderResources(const SShaderResources& pShaderResources, const SMatrix4& transform);
};




SP_NMSPACE_END

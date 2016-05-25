//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2011-2014, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SPrerequisites.h>
#include "DirectX11.h"
#include <Abstract\IShader.h>
#include <Abstract\IConstantsBuffer.h>
#include <vector>

using std::vector;

SP_NMSPACE_BEG

struct S_API IGameEngine;
class S_API DirectX11Renderer;
struct S_API IRenderer;
struct S_API IFBO;

// DX11 implementation of a shader (PS + VS)
class S_API DirectX11Shader : public IShader
{
private:
	IGameEngine* m_pEngine;
	DirectX11Renderer* m_pDXRenderer;

	ID3D11InputLayout* m_pVSInputLayout;

	ID3D11PixelShader* m_pPixelShader;
	ID3D11VertexShader* m_pVertexShader;

public:
	DirectX11Shader();
	virtual ~DirectX11Shader();

	virtual SResult Load(IRenderer* pRenderer, const SShaderInfo& info);	
	virtual void Clear();

	virtual SResult Bind();
};



// TODO: GET THIS OUT OF THE DIRECTX11 IMPLEMENTATION PROJECT AND MOVE IT INTO A MORE GENERAL RENDERER

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
		m_pShader(0),
		m_pConstants(0)
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
	IConstantsBuffer<SMatObjConstants>* m_pConstants;
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
	IConstantsBuffer<SMatObjConstants>* m_pConstants;
	IRenderer* m_pRenderer;

public:
	GBufferShaderPass()
		: m_pRenderer(0),
		m_pConstants(0),
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
class S_API ShadowmapShaderPass : public IShaderPass
{
public:
	ShadowmapShaderPass();
	virtual ~ShadowmapShaderPass()
	{
		Clear();
	}

	virtual SResult Initialize(IRenderer* pRenderer);
	virtual void Clear();

	virtual SResult Bind();
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
	IConstantsBuffer<SShadingPassConstants>* m_pConstants;

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
class S_API PosteffectShaderPass : public IShaderPass
{
public:
	PosteffectShaderPass();
	virtual ~PosteffectShaderPass()
	{
		Clear();
	}

	virtual SResult Initialize(IRenderer* pRenderer);
	virtual void Clear();

	virtual SResult Bind();
};




SP_NMSPACE_END
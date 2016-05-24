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
#include <Abstract\IShader.h>
#include "DirectX11.h"

SP_NMSPACE_BEG

struct S_API IGameEngine;
class S_API DirectX11Renderer;
struct S_API IRenderer;

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



// TODO: GET THIS OUT OF THE DIRECTX11 IMPLEMENTATION AND MOVE IT INTO A MORE GENERAL RENDERER

class S_API GBufferShaderPass : public IShaderPass
{
private:
	/*
	
	GBUFFER-LAYOUT:
	A - Albedo, N - Normal

	GBUF0 [ Ar | Ag | Ab | Spec ]
	GBUF1 [ Nx | Ny | Nz | ?? ]	

	*/
#define NUM_GBUFFER_LAYERS 2
	vector<IFBO*> m_pGBuffer;	

	IRenderer* m_pRenderer;

public:
	GBufferShaderPass()
		: m_pRenderer(0)
	{
	}

	virtual ~GBufferShaderPass()
	{
		Clear();
	}

	virtual SResult Initialize(IRenderer* pRenderer);
	virtual void Clear();

	virtual SResult Bind();

	const vector<IFBO*>& GetGBuffer() const;
};

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

class S_API ShadingShaderPass : public IShaderPass
{
private:
	GBufferShaderPass* m_pGBufferPass;
	ShadowmapShaderPass* m_pShadowmapPass;
	IRenderer* m_pRenderer;

public:
	ShadingShaderPass(GBufferShaderPass* pGBufferPass, ShadowmapShaderPass* pShadowmapPass)
		: m_pGBufferPass(pGBufferPass),
		m_pShadowmapPass(pShadowmapPass)
	{
	}

	virtual ~ShadingShaderPass()
	{
		Clear();
	}

	virtual SResult Initialize(IRenderer* pRenderer);
	virtual void Clear();

	virtual SResult Bind();
};


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
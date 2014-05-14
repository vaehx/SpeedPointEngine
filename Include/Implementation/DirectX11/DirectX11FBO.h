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
#include <Abstract\IFBO.h>
#include <d3d11.h>

SP_NMSPACE_BEG

class S_API DirectX11FBO : public IFBO
{
private:
	SpeedPointEngine*	m_pEngine;
	DirectX11Renderer*	m_pDXRenderer;

	ID3D11Texture2D*	m_pTexture;
	ID3D11RenderTargetView* m_pRTV;

	//LPDIRECT3DTEXTURE9	pTexture;
	//LPDIRECT3DSURFACE9	pSurface;

public:
	DirectX11FBO();
	~DirectX11FBO();	

	// initialize with given renderer
	virtual SResult Initialize(EFBOType type, SSpeedPointEngine* pEngine, IRenderer* pRenderer, unsigned int nW, unsigned int nH);
	virtual bool IsInitialized();

	// Clear buffers
	virtual void Clear(void);

	virtual SSpeedPointEngine* GetEngine()
	{
		return m_pEngine;
	}

	virtual IRenderer* GetRenderer()
	{
		return (IRenderer*)m_pDXRenderer;
	}				

	// Getter / setter
	ID3D11RenderTargetView* GetRTV()
	{
		return m_pRTV;
	}
};


SP_NMSPACE_END
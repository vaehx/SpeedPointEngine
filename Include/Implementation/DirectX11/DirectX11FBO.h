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

class S_API SpeedPointEngine;
class S_API DirectX11Renderer;
struct S_API IRenderer;




class S_API DirectX11FBO : public IFBO
{
private:
	SpeedPointEngine* m_pEngine;
	DirectX11Renderer* m_pDXRenderer;

	D3D11_TEXTURE2D_DESC m_texDesc;
	ID3D11Texture2D* m_pTexture;
	ID3D11RenderTargetView* m_pRTV;

	ID3D11DepthStencilView* m_pDepthStencilView;
	ID3D11Texture2D* m_pDepthStencilBuffer;

	ID3D11ShaderResourceView* m_pSRV;

	EFBOType m_FBOType;

	usint32 m_nBufferWidth, m_nBufferHeight;


public:
	DirectX11FBO();
	~DirectX11FBO();	

	// Summary:
	//	Initialize with given renderer
	// Arguments:
	//	nW / nH - (default 0) resolution of the buffer. set to 0 or omit to use FBOType-Default
	virtual SResult Initialize(EFBOType type, SpeedPointEngine* pEngine, IRenderer* pRenderer, unsigned int nW = 0, unsigned int nH = 0);
	
	// Summary:
	//	Mark this FBO to later be used as a texture, so generate the Shader resource view
	SResult InitializeSRV();

	virtual SResult InitializeDSV();

	virtual bool IsInitialized();

	// Clear buffers
	virtual void Clear(void);

	virtual SpeedPointEngine* GetEngine()
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
	void SetRTV(ID3D11RenderTargetView* pRTV)
	{
		SP_ASSERT(pRTV);
		m_pRTV = pRTV;
	}

	ID3D11DepthStencilView* GetDSV() { return m_pDepthStencilView; }

	ID3D11ShaderResourceView* GetSRV() { return m_pSRV; }
	
};




SP_NMSPACE_END
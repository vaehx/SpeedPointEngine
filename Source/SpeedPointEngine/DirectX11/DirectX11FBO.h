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

struct S_API IGameEngine;
class S_API DirectX11Renderer;
struct S_API IRenderer;




class S_API DirectX11FBO : public IFBO
{
private:	
	DirectX11Renderer* m_pDXRenderer;

	D3D11_TEXTURE2D_DESC m_texDesc;
	ID3D11Texture2D* m_pTexture;
	ID3D11RenderTargetView* m_pRTV;

	ID3D11DepthStencilView* m_pDepthStencilView;
	ID3D11Texture2D* m_pDepthStencilBuffer;

	ID3D11ShaderResourceView* m_pSRV;

	EFBOType m_FBOType;

	usint32 m_nBufferWidth, m_nBufferHeight;

	bool m_bSwapChainFBO;


public:
	DirectX11FBO();
	~DirectX11FBO();	

	// Summary:
	//	Initialize with given renderer
	// Arguments:
	//	nW / nH - (default 0) resolution of the buffer. set to 0 or omit to use FBOType-Default
	virtual SResult Initialize(EFBOType type, IRenderer* pRenderer, unsigned int nW = 0, unsigned int nH = 0);
	
	// Summary:
	//	Mark this FBO to later be used as a texture, so generate the Shader resource view
	virtual SResult InitializeSRV();

	virtual SResult InitializeDSV();

	virtual bool IsInitialized();

	// Clear buffers
	virtual void Clear(void);

	virtual IRenderer* GetRenderer()
	{
		return (IRenderer*)m_pDXRenderer;
	}				

	// Getter / setter
	ID3D11RenderTargetView* GetRTV() const { return m_pRTV; }
	ID3D11DepthStencilView* GetDSV() const { return m_pDepthStencilView; }
	ID3D11ShaderResourceView* GetSRV() const { return m_pSRV; }

	// Description:
	//	flags that this FBO only contains the RTV of the swapchain and does not handle a texture on its own
	void FlagSwapchainFBO(ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV)
	{
		m_pRTV = pRTV;
		m_pDepthStencilView = pDSV;
		m_bSwapChainFBO = true;
	}
	
};




SP_NMSPACE_END
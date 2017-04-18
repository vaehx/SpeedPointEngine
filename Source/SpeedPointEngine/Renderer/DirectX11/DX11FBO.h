//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2016 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DX11.h"
#include "..\IFBO.h"
#include <Common\SPrerequisites.h>

SP_NMSPACE_BEG

class S_API DX11Renderer;
struct S_API IRenderer;




class S_API DX11FBO : public IFBO
{
private:	
	DX11Renderer* m_pDXRenderer;

	D3D11_TEXTURE2D_DESC m_texDesc;
	ID3D11Texture2D* m_pTexture;
	ID3D11RenderTargetView* m_pRTV;
	ID3D11ShaderResourceView* m_pSRV;

	D3D11_TEXTURE2D_DESC m_DepthBufferDesc;
	ID3D11Texture2D* m_pDepthBuffer;
	ID3D11DepthStencilView* m_pDSV;
	ID3D11DepthStencilView* m_pReadonlyDSV;
	ID3D11ShaderResourceView* m_pDepthSRV;

	EFBOType m_FBOType;

public:
	DX11FBO();
	~DX11FBO();
	
	virtual SResult Initialize(EFBOType type, IRenderer* pRenderer, unsigned int width, unsigned int height, bool allowAsTexture = false);
	SResult D3D11_InitializeFromCustomResource(ID3D11Resource* pResource, IRenderer* pRenderer, unsigned int width, unsigned height, bool allowAsTexture = false);
	virtual SResult InitializeDepthBuffer(bool allowAsTexture = false);
	virtual bool IsInitialized() const;

	// Clear buffers
	virtual void Clear(void);

	virtual IRenderer* GetRenderer()
	{
		return (IRenderer*)m_pDXRenderer;
	}				

	// Getter / setter
	ID3D11RenderTargetView* GetRTV() const { return m_pRTV; }
	ID3D11DepthStencilView* GetDSV() const { return m_pDSV; }
	ID3D11DepthStencilView* GetReadonlyDSV() const { return m_pReadonlyDSV; }
	ID3D11ShaderResourceView* GetSRV() const { return m_pSRV; }
	ID3D11ShaderResourceView* GetDepthBufferSRV() const { return m_pDepthSRV; }
	
};




SP_NMSPACE_END

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
class S_API DX11Texture;
struct S_API IRenderer;


class S_API DX11FBO : public IFBO
{
private:	
	DX11Renderer* m_pDXRenderer;

	D3D11_TEXTURE2D_DESC m_texDesc;
	ID3D11Texture2D* m_pFrameBuffer;
	ID3D11RenderTargetView* m_pRTV;
	DX11Texture* m_pFrameBufferTexture;

	D3D11_TEXTURE2D_DESC m_DepthBufferDesc;
	ID3D11Texture2D* m_pDepthBuffer;
	ID3D11DepthStencilView* m_pDSV;
	ID3D11DepthStencilView* m_pReadonlyDSV;
	DX11Texture* m_pDepthBufferTexture;

	EFBOType m_FBOType;
	SColor m_ClearColor;

	SResult InitializeDepthBufferIntrnl(bool allowAsTexture, const string& specification, unsigned int resolution[2] = 0);

public:
	DX11FBO(DX11Renderer* pRenderer);
	~DX11FBO();
	
	virtual SResult Initialize(EFBOType type, unsigned int width, unsigned int height);
	virtual SResult InitializeAsTexture(EFBOType type, unsigned int width, unsigned int height, const string& specification);
	SResult D3D11_InitializeFromCustomResource(ID3D11Resource* pResource, unsigned int width, unsigned height, bool allowAsTexture = false, const string& specification = "");
	
	virtual SResult InitializeDepthBuffer(unsigned int resolution[2] = 0);
	virtual SResult InitializeDepthBufferAsTexture(const string& specification, unsigned int resolution[2] = 0);

	virtual bool IsInitialized() const;

	// Clear buffers
	virtual void Clear(void);

	virtual IRenderer* GetRenderer()
	{
		return (IRenderer*)m_pDXRenderer;
	}

	virtual ITexture* GetTexture() const;
	virtual ITexture* GetDepthBufferTexture() const;

	virtual void SetClearColor(const SColor& color) { m_ClearColor = color; }
	virtual const SColor& GetClearColor() const { return m_ClearColor; }

	// Getter / setter
	ID3D11RenderTargetView* GetRTV() const { return m_pRTV; }
	ID3D11DepthStencilView* GetDSV() const { return m_pDSV; }
	ID3D11DepthStencilView* GetReadonlyDSV() const { return m_pReadonlyDSV; }
	ID3D11ShaderResourceView* GetSRV() const;
	ID3D11ShaderResourceView* GetDepthSRV() const;
};




SP_NMSPACE_END

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Engine.
//	Copyright (c) 2011-2015 Pascal Rosenkranz
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Abstract\IFont.h>
#include "DirectX11Renderer.h"
#include <d3d10_1.h>
#include <dxgi.h>
#include <d2d1.h>
#include <dwrite.h>

SP_NMSPACE_BEG

class S_API DirectX11FontRenderer : public IFontRenderer
{
private:
	DirectX11Renderer* m_pDXRenderer; // D3D11 Renderer

	ID3D10Device1* m_pD3D101Device;	

	IDXGIKeyedMutex* m_pKeyedMutex11;
	IDXGIKeyedMutex* m_pKeyedMutex10;

	ID2D1RenderTarget* m_pD2DRenderTarget;
	ID2D1SolidColorBrush* m_pD2DBrush;
	
	ID3D11Texture2D* m_pBackBuffer;
	ID3D11Texture2D* m_pSharedTexture;

	ID3D11Buffer* m_pD2DVtxBuffer;
	ID3D11Buffer* m_pD2DIndexBuffer;

	ID3D11ShaderResourceView* m_pD2DTexSRV; // d2dTexture

	IDWriteFactory* m_pDWriteFactory;
	IDWriteTextFormat* m_pTextFormat;
	IDWriteTextFormat* m_pBoldTextFormat;

	D3D11_BLEND_DESC m_BlendDesc;
	ID3D11BlendState* m_pBlendState;

	unsigned int m_TexSz[2];

	DirectX11Effect m_FontShader;

	ID3D11Buffer* m_pConstantsBuffer;



	SResult InitD2DScreenTexture();
	void InitBlendDesc();
	void InitConstantsBuffer();

public:
	DirectX11FontRenderer()
		: m_pDXRenderer(0),
		m_pD3D101Device(0),
		m_pKeyedMutex10(0),
		m_pKeyedMutex11(0),
		m_pD2DRenderTarget(0),
		m_pD2DBrush(0),
		m_pBackBuffer(0),
		m_pSharedTexture(0),
		m_pD2DVtxBuffer(0),
		m_pD2DIndexBuffer(0),
		m_pD2DTexSRV(0),
		m_pDWriteFactory(0),
		m_pTextFormat(0),
		m_pBlendState(0)
	{
	}

	virtual ~DirectX11FontRenderer()
	{
		Clear();
	}

	virtual SResult Init(IRenderer* pRenderer);

	virtual void BeginRender();
	virtual void RenderText(const char* text, const SColor& color, const SPixelPosition& pixelPos, bool alignRight = false);
	virtual void EndRender();

	virtual void Clear();
};

SP_NMSPACE_END
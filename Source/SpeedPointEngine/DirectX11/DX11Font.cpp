//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2016 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "DX11Font.h"
#include <Abstract\SVertex.h>
#include <Abstract\SColor.h>
#include <Abstract\IIndexBuffer.h>

SP_NMSPACE_BEG

// -----------------------------------------------------------------------------------------------
S_API SResult DX11FontRenderer::Init(IRenderer* pRenderer)
{
	if (!IS_VALID_PTR(pRenderer) || pRenderer->GetType() != S_DIRECTX11)
		return S_INVALIDPARAM;

	m_pDXRenderer = dynamic_cast<DX11Renderer*>(pRenderer);
	IDXGIAdapter1* pAdapter = m_pDXRenderer->GetAutoSelectedAdapter();
	ID3D11Device* pD3D11Device = m_pDXRenderer->GetD3D11Device();

	HRESULT hr;

	hr = D3D10CreateDevice1(pAdapter, D3D10_DRIVER_TYPE_HARDWARE, 0, D3D10_CREATE_DEVICE_DEBUG | D3D10_CREATE_DEVICE_BGRA_SUPPORT,
		D3D10_FEATURE_LEVEL_9_3, D3D10_1_SDK_VERSION, &m_pD3D101Device);

	D3D11_TEXTURE2D_DESC sharedTexDsc;
	ZeroMemory(&sharedTexDsc, sizeof(D3D11_TEXTURE2D_DESC));

	SIZE vpsz = pRenderer->GetTargetViewport()->GetSize();
	sharedTexDsc.Width = vpsz.cx;
	sharedTexDsc.Height = vpsz.cy;
	m_TexSz[0] = vpsz.cx;
	m_TexSz[1] = vpsz.cy;

	sharedTexDsc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sharedTexDsc.MipLevels = 1;
	sharedTexDsc.ArraySize = 1;
	sharedTexDsc.SampleDesc.Count = 1;
	sharedTexDsc.Usage = D3D11_USAGE_DEFAULT;
	sharedTexDsc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	sharedTexDsc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;

	hr = pD3D11Device->CreateTexture2D(&sharedTexDsc, 0, &m_pSharedTexture);

	// Get mutex
	hr = m_pSharedTexture->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&m_pKeyedMutex11);

	// Get shared handle
	IDXGIResource* pSharedResource;
	HANDLE sharedHandle;

	hr = m_pSharedTexture->QueryInterface(__uuidof(IDXGIResource), (void**)&pSharedResource);
	hr = pSharedResource->GetSharedHandle(&sharedHandle);

	pSharedResource->Release();

	// let d3d10 open the shared resource as surface
	IDXGISurface1* pSharedSurface;

	hr = m_pD3D101Device->OpenSharedResource(sharedHandle, __uuidof(IDXGISurface1), (void**)(&pSharedSurface));
	hr = pSharedSurface->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&m_pKeyedMutex10);



	// Create D2D DXGI Surface Render Target using D2D Factory
	ID2D1Factory *pD2DFactory;	
	D2D1_FACTORY_OPTIONS options;
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, options, &pD2DFactory);

	D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties;
	ZeroMemory(&renderTargetProperties, sizeof(D2D1_RENDER_TARGET_PROPERTIES));
	renderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
	renderTargetProperties.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);

	hr = pD2DFactory->CreateDxgiSurfaceRenderTarget(pSharedSurface, renderTargetProperties, &m_pD2DRenderTarget);

	m_pD2DRenderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

	pSharedSurface->Release();
	pD2DFactory->Release();


	// Create the solid brush
	hr = m_pD2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), &m_pD2DBrush);		




	// Initialize DirectWrite
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&m_pDWriteFactory));
	
	// Regular TextFormat
	hr = m_pDWriteFactory->CreateTextFormat(L"Script", 0, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL, 13.0f, L"", &m_pTextFormat);

	hr = m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	hr = m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);	

	// Bold Text format
	hr = m_pDWriteFactory->CreateTextFormat(L"Script", 0, DWRITE_FONT_WEIGHT_BLACK, DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL, 13.0f, L"", &m_pBoldTextFormat);		

	hr = m_pBoldTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	hr = m_pBoldTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);


	// Medium Text Format
	hr = m_pDWriteFactory->CreateTextFormat(L"Script", 0, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL, 18.0f, L"", &m_pMediumTextFormat);

	hr = m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	hr = m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	// Large Text Format
	hr = m_pDWriteFactory->CreateTextFormat(L"Script", 0, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL, 26.0f, L"", &m_pLargeTextFormat);

	hr = m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	hr = m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);


	// suppress no-topology-set warning
	m_pD3D101Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);

	if (Failure(hr))
		return S_ERROR;

	SResult res;
	
	res = InitD2DScreenTexture();
	
	InitBlendDesc();
	
	string fontFXFile = m_pDXRenderer->GetEngine()->GetShaderPath(eSHADERFILE_FONT);
	m_FontShader.Load(m_pDXRenderer, SShaderInfo(fontFXFile, "font"));

	InitConstantsBuffer();

	return res;
}


// -----------------------------------------------------------------------------------------------
S_API SResult DX11FontRenderer::InitD2DScreenTexture()
{
	HRESULT hr;
	ID3D11Device* pD3D11Device = m_pDXRenderer->GetD3D11Device();	

	SVertex pVertices[] = 
	{
		SVertex(1.0f, -1.0f, 1.0f, 0, 0, -1.0f, 1.0f, 0, 0, 1.0f, 1.0f),	// right bottom
		SVertex(1.0f, 1.0f, 1.0f, 0, 0, -1.0f, 1.0f, 0, 0, 1.0f, 0.0f),		// right top
		SVertex(-1.0f, 1.0f, 1.0f, 0, 0, -1.0f, 1.0f, 0, 0, 0.0f, 0.0f),	// left top
		SVertex(-1.0f, -1.0f, 1.0f, 0, 0, -1.0f, 1.0f, 0, 0, 0.0f, 1.0f)	// left bottom
	};

	SIndex pIndices[] =
	{
		0, 2, 3,
		0, 1, 2
	};
	
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));

	D3D11_BUFFER_DESC bufferDsc;
	ZeroMemory(&bufferDsc, sizeof(D3D11_BUFFER_DESC));
	bufferDsc.Usage = D3D11_USAGE_DEFAULT;	
	bufferDsc.CPUAccessFlags = 0;
	bufferDsc.MiscFlags = 0;

	// Create Index Buffer
	bufferDsc.ByteWidth = sizeof(SIndex) * 6;
	bufferDsc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		
	initData.pSysMem = pIndices;
	hr = pD3D11Device->CreateBuffer(&bufferDsc, &initData, &m_pD2DIndexBuffer);

	// Create Vertex buffer
	bufferDsc.ByteWidth = sizeof(SVertex) * 4;
	bufferDsc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	initData.pSysMem = pVertices;
	hr = pD3D11Device->CreateBuffer(&bufferDsc, &initData, &m_pD2DVtxBuffer);


	// Create SRV for shared texture to use as texture for the fullscreen square
	hr = pD3D11Device->CreateShaderResourceView(m_pSharedTexture, 0, &m_pD2DTexSRV);

	return SUCCEEDED(hr) ? S_SUCCESS : S_ERROR;
}

// -----------------------------------------------------------------------------------------------
S_API void DX11FontRenderer::InitBlendDesc()
{
	ZeroMemory(&m_BlendDesc, sizeof(m_BlendDesc));

	m_BlendDesc.AlphaToCoverageEnable = false;
	m_BlendDesc.IndependentBlendEnable = false;
	m_BlendDesc.RenderTarget[0].BlendEnable = true;
	m_BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	m_BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	m_BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	m_BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	m_BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	m_BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	m_BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	ID3D11Device* pD3D11Device = m_pDXRenderer->GetD3D11Device();
	HRESULT hr = pD3D11Device->CreateBlendState(&m_BlendDesc, &m_pBlendState);
}

// -----------------------------------------------------------------------------------------------
S_API void DX11FontRenderer::InitConstantsBuffer()
{
	m_pDXRenderer->D3D11_CreateConstantsBuffer(&m_pConstantsBuffer, sizeof(SFontConstantsBuffer));
	
	// NOTE TO ALWAYS REFILL THE CONSTANTS BUFFER WHENEVER THE SCREEN RESOLUTION CHANGES!

	// Fill with initial data.
	SFontConstantsBuffer cb;
	cb.screenResolution[0] = m_TexSz[0];
	cb.screenResolution[1] = m_TexSz[1];

	SFontConstantsBuffer* pCB;
	m_pDXRenderer->D3D11_LockConstantsBuffer(m_pConstantsBuffer, (void**)&pCB);
	memcpy(pCB, &cb, sizeof(SFontConstantsBuffer));
	m_pDXRenderer->D3D11_UnlockConstantsBuffer(m_pConstantsBuffer);
}

// -----------------------------------------------------------------------------------------------
S_API void DX11FontRenderer::BeginRender()
{
	if (m_SkipFrameCounter >= 1 && m_SkipFrameCounter <= m_nKeepFrames)
		return; // iteration is done in EndRender()

	m_pKeyedMutex11->ReleaseSync(0);
	m_pKeyedMutex10->AcquireSync(0, 5);

	m_pD2DRenderTarget->BeginDraw();
	m_pD2DRenderTarget->Clear(D2D1::ColorF(0, 0, 0, 0.0f));
}

// -----------------------------------------------------------------------------------------------
S_API void DX11FontRenderer::RenderText(const string& text, const SColor& color, const SPixelPosition& pixelPos,
	EFontSize fontSize /*=eFONTSIZE_NORMAL*/, bool alignRight /*=false*/)
{
	if (m_SkipFrameCounter >= 1 && m_SkipFrameCounter <= m_nKeepFrames)
		return; // iteration is done in EndRender()

	unsigned int screenPadding[2];
	screenPadding[0] = 6;
	screenPadding[1] = 5;

	D2D1_RECT_F drawRect = D2D1::RectF(
		(float)(pixelPos.x + screenPadding[0]), (float)(pixelPos.y + screenPadding[1]),
		(float)(m_TexSz[0] - pixelPos.x - screenPadding[0]), (float)(m_TexSz[1] - pixelPos.y - screenPadding[1]));	

	// Choose the correct text format
	IDWriteTextFormat* pUsedTextFormat;
	switch (fontSize)
	{
	case eFONTSIZE_NORMAL: pUsedTextFormat = m_pTextFormat; break;
	case eFONTSIZE_MEDIUM: pUsedTextFormat = m_pMediumTextFormat; break;
	case eFONTSIZE_LARGE: pUsedTextFormat = m_pLargeTextFormat; break;
	default:
		pUsedTextFormat = m_pTextFormat;
	}

	// Right-Align the text if necessary
	if (alignRight)
		pUsedTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	else
		pUsedTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);


	// Convert multibyte string to wide string
	unsigned int wbufsz = text.length() + 1;
	wchar_t* wtext = new wchar_t[wbufsz];
	MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, wtext, wbufsz);

	// Create layout
	IDWriteTextLayout* pTextLayout;
	m_pDWriteFactory->CreateTextLayout(wtext, wbufsz, pUsedTextFormat, drawRect.right - drawRect.left, drawRect.bottom - drawRect.top, &pTextLayout);		

	// Draw the shadow	
	D2D1_COLOR_F fontColor = D2D1::ColorF(0.01f, 0.01f, 0.01f, 1.0f);
	m_pD2DBrush->SetColor(fontColor);
	m_pD2DBrush->SetOpacity(1.0f);		

	D2D1_RECT_F shadowDrawRect = drawRect;
	for (unsigned short i = 0; i < 2; ++i)
	{
		float shadowOffset = 1.0f;
		shadowDrawRect.left += shadowOffset;
		shadowDrawRect.top += shadowOffset;
		shadowDrawRect.bottom += shadowOffset;
		shadowDrawRect.right += shadowOffset;		
		m_pD2DRenderTarget->DrawTextLayout(D2D1::Point2F(shadowDrawRect.left, shadowDrawRect.top), pTextLayout, m_pD2DBrush, D2D1_DRAW_TEXT_OPTIONS_NONE);
	}

	// Draw the Text
	fontColor = D2D1::ColorF(color.r, color.g, color.b, 1.0f);
	m_pD2DBrush->SetColor(fontColor);
	m_pD2DRenderTarget->DrawTextLayout(D2D1::Point2F(shadowDrawRect.left, shadowDrawRect.top), pTextLayout, m_pD2DBrush, D2D1_DRAW_TEXT_OPTIONS_NONE);

	pTextLayout->Release();


	// Free multibyte string
	delete[] wtext;		
}

// -----------------------------------------------------------------------------------------------
S_API void DX11FontRenderer::EndRender()
{
	if (m_SkipFrameCounter == 0)
	{
		m_pD2DRenderTarget->EndDraw();

		m_pKeyedMutex10->ReleaseSync(1);
		m_pKeyedMutex11->AcquireSync(1, 5);
	}

	if (m_SkipFrameCounter == m_nKeepFrames)
		m_SkipFrameCounter = 0;
	else
		++m_SkipFrameCounter;


	ID3D11DeviceContext* pD3D11DeviceContext = m_pDXRenderer->GetD3D11DeviceContext();
	m_FontShader.Bind();
	pD3D11DeviceContext->OMSetBlendState(m_pBlendState, 0, 0xffffffff);
	pD3D11DeviceContext->IASetIndexBuffer(m_pD2DIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	unsigned int stride = sizeof(SVertex);
	unsigned int offset = 0;
	pD3D11DeviceContext->IASetVertexBuffers(0, 1, &m_pD2DVtxBuffer, &stride, &offset);

	pD3D11DeviceContext->VSSetConstantBuffers(0, 0, 0);
	//pD3D11DeviceContext->PSSetConstantBuffers(0, 1, &m_pConstantsBuffer);
	pD3D11DeviceContext->PSSetShaderResources(0, 1, &m_pD2DTexSRV);

	pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pD3D11DeviceContext->DrawIndexed(6, 0, 0);

	//pD3D11DeviceContext->PSSetConstantBuffers(0, 0, 0);
}

// -----------------------------------------------------------------------------------------------
S_API void DX11FontRenderer::Clear()
{
	SP_SAFE_RELEASE(m_pConstantsBuffer);
	SP_SAFE_RELEASE(m_pD2DVtxBuffer);
	SP_SAFE_RELEASE(m_pD2DIndexBuffer);
	SP_SAFE_RELEASE(m_pD3D101Device);
	SP_SAFE_RELEASE(m_pKeyedMutex11);
	SP_SAFE_RELEASE(m_pKeyedMutex10);
	SP_SAFE_RELEASE(m_pD2DRenderTarget);
	SP_SAFE_RELEASE(m_pD2DBrush);
	SP_SAFE_RELEASE(m_pBackBuffer);
	SP_SAFE_RELEASE(m_pSharedTexture);
	SP_SAFE_RELEASE(m_pDWriteFactory);
	SP_SAFE_RELEASE(m_pTextFormat);
	SP_SAFE_RELEASE(m_pBoldTextFormat);
	SP_SAFE_RELEASE(m_pMediumTextFormat);
	SP_SAFE_RELEASE(m_pLargeTextFormat);
	SP_SAFE_RELEASE(m_pD2DTexSRV);
	SP_SAFE_RELEASE(m_pBlendState);
	m_FontShader.Clear();
	m_pDXRenderer = 0;
}



SP_NMSPACE_END

// ********************************************************************************************

//	This file is part of the SpeedPoint Game Engine

//	(c) 2011-2014 Pascal R. aka iSmokiieZz
//	All rights reserved.

// ********************************************************************************************

#include <Implementation\DirectX9\SDirectX9Shader.h>
#include <Implementation\DirectX9\SDirectX9Renderer.h>
#include <SpeedPoint.h>
#include <SSpeedPointEngine.h>

namespace SpeedPoint
{
	// **********************************************************************************

	S_API SResult SDirectX9Shader::Initialize(SpeedPointEngine* engine, char* cFilename)
	{
		Clear();

		if (engine == 0 || cFilename == 0) return S_ABORTED;

		m_pEngine = engine;
		SDirectX9Renderer* pDXRenderer = (SDirectX9Renderer*)m_pEngine->GetRenderer();

		// Create Effect
//~~~~~~~~~~~~
// TODO: Setup custom compilation and catch errors and throw them into the Logging Stream!
		LPD3DXBUFFER pErrorBuffer = 0;
		if (FAILED(D3DXCreateEffectFromFileA(pDXRenderer->pd3dDevice, cFilename, 0, 0, D3DXSHADER_DEBUG, 0, &m_pEffect, &pErrorBuffer)))
		{
#ifdef _DEBUG
			char* cErrorText = (char*)pErrorBuffer->GetBufferPointer();
			MessageBoxA(0, cErrorText, "Shader Error", MB_ICONERROR | MB_OK);
#endif
			return S_ERROR;
		}
//~~~~~~~~~~~~

		// Set first technique we can find
		if (FAILED(m_pEffect->SetTechnique(m_pEffect->GetTechnique(0))))
		{
			return S_ERROR;
		}

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API bool SDirectX9Shader::IsInitialized()
	{
		return (m_pEngine != 0 && m_pEffect != 0);
	}

	// **********************************************************************************

	S_API SResult SDirectX9Shader::SetTechnique(char* cTechnique)
	{
		if (m_pEffect == 0) return S_ABORTED;

		if (FAILED(m_pEffect->SetTechnique(cTechnique)))
		{
			return S_ERROR;
		}

		return S_SUCCESS;
	}

	// **********************************************************************************
	
	S_API SResult SDirectX9Shader::Clear(void)
	{
		if (m_pEffect) m_pEffect->Release(); m_pEffect = 0;
		m_pEngine = 0;

		return S_SUCCESS;
	}

}
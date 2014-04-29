// ********************************************************************************************

//	This file is part of the SpeedPoint Game Engine

//	(c) 2011-2014 Pascal R. aka iSmokiieZz
//	All rights reserved.

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\IShader.h>
#include <d3dx9.h>

namespace SpeedPoint
{
	// DirectX9-Specific implementation of the Shader interface
	class S_API SDirectX9Shader : public IShader
	{
	private:
		SpeedPointEngine*	m_pEngine;

//~~~~~~~~~~~~~~~


// TODO: REWRITE THIS TO USE D3D-API SHADERS INSTEAD OF D3DX CRAP!!!!!
		
		
		LPD3DXEFFECT		m_pEffect;


//~~~~~~~~~~~~~~~~

	public:		

		// Default constructor
		SDirectX9Shader()
			: m_pEngine(0),
			m_pEffect(0)
		{
		}

		// Default destructor
		~SDirectX9Shader()
		{
			Clear();
		}

		// Copy constructor
		SDirectX9Shader(const SDirectX9Shader& o)
			: m_pEngine(o.m_pEngine),
			m_pEffect(o.m_pEffect)
		{
		}

		// Initialize this shader with engine and load shader data from given file
		// It will also compile the shader to HLSL
		virtual SResult Initialize(SpeedPointEngine* pEngine, char* cFilename);

		// check if the shader is initialized
		virtual bool IsInitialized();

		// Set current technique
		virtual SResult SetTechnique(char* cTechnique);

		// Clearout this shader
		virtual SResult Clear(void);

	// Getter / Setter
	public:
		SpeedPointEngine* GetEngine() { return m_pEngine; }
		LPD3DXEFFECT GetEffect() { return m_pEffect; }
	};
}
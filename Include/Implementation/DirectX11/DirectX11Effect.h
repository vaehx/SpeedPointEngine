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

class S_API SpeedPointEngine;
class S_API DirectX11Renderer;
struct S_API IRenderer;

class S_API DirectX11Effect : public IShader
{
private:
	SpeedPointEngine* m_pEngine;
	DirectX11Renderer* m_pDXRenderer;

	ID3D11InputLayout* m_pVSInputLayout;

	ID3D11PixelShader* m_pPixelShader;
	ID3D11VertexShader* m_pVertexShader;

	//ID3D11DomainShader* m_pDomainShader;
	//ID3D11HullShader* m_pHullShader;
	//ID3D11GeometryShader* m_pGeomShader;

public:
	DirectX11Effect();
	~DirectX11Effect();

	virtual SResult Initialize(SpeedPointEngine* pEngine, char* cFilename);	
	virtual bool IsInitialized();
	virtual SResult Clear(void);

	virtual SResult Enable();

	virtual SResult SetTechnique(char* cTechnique);	
};


// . . .


SP_NMSPACE_END
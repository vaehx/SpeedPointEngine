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
#include <Abstract\IOutputPlane.h>

SP_NMSPACE_BEG

class DirectX11Renderer;
class IVertexBuffer;
class IIndexBuffer;
class DirectX11Effect;



//////////////////////////////////////////////////////////////////////////////////////////////////


class DirectX11OutputPlane : public IOutputPlane
{
private:
	SpeedPointEngine* m_pEngine;
	DirectX11Renderer* m_pDXRenderer;
	IVertexBuffer* m_pVertexBuffer;
	IIndexBuffer* m_pIndexBuffer;

	SMatrix m_mtxProjection;
	SMatrix m_mtxView;

	// Default constructor
	DirectX11OutputPlane()
		: m_pEngine(0),
		m_pDXRenderer(0),
		m_pVertexBuffer(0),
		m_pIndexBuffer(0)
	{
	}

	~DirectX11OutputPlane()
	{		
		Clear();
	}

public:

	// Initialize the output plane
	virtual SResult Initialize(SpeedPointEngine* eng, IRenderer* renderer, int nW, int nH);

	virtual bool IsInitialized();

	// Render the output plane using the renderer specified when initializing
	virtual SResult Render(IFBO* pGBufferAlbedo, IFBO* pLightingBuffer);

	// Clearout the output plane
	virtual SResult Clear(void);
};




//////////////////////////////////////////////////////////////////////////////////////////////////

SP_NMSPACE_END
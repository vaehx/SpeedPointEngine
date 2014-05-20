//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2011-2014, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////


#pragma once

#include <Abstract\IVertexBuffer.h>
#include "DirectX11.h"

SP_NMSPACE_BEG

// SpeedPoint VertexBuffer Resource
class S_API DirectX11VertexBuffer : public IVertexBuffer
{
private:
	SpeedPointEngine*		m_pEngine;
	IRenderer*			m_pRenderer;
	ID3D11Buffer*			m_pHWVertexBuffer;
	SVertex*			m_pShadowBuffer;
	int				m_nVertices;
	int				m_nVerticesWritten;
	bool				m_bDynamic;
	bool				m_bLocked;


public:	
	DirectX11VertexBuffer();	
	DirectX11VertexBuffer(const DirectX11VertexBuffer& o);		
	~DirectX11VertexBuffer();

	// Initialize the vertex buffer
	SResult Initialize(int nSize, bool bDynamic, SpeedPointEngine* pEng, IRenderer* renderer);

	// Create the Hardware Vertex Buffer
	SResult Create(int nSize, bool bDynamic_);

	// Check if this Vertex Buffer is inited properly
	BOOL IsInited(void);

	// Change the size of the Hardware Vertex Buffer
	SResult Resize(int nNewSize);

	// Lock the Hardware Vertex Buffer in order to be able to fill Hardware data
	SResult Lock(UINT iBegin, UINT iLength, SVertex** buf, EVBLockType lockType);
	SResult Lock(UINT iBegin, UINT iLength, SVertex** buf);

	// Fill the Hardware Vertex Buffer with an array of vertices
	SResult Fill(SVertex* pVertices, int nVertices, bool append);

	// Unlock the Hardware Vertex Buffer
	SResult Unlock(void);

	// Get the RAM Copy of the hardware Vertex Buffer
	SVertex* GetShadowBuffer(void);

	// Get a Pointer to an Vertex
	SVertex* GetVertex(int iVertex);

	// Get the total count of all vertices
	INT GetVertexCount(void);

	// Clear everything and free memory
	SResult Clear(void);
};


SP_NMSPACE_END
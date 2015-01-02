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

struct S_API IGameEngine;


struct S_API SDirectX11VBCreateFlags
{
	D3D11_USAGE usage;
	unsigned int bindFlags;
	unsigned int cpuAccessFlags;

	SDirectX11VBCreateFlags()
		: usage(D3D11_USAGE_DEFAULT),
		bindFlags(D3D11_BIND_VERTEX_BUFFER),
		cpuAccessFlags(0)
	{
	}

	SDirectX11VBCreateFlags(const SDirectX11VBCreateFlags& o)
		: usage(o.usage),
		bindFlags(o.bindFlags),
		cpuAccessFlags(o.cpuAccessFlags)
	{
	}

	SDirectX11VBCreateFlags(D3D11_USAGE pUsage, unsigned int pBindFlags, unsigned int pCpuAccessFlags)
		: usage(pUsage),
		bindFlags(pBindFlags),
		cpuAccessFlags(pCpuAccessFlags)
	{
	}
};


// SpeedPoint VertexBuffer Resource
class S_API DirectX11VertexBuffer : public IVertexBuffer
{
private:
	IGameEngine*			m_pEngine;	// used for logging!
	IRenderer*			m_pRenderer;

	ID3D11Buffer*			m_pHWVertexBuffer;
	
	SVertex*			m_pShadowBuffer;
	unsigned long			m_nVertices;
	EVBUsage			m_Usage;	


public:	
	DirectX11VertexBuffer();	
	DirectX11VertexBuffer(const DirectX11VertexBuffer& o);		
	~DirectX11VertexBuffer();
		
	virtual SResult Initialize(IGameEngine* pEngine, IRenderer* renderer, EVBUsage usage,
		const SVertex* pInitialData = nullptr, const unsigned long nInitialVertices = 0);

	SResult Create(const SVertex* pInitialData, const unsigned long nInitialDataCount);
	
	virtual bool IsInited(void);
	virtual SResult Fill(const SVertex* pVertices, const unsigned long nVertices);

	virtual SResult UploadVertexData(unsigned long iVtxStart = 0, unsigned long nVertices = 0);

	virtual SVertex* GetShadowBuffer(void);	
	virtual SVertex* GetVertex(unsigned long iVertex);	
	virtual unsigned long GetVertexCount(void) const;

	virtual SResult Clear(void);



	// DX
	ID3D11Buffer* D3D11_GetBuffer()
	{
		return m_pHWVertexBuffer;
	}

	SDirectX11VBCreateFlags GetCreateFlags();
};


SP_NMSPACE_END
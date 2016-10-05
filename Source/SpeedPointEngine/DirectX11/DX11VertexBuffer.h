//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2016 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Abstract\IVertexBuffer.h>
#include "DX11.h"

SP_NMSPACE_BEG

struct S_API IGameEngine;


struct S_API SDX11VBCreateFlags
{
	D3D11_USAGE usage;
	unsigned int bindFlags;
	unsigned int cpuAccessFlags;

	SDX11VBCreateFlags()
		: usage(D3D11_USAGE_DEFAULT),
		bindFlags(D3D11_BIND_VERTEX_BUFFER),
		cpuAccessFlags(0)
	{
	}

	SDX11VBCreateFlags(const SDX11VBCreateFlags& o)
		: usage(o.usage),
		bindFlags(o.bindFlags),
		cpuAccessFlags(o.cpuAccessFlags)
	{
	}

	SDX11VBCreateFlags(D3D11_USAGE pUsage, unsigned int pBindFlags, unsigned int pCpuAccessFlags)
		: usage(pUsage),
		bindFlags(pBindFlags),
		cpuAccessFlags(pCpuAccessFlags)
	{
	}
};


// SpeedPoint VertexBuffer Resource
class S_API DX11VertexBuffer : public IVertexBuffer
{
private:	
	IRenderer*			m_pRenderer;

	ID3D11Buffer*			m_pHWVertexBuffer;
	
	SVertex*			m_pShadowBuffer;
	unsigned long			m_nVertices;
	EVBUsage			m_Usage;	


public:	
	DX11VertexBuffer();	
	DX11VertexBuffer(const DX11VertexBuffer& o);		
	~DX11VertexBuffer();
		
	virtual SResult Initialize(IRenderer* renderer, EVBUsage usage,
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

	SDX11VBCreateFlags GetCreateFlags();
};


SP_NMSPACE_END

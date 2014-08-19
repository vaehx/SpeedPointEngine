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
	int				m_nVertices;
	int				m_nVerticesWritten;
	bool				m_bLocked;
	EVBUsage			m_Usage;
	bool				m_bInitialDataWritten;


public:	
	DirectX11VertexBuffer();	
	DirectX11VertexBuffer(const DirectX11VertexBuffer& o);		
	~DirectX11VertexBuffer();
	
	// Arguments:
	//	nSize - spezifies the vertex count of pInitialData if set
	virtual SResult Initialize(IGameEngine* pEngine, IRenderer* renderer, EVBUsage usage, int nSize, SVertex* pInitialData = nullptr);

	// Arguments:
	//	nSize - spezifies the count of pInitialData if set
	virtual SResult Create(int nSize, SVertex* pInitialData = nullptr, usint32 nInitialDataCount = 0);
	
	virtual BOOL IsInited(void);	
	virtual SResult Resize(int nNewSize);	
	virtual SResult Lock(UINT iBegin, UINT iLength, SVertex** buf, EVBLockType lockType);
	virtual SResult Lock(UINT iBegin, UINT iLength, SVertex** buf);	
	virtual SResult Fill(SVertex* pVertices, int nVertices, bool append);	
	virtual SResult Unlock(void);	
	virtual SVertex* GetShadowBuffer(void);	
	virtual SVertex* GetVertex(int iVertex);	
	virtual INT GetVertexCount(void);	
	virtual SResult Clear(void);



	// DX
	ID3D11Buffer* D3D11_GetBuffer()
	{
		return m_pHWVertexBuffer;
	}

	SDirectX11VBCreateFlags GetCreateFlags();
};


SP_NMSPACE_END
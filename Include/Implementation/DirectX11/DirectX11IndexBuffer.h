//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2011-2014, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////


#pragma once

#include <Abstract\IIndexBuffer.h>
#include "DirectX11.h"

SP_NMSPACE_BEG

struct S_API IGameEngine;
struct S_API IRenderer;

struct S_API SDirectX11IBCreateFlags
{
	D3D11_USAGE usage;
	unsigned int bindFlags;
	unsigned int cpuAccessFlags;

	SDirectX11IBCreateFlags()
		: usage(D3D11_USAGE_DEFAULT),
		bindFlags(D3D11_BIND_VERTEX_BUFFER),
		cpuAccessFlags(0)
	{
	}

	SDirectX11IBCreateFlags(const SDirectX11IBCreateFlags& o)
		: usage(o.usage),
		bindFlags(o.bindFlags),
		cpuAccessFlags(o.cpuAccessFlags)
	{
	}

	SDirectX11IBCreateFlags(D3D11_USAGE pUsage, unsigned int pBindFlags, unsigned int pCpuAccessFlags)
		: usage(pUsage),
		bindFlags(pBindFlags),
		cpuAccessFlags(pCpuAccessFlags)
	{
	}
};


// SpeedPoint IndexBuffer Resource
class S_API DirectX11IndexBuffer : public IIndexBuffer
{
private:
	IGameEngine*		m_pEngine;
	IRenderer*			m_pRenderer;
	ID3D11Buffer*			m_pHWIndexBuffer;
	SIndex*			m_pShadowBuffer;
	int				m_nIndices;
	int				m_nIndicesWritten;
	EIBUsage			m_Usage;
	bool				m_bLocked;
	bool				m_bInitialDataWritten;


public:
	DirectX11IndexBuffer();
	DirectX11IndexBuffer(const DirectX11IndexBuffer& o);
	~DirectX11IndexBuffer();
	
	virtual SResult Initialize(IGameEngine* pEngine, IRenderer* pRenderer, EIBUsage usage, int nSize, S_INDEXBUFFER_FORMAT format, SIndex* pInitialData = nullptr);

	virtual SResult Initialize(IGameEngine* pEngine, IRenderer* pRenderer, EIBUsage usage, int nSize, SIndex* pInitialData = nullptr)
	{
		return Initialize(pEngine, pRenderer, usage, nSize, S_INDEXBUFFER_16, pInitialData);
	}

	virtual SResult Create(int nSize, SIndex* pInitialData = nullptr, usint32 nInitialDataCount = 0);	

	virtual BOOL IsInited(void);	
	virtual SResult Resize(int nNewSize);	
	virtual SResult Lock(UINT iBegin, UINT iLength, SIndex** buf, EIBLockType lockType);
	virtual SResult Lock(UINT iBegin, UINT iLength, SIndex** buf);	
	virtual SResult Fill(SIndex* pIndices, int nIndices, bool append);	
	virtual SResult Unlock(void);	
	virtual SIndex* GetShadowBuffer(void);	
	virtual SIndex* GetIndex(int iIndex);	
	virtual INT GetIndexCount(void);	
	virtual SResult Clear(void);
//virtual S_INDEXBUFFER_FORMAT GetFormat();

	// DX
	ID3D11Buffer* D3D11_GetBuffer()
	{
		return m_pHWIndexBuffer;
	}

	SDirectX11IBCreateFlags GetCreateFlags();
};


SP_NMSPACE_END
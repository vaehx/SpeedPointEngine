//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2016 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////


#pragma once

#include "..\IIndexBuffer.h"
#include "DX11.h"

SP_NMSPACE_BEG

struct S_API IRenderer;

struct S_API SDX11IBCreateFlags
{
	D3D11_USAGE usage;
	unsigned int bindFlags;
	unsigned int cpuAccessFlags;

	SDX11IBCreateFlags()
		: usage(D3D11_USAGE_DEFAULT),
		bindFlags(D3D11_BIND_VERTEX_BUFFER),
		cpuAccessFlags(0)
	{
	}

	SDX11IBCreateFlags(const SDX11IBCreateFlags& o)
		: usage(o.usage),
		bindFlags(o.bindFlags),
		cpuAccessFlags(o.cpuAccessFlags)
	{
	}

	SDX11IBCreateFlags(D3D11_USAGE pUsage, unsigned int pBindFlags, unsigned int pCpuAccessFlags)
		: usage(pUsage),
		bindFlags(pBindFlags),
		cpuAccessFlags(pCpuAccessFlags)
	{
	}
};


// SpeedPoint IndexBuffer Resource
class S_API DX11IndexBuffer : public IIndexBuffer
{
private:	
	IRenderer*			m_pRenderer;
	ID3D11Buffer*			m_pHWIndexBuffer;
	void*			m_pShadowBuffer;
	unsigned long			m_nIndices;
	unsigned long			m_nIndicesWritten;
	EIBUsage			m_Usage;
	bool				m_bLocked;
	bool				m_bInitialDataWritten;
	S_INDEXBUFFER_FORMAT m_Format;

public:
	DX11IndexBuffer();
	DX11IndexBuffer(const DX11IndexBuffer& o);
	~DX11IndexBuffer();
	
	virtual SResult Initialize(IRenderer* pRenderer, EIBUsage usage, S_INDEXBUFFER_FORMAT format, const void* pInitialData = nullptr, unsigned long nInitialIndices = 0);

	virtual SResult Initialize(IRenderer* pRenderer, EIBUsage usage, const SIndex* pInitialData = nullptr, unsigned long nInitialIndices = 0)
	{
		return Initialize(pRenderer, usage, S_INDEXBUFFER_16, (const void*)pInitialData, nInitialIndices);
	}

	virtual SResult Create(unsigned long nSize, const void* pInitialData = nullptr, usint32 nInitialDataCount = 0);	

	virtual BOOL IsInited(void);	
	virtual SResult Resize(unsigned long nNewSize);	
	virtual SResult Lock(UINT iBegin, UINT iLength, void** buf, EIBLockType lockType);
	virtual SResult Lock(UINT iBegin, UINT iLength, void** buf);	
	virtual SResult Fill(const void* pIndices, unsigned long nIndices, bool append);
	virtual SResult Unlock(void);	
	virtual void* GetShadowBuffer(void);	
	virtual SIndex* GetIndex(unsigned long iIndex);	
	virtual SLargeIndex* GetLargeIndex(unsigned long iIndex);
	virtual unsigned long GetIndexCount(void) const;	
	virtual SResult Clear(void);
	virtual S_INDEXBUFFER_FORMAT GetFormat() const
	{
		return m_Format;
	}

	// DX
	ID3D11Buffer* D3D11_GetBuffer()
	{
		return m_pHWIndexBuffer;
	}

	SDX11IBCreateFlags GetCreateFlags();
};


SP_NMSPACE_END

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

class S_API SpeedPointEngine;
struct S_API IRenderer;


// SpeedPoint IndexBuffer Resource
class S_API DirectX11IndexBuffer : public IIndexBuffer
{
private:
	SpeedPointEngine*		m_pEngine;
	IRenderer*			m_pRenderer;
	ID3D11Buffer*			m_pHWIndexBuffer;
	SIndex*			m_pShadowBuffer;
	int				m_nIndices;
	int				m_nIndicesWritten;
	bool				m_bDynamic;
	bool				m_bLocked;


public:
	DirectX11IndexBuffer();
	DirectX11IndexBuffer(const DirectX11IndexBuffer& o);
	~DirectX11IndexBuffer();
	
	virtual SResult Initialize(int nSize, bool bDynamic, SpeedPointEngine* pEng, IRenderer* renderer, S_INDEXBUFFER_FORMAT format = S_INDEXBUFFER_16);
	virtual SResult Create(int nSize, bool bDynamic_);	

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
	virtual S_INDEXBUFFER_FORMAT GetFormat();

	// DX
	ID3D11Buffer* D3D11_GetBuffer()
	{
		return m_pHWIndexBuffer;
	}
};


SP_NMSPACE_END
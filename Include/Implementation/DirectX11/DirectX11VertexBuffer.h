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
	
	virtual SResult Initialize(int nSize, bool bDynamic, SpeedPointEngine* pEng, IRenderer* renderer);	
	virtual SResult Create(int nSize, bool bDynamic_);
	
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
};


SP_NMSPACE_END
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


enum EIBLockType
{
	eIBLOCK_DISCARD,
	eIBLOCK_NOOVERWRITE
};


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

	// Initialize the Index buffer
	SResult Initialize(int nSize, bool bDynamic, SpeedPointEngine* pEng, IRenderer* renderer);

	// Create the Hardware Index Buffer
	SResult Create(int nSize, bool bDynamic_);

	// Check if this Index Buffer is inited properly
	BOOL IsInited(void);

	// Change the size of the Hardware Index Buffer
	SResult Resize(int nNewSize);

	// Lock the Hardware Index Buffer in order to be able to fill Hardware data
	SResult Lock(UINT iBegin, UINT iLength, SIndex** buf, EIBLockType lockType);
	SResult Lock(UINT iBegin, UINT iLength, SIndex** buf);

	// Fill the Hardware Index Buffer with an array of Indices
	SResult Fill(SIndex* pIndices, int nIndices, bool append);

	// Unlock the Hardware Index Buffer
	SResult Unlock(void);

	// Get the RAM Copy of the hardware Index Buffer
	SIndex* GetShadowBuffer(void);

	// Get a Pointer to an Index
	SIndex* GetIndex(int iIndex);

	// Get the total count of all Indices
	INT GetIndexCount(void);

	// Clear everything and free memory
	SResult Clear(void);
};


SP_NMSPACE_END
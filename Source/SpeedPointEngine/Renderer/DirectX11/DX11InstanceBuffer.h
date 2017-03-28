//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\IInstanceBuffer.h"
#include "DX11.h"
#include <mutex>

using std::mutex;
using std::unique_lock;

SP_NMSPACE_BEG

class S_API DX11Renderer;

class S_API DX11InstanceBuffer : public ITypelessInstanceBuffer
{
private:
	DX11Renderer* m_pDXRenderer;
	D3D11_BUFFER_DESC m_HWBufferDesc;
	ID3D11Buffer* m_pHWBuffer;
	void* m_pBuffer;
	unsigned int m_StrideSz;
	unsigned int m_ChunkSz;
	unsigned int m_nInstances;
	unsigned int m_Capacity;

	mutex m_Mutex;
	unique_lock<mutex> m_Lock;
	unsigned int m_CapacityBeforeLock;
	unsigned int m_MinChangedIndex;
	unsigned int m_MaxChangedIndex;

public:
	DX11InstanceBuffer(DX11Renderer* pDXRenderer);

	inline virtual ITypelessInstanceBuffer* GetTypelessInstanceBuffer() { return this; }
	inline virtual unsigned int GetStrideSize() const { return m_StrideSz; }

	virtual void Init(unsigned int strideSz, unsigned int chunkSize = 10);
	virtual bool IsLocked() const;
	virtual const void* Lock(unsigned int* pnInstances);
	virtual void Unlock();
	virtual void* AddInstance();
	virtual const void* GetInstance(unsigned int i) const;
	virtual void* GetInstanceWriteable(unsigned int i);
	virtual void RemoveInstance(unsigned int i);
	virtual unsigned int GetNumInstances() const;
	virtual void Clear();

	ID3D11Buffer* D3D11_GetHWBuffer()
	{
		return m_pHWBuffer;
	}
};

SP_NMSPACE_END

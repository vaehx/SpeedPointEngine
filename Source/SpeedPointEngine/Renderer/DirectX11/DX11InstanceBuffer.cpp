//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "DX11InstanceBuffer.h"
#include "DX11Renderer.h"

SP_NMSPACE_BEG

S_API DX11InstanceBuffer::DX11InstanceBuffer(DX11Renderer* pDXRenderer)
	: m_pDXRenderer(pDXRenderer),
	m_Lock(m_Mutex, std::defer_lock),
	m_pBuffer(0),
	m_pHWBuffer(0)
{
}

S_API void DX11InstanceBuffer::Init(unsigned int strideSz, unsigned int chunkSize /*= 10*/)
{
	Clear();

	m_StrideSz = strideSz;
	m_ChunkSz = chunkSize;

	m_pBuffer = malloc(m_ChunkSz * m_StrideSz);
	m_Capacity = m_ChunkSz;

	// Create HW buffer
	m_HWBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_HWBufferDesc.ByteWidth = m_ChunkSz * m_StrideSz;
	m_HWBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_HWBufferDesc.MiscFlags = 0;
	m_HWBufferDesc.StructureByteStride = 0;
	m_HWBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(m_pDXRenderer->GetD3D11Device()->CreateBuffer(&m_HWBufferDesc, 0, &m_pHWBuffer)))
	{
		CLog::Log(S_ERROR, "Failed create DX11InstanceBuffer: CreateBuffer() failed");
		return;
	}
}

S_API bool DX11InstanceBuffer::IsLocked() const
{
	return m_Lock.owns_lock();
}

S_API const void* DX11InstanceBuffer::Lock(unsigned int* pnInstances)
{
	m_Lock.lock();

	if (pnInstances)
		*pnInstances = m_nInstances;

	m_MinChangedIndex = UINT_MAX;
	m_MaxChangedIndex = 0;
	m_CapacityBeforeLock = m_Capacity;

	return m_pBuffer;
}

S_API void DX11InstanceBuffer::Unlock()
{
	if (!m_Lock.owns_lock())
		return;

	// Upload data
	if (m_MinChangedIndex <= m_MaxChangedIndex)
	{
		if (m_Capacity > m_CapacityBeforeLock)
		{
			// Create a new, bigger buffer
			m_pHWBuffer->Release();

			D3D11_SUBRESOURCE_DATA initialData;
			initialData.pSysMem = m_pBuffer;
			initialData.SysMemPitch = 0;
			initialData.SysMemSlicePitch = 0;

			m_HWBufferDesc.ByteWidth = m_Capacity * m_StrideSz;
			m_pDXRenderer->GetD3D11Device()->CreateBuffer(&m_HWBufferDesc, &initialData, &m_pHWBuffer);
		}
		else
		{
			// Map and update existing buffer
			ID3D11DeviceContext* pDevCtx = m_pDXRenderer->GetD3D11DeviceContext();

			D3D11_MAPPED_SUBRESOURCE mappedSubresource;
			if (FAILED(pDevCtx->Map(m_pHWBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource)))
			{
				CLog::Log(S_ERROR, "Failed upload instance data: Map() failed");
				return;
			}

			memcpy(mappedSubresource.pData, m_pBuffer, m_nInstances * m_StrideSz);

			pDevCtx->Unmap(m_pHWBuffer, 0);
		}
	}

	m_MinChangedIndex = UINT_MAX;
	m_MaxChangedIndex = 0;

	m_Lock.unlock();
}

S_API void* DX11InstanceBuffer::AddInstance()
{
	if (!m_Lock.owns_lock())
		return 0;

	if (m_nInstances >= m_Capacity)
	{
		// Resize buffer
		unsigned int newCapacity = m_nInstances + m_ChunkSz;
		void* newBuffer = malloc(newCapacity * m_StrideSz);
		memcpy(newBuffer, m_pBuffer, m_Capacity * m_StrideSz);
		free(m_pBuffer);
		m_pBuffer = newBuffer;
		m_Capacity = newCapacity;

		//CLog::Log(S_DEBUG, "Buffer resized to capacity %u", m_Capacity);
	}

	unsigned int newIndex = m_nInstances++;
	if (newIndex < m_MinChangedIndex)
		m_MinChangedIndex = newIndex;
	if (newIndex > m_MaxChangedIndex)
		m_MaxChangedIndex = newIndex;

	return (void*)((char*)m_pBuffer + newIndex * m_StrideSz);
}

S_API void DX11InstanceBuffer::RemoveInstance(unsigned int i)
{
	if (!m_Lock.owns_lock())
		return;

	if (i >= m_nInstances)
		return;

	// Move instances above that instance down, if any
	if (i < m_nInstances - 1)
	{
		char* pInstance = (char*)m_pBuffer + i * m_StrideSz;
		memcpy(pInstance, pInstance + m_StrideSz, m_StrideSz * (m_nInstances - (i + 1)));

		if (i < m_MinChangedIndex)
			m_MinChangedIndex = i;
		if (m_nInstances - 2 > m_MaxChangedIndex)
			m_MaxChangedIndex = m_nInstances - 2;
	}

	--m_nInstances;
}

S_API const void* DX11InstanceBuffer::GetInstance(unsigned int i) const
{
	if (!m_Lock.owns_lock())
		return 0;

	if (i >= m_nInstances)
		return 0;

	return (const void*)((char*)m_pBuffer + i * m_StrideSz);
}

S_API void* DX11InstanceBuffer::GetInstanceWriteable(unsigned int i)
{
	if (!m_Lock.owns_lock())
		return 0;

	if (i >= m_nInstances)
		return 0;

	if (i < m_MinChangedIndex)
		m_MinChangedIndex = i;
	if (i > m_MaxChangedIndex)
		m_MaxChangedIndex = i;

	return (void*)((char*)m_pBuffer + i * m_StrideSz);
}

S_API unsigned int DX11InstanceBuffer::GetNumInstances() const
{
	if (!m_Lock.owns_lock())
		return UINT_MAX;

	return m_nInstances;
}

S_API void DX11InstanceBuffer::Clear()
{
	if (m_pBuffer)
		free(m_pBuffer);

	if (m_pHWBuffer)
		m_pHWBuffer->Release();

	m_pBuffer = 0;
	m_pHWBuffer = 0;
	m_Capacity = 0;
}


SP_NMSPACE_END

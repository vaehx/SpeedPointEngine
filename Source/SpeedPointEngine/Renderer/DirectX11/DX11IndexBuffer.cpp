//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2016 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "DX11IndexBuffer.h"
#include "DX11Renderer.h"

SP_NMSPACE_BEG

S_API DX11IndexBuffer::DX11IndexBuffer()
: m_pRenderer(NULL),
m_pHWIndexBuffer(NULL),
m_pShadowBuffer(NULL),
m_nIndices(0),
m_nIndicesWritten(0),
m_Usage(eIBUSAGE_STATIC),
m_bInitialDataWritten(false),
m_bLocked(false)
{
}


S_API DX11IndexBuffer::DX11IndexBuffer(const DX11IndexBuffer& o)
	: m_pRenderer(o.m_pRenderer),
	m_pHWIndexBuffer(o.m_pHWIndexBuffer),
	m_pShadowBuffer(o.m_pShadowBuffer),
	m_nIndices(o.m_nIndices),
	m_nIndicesWritten(o.m_nIndicesWritten),
	m_Usage(o.m_Usage),
	m_bInitialDataWritten(o.m_bInitialDataWritten),
	m_bLocked(o.m_bLocked)
{
}


// -----------------------------------------------------------------------------------------------
S_API DX11IndexBuffer::~DX11IndexBuffer()
{
	if (m_RefCount > 0)
		CLog::Log(S_WARNING, "Warning: Destructing index buffer with refcount = %u", m_RefCount);

	Clear();
}

// -----------------------------------------------------------------------------------------------
S_API BOOL DX11IndexBuffer::IsInited(void)
{
	return		
		m_pRenderer &&
		m_pHWIndexBuffer &&
		m_pShadowBuffer;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11IndexBuffer::Initialize(
						IRenderer* renderer,
						EIBUsage usage,						
						S_INDEXBUFFER_FORMAT format,
						const void* pInitialData /* = nullptr */,
						unsigned long nSize /* = 0 */)
{
	if (IsInited())
		Clear();
	
	m_pRenderer = renderer;
	m_Usage = usage;	
	m_Format = format;

	if (usage == eIBUSAGE_STATIC)
	{
		if (pInitialData && nSize > 0)
		{
			if (Failure(Create(nSize, pInitialData, nSize)))
				return S_ERROR;
		}
		else
		{
			m_bInitialDataWritten = false; // just to be on the safe side
		}
	}
	else
	{
		if (Failure(Create(nSize)))
			return S_ERROR;
	}

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SDX11IBCreateFlags DX11IndexBuffer::GetCreateFlags()
{
	switch (m_Usage)
	{
	case eIBUSAGE_STATIC:
		return SDX11IBCreateFlags(
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_INDEX_BUFFER,
			0);

	default:
		CLog::Log(S_WARN, "Tried to retrieve ib create flags for unsupported vb usage!");
		return SDX11IBCreateFlags();
	}
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11IndexBuffer::Create(unsigned long nIndices_, const void* pInitialData /* = 0 */, usint32 nInitialDataCount /* = 0 */)
{
	SP_ASSERTR(m_pRenderer, S_NOTINIT);
	SP_ASSERTR(nIndices_ > 0, S_INVALIDPARAM);
	SP_ASSERTR(nInitialDataCount <= (usint32)nIndices_, S_INVALIDPARAM);

	if (m_pHWIndexBuffer || m_pShadowBuffer)
		return CLog::Log(S_ERROR, "Tried to create existing IB - Do a clear first!");

	if (m_Usage == eIBUSAGE_STATIC && !pInitialData)
		return CLog::Log(S_ERROR, "Cannot create static IB without given initial Data!");


	DX11Renderer* pDXRenderer = dynamic_cast<DX11Renderer*>(m_pRenderer);

	unsigned int formatByteSz = sizeof(SIndex);
	if (m_Format == S_INDEXBUFFER_32)
		formatByteSz = sizeof(SLargeIndex);


	m_nIndices = nIndices_;
	m_pShadowBuffer = malloc(m_nIndices * formatByteSz);
	if (pInitialData && nInitialDataCount)
	{
		memcpy(m_pShadowBuffer, pInitialData, formatByteSz * nInitialDataCount);
		if ((usint32)m_nIndices > nInitialDataCount)
		{
			if (m_Format == S_INDEXBUFFER_16)
				memset((SIndex*)m_pShadowBuffer + nInitialDataCount - 1, 0, (m_nIndices - nInitialDataCount) * formatByteSz);
			else
				memset((SLargeIndex*)m_pShadowBuffer + nInitialDataCount - 1, 0, (m_nIndices - nInitialDataCount) * formatByteSz);
		}

		m_nIndicesWritten = nInitialDataCount;
		m_bInitialDataWritten = true;
	}
	else
	{
		// zero memory
		memset(m_pShadowBuffer, 0, formatByteSz * m_nIndices);
		m_nIndicesWritten = 0;
	}

	D3D11_BUFFER_DESC bufferDesc;
	SDX11IBCreateFlags createFlags = GetCreateFlags();
	bufferDesc.BindFlags = createFlags.bindFlags;
	bufferDesc.CPUAccessFlags = createFlags.cpuAccessFlags;
	bufferDesc.Usage = createFlags.usage;
	bufferDesc.ByteWidth = m_nIndices * formatByteSz;
	bufferDesc.StructureByteStride = 0;
	bufferDesc.MiscFlags = 0;


	D3D11_SUBRESOURCE_DATA IndexData;
	IndexData.pSysMem = (void*)m_pShadowBuffer;
	IndexData.SysMemPitch = 0;
	IndexData.SysMemSlicePitch = 0;


	if (Failure(pDXRenderer->GetD3D11Device()->CreateBuffer(&bufferDesc, &IndexData, &m_pHWIndexBuffer)))
	{
		return CLog::Log(S_ERROR, "Could not create Index buffer resource!");
	}

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11IndexBuffer::Resize(unsigned long nIndices_)
{
	if (nIndices_ == m_nIndices)
		return S_SUCCESS;

	if (m_Usage == eIBUSAGE_STATIC)
		return CLog::Log(S_ERROR, "Tried to resize static IB!");

	SP_ASSERTR(m_pRenderer, S_NOTINIT);

	usint32 nIndicesOld = m_nIndices;
	usint32 nIndicesWrittenOld = m_nIndicesWritten;
	m_nIndices = nIndices_; // total size

	DX11Renderer* pDXRenderer = dynamic_cast<DX11Renderer*>(m_pRenderer);


	// check whether we acutally resize or create a new buffer
	bool bResize = nIndicesOld > 0 && m_pShadowBuffer && m_pHWIndexBuffer;

	// Resize Sys memory or create if not existing
	void* vBackup = m_pShadowBuffer;
	m_pShadowBuffer = 0;

	// Clear the old HW buffer
	SP_SAFE_RELEASE(m_pHWIndexBuffer);
	m_pHWIndexBuffer = 0;

	// Create the new buffer
	Create(m_nIndices, vBackup, nIndicesWrittenOld);

	// delete old shadow buffer
	if (vBackup)
		delete[] vBackup;

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11IndexBuffer::Lock(UINT iBegin, UINT iLength, void** buf)
{
	return Lock(iBegin, iLength, buf, eIBLOCK_NOOVERWRITE);
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11IndexBuffer::Lock(UINT iBegin, UINT iLength, void** buf, EIBLockType locktype)
{
	SP_ASSERTR(m_pRenderer, S_NOTINIT);
	SP_ASSERTR(buf, S_INVALIDPARAM);

	// just don't allow any lock if not dynamic
	if (m_Usage == eIBUSAGE_STATIC)
	{
		CLog::Log(S_WARN, "Tried to lock static IB!");
		return S_ABORTED;
	}

	SP_ASSERTR(IsInited() && m_pHWIndexBuffer != NULL && !m_bLocked, S_NOTINITED);

	// Update the HW buffer data
	DX11Renderer* pDXRenderer = dynamic_cast<DX11Renderer*>(m_pRenderer);

	// cast the Lock type
	D3D11_MAP mapType;
	switch (locktype)
	{
	case eIBLOCK_DISCARD:
		mapType = D3D11_MAP_WRITE_DISCARD;
		break;
	case eIBLOCK_NOOVERWRITE:
		mapType = D3D11_MAP_WRITE_NO_OVERWRITE;
		break;
	default:
		mapType = D3D11_MAP_WRITE_NO_OVERWRITE;
	}

	D3D11_MAPPED_SUBRESOURCE mappedVBResource;
	if (Failure(pDXRenderer->GetD3D11DeviceContext()->Map(m_pHWIndexBuffer, 0, mapType, 0, &mappedVBResource)))
	{
		return CLog::Log(S_ERROR, "Failed lock mapped HW-IB resource!");
	}
	m_bLocked = true;

	*buf = (SIndex*)mappedVBResource.pData;

	// everything went well
	return S_SUCCESS;
}


// -----------------------------------------------------------------------------------------------
S_API SResult DX11IndexBuffer::Fill(const void* Indices, unsigned long nIndices_, bool bAppend)
{
	SP_ASSERTR(m_pRenderer, S_NOTINIT);
	SP_ASSERTR(Indices, S_INVALIDPARAM);
	SP_ASSERTR(!m_bLocked, S_ABORTED);

	if (m_Usage == eIBUSAGE_STATIC)
	{


		if (m_bInitialDataWritten)
			return CLog::Log(S_ABORTED, "Cannot fill static buffer which is already filled!");


		// this Fill()-Call writes initial data so create the buffer with the initial data
		SResult createRes;
		if (Failure((createRes = Create(nIndices_, Indices, nIndices_))))
			return createRes;

		m_nIndicesWritten = nIndices_;
		m_bInitialDataWritten = true;

		return S_SUCCESS;

	}


	// else:

	usint32 iBegin;
	EIBLockType lockType;

	if (bAppend) // add to end of existing data
	{
		if (nIndices_ + m_nIndicesWritten >
			m_nIndices) // m_nIndices actually stores the size of the existing IB, or 0 if not existing
		{


			// Resize
			// also creates the buffer if not existing yet
			if (Failure(Resize(nIndices_ + m_nIndicesWritten)))
				return CLog::Log(S_ERROR, "Resize for fill Index buffer failed (append)!");

		}

		iBegin = m_nIndicesWritten;
		lockType = eIBLOCK_NOOVERWRITE;
	}
	else // overwrite old data
	{
		if (nIndices_ > m_nIndices)
		{

			// also creates the buffer if not existing yet
			if (Failure(Resize(nIndices_)))
				return CLog::Log(S_ERROR, "Resize for fill Index buffer failed!");

		}

		iBegin = 0;
		lockType = eIBLOCK_DISCARD;
	}

	unsigned int formatByteSz = sizeof(SIndex);
	if (m_Format == S_INDEXBUFFER_32)
		formatByteSz = sizeof(SLargeIndex);


	// Lock buffer if not happened
	SIndex* pIndices = 0;
	SResult lockResult = Lock(iBegin * formatByteSz, (UINT)(nIndices_ * formatByteSz), (void**)&pIndices, lockType);

	if (Failure(lockResult) || pIndices == 0)
		return CLog::Log(S_ERROR, "Could not lock Index buffer to fill with Index data!");


	// Now copy data		
	memcpy(pIndices, (void*)Indices, nIndices_ * sizeof(SIndex));

	// copy data to our copy in sys mem (shadow buffer)
	if (m_Format == S_INDEXBUFFER_16)
		memcpy((void*)&((SIndex*)m_pShadowBuffer)[iBegin], (void*)Indices, nIndices_ * formatByteSz);
	else
		memcpy((void*)&((SLargeIndex*)m_pShadowBuffer)[iBegin], (void*)Indices, nIndices_ * formatByteSz);

	m_nIndicesWritten = (unsigned long)iBegin + nIndices_;
	m_bInitialDataWritten = true;

	// ... and unlock	
	if (Failure(Unlock()))
		return S_ERROR;

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11IndexBuffer::Unlock(void)
{
	SP_ASSERTR(IsInited(), S_NOTINIT);
	SP_ASSERTR(m_bLocked, S_ABORTED);

	DX11Renderer* pDXRenderer = dynamic_cast<DX11Renderer*>(m_pRenderer);
	if (!pDXRenderer)
		return S_ERROR;

	pDXRenderer->GetD3D11DeviceContext()->Unmap(m_pHWIndexBuffer, 0);

	m_bLocked = false;
	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API void DX11IndexBuffer::Clear(void)
{
	SP_SAFE_RELEASE(m_pHWIndexBuffer);
	if (m_pShadowBuffer)
		free(m_pShadowBuffer);

	m_nIndices = 0;
	m_nIndicesWritten = 0;
	m_pShadowBuffer = 0;
}

// -----------------------------------------------------------------------------------------------
S_API void* DX11IndexBuffer::GetShadowBuffer(void)
{
	if (IsInited())
	{
		return m_pShadowBuffer;
	}

	return NULL;
}

// -----------------------------------------------------------------------------------------------
S_API SIndex* DX11IndexBuffer::GetIndex(unsigned long iIndex)
{
	if (IsInited() && m_nIndices > iIndex && iIndex >= 0)
	{
		if (m_Format == S_INDEXBUFFER_16)
			return &((SIndex*)m_pShadowBuffer)[iIndex];
		else
			return (SIndex*)&((SLargeIndex*)m_pShadowBuffer)[iIndex];
	}

	return NULL;
}

// -----------------------------------------------------------------------------------------------
S_API SLargeIndex* DX11IndexBuffer::GetLargeIndex(unsigned long iIndex)
{
	if (IsInited() && m_nIndices > iIndex && iIndex >= 0)
	{
		if (m_Format == S_INDEXBUFFER_16)
			return (SLargeIndex*)&((SIndex*)m_pShadowBuffer)[iIndex];
		else
			return &((SLargeIndex*)m_pShadowBuffer)[iIndex];
	}

	return NULL;
}

// -----------------------------------------------------------------------------------------------
S_API unsigned long DX11IndexBuffer::GetIndexCount(void) const
{
	return m_nIndices;
}


SP_NMSPACE_END

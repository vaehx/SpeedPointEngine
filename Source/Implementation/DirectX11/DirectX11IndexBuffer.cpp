//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2011-2014, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include <Implementation\DirectX11\DirectX11IndexBuffer.h>
#include <Implementation\DirectX11\DirectX11Renderer.h>
#include <SpeedPointEngine.h>

SP_NMSPACE_BEG

S_API DirectX11IndexBuffer::DirectX11IndexBuffer()
: m_pEngine(NULL),
m_pRenderer(NULL),
m_pHWIndexBuffer(NULL),
m_pShadowBuffer(NULL),
m_nIndices(0),
m_nIndicesWritten(0),
m_Usage(eIBUSAGE_STATIC),
m_bInitialDataWritten(false),
m_bLocked(false)
{
}


S_API DirectX11IndexBuffer::DirectX11IndexBuffer(const DirectX11IndexBuffer& o)
	: m_pEngine(o.m_pEngine),
	m_pRenderer(o.m_pRenderer),
	m_pHWIndexBuffer(o.m_pHWIndexBuffer),
	m_pShadowBuffer(o.m_pShadowBuffer),
	m_nIndices(o.m_nIndices),
	m_nIndicesWritten(o.m_nIndicesWritten),
	m_Usage(o.m_Usage),
	m_bInitialDataWritten(o.m_bInitialDataWritten),
	m_bLocked(o.m_bLocked)
{
}


// --------------------------------------------------------------------------------
S_API DirectX11IndexBuffer::~DirectX11IndexBuffer()
{
	// Make sure resources are freed
	Clear();
}

// --------------------------------------------------------------------------------
S_API BOOL DirectX11IndexBuffer::IsInited(void)
{
	return
		m_pEngine &&
		m_pRenderer &&
		m_pHWIndexBuffer &&
		m_pShadowBuffer;
}

// --------------------------------------------------------------------------------
S_API SResult DirectX11IndexBuffer::Initialize(IGameEngine* pEngine,
						IRenderer* renderer,
						EIBUsage usage,
						unsigned long nSize,						
						S_INDEXBUFFER_FORMAT format,
						SIndex* pInitialData /* = nullptr */)
{
	if (IsInited())
		Clear();

	m_pEngine = pEngine;
	m_pRenderer = renderer;
	m_Usage = usage;	

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

// --------------------------------------------------------------------------------
S_API SDirectX11IBCreateFlags DirectX11IndexBuffer::GetCreateFlags()
{
	switch (m_Usage)
	{
	case eIBUSAGE_STATIC:
		return SDirectX11IBCreateFlags(
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_INDEX_BUFFER,
			0);

	default:
		m_pEngine->LogW("Tried to retrieve ib create flags for unsupported vb usage!");
		return SDirectX11IBCreateFlags();
	}
}

// --------------------------------------------------------------------------------
S_API SResult DirectX11IndexBuffer::Create(unsigned long nIndices_, SIndex* pInitialData /* = 0 */, usint32 nInitialDataCount /* = 0 */)
{
	SP_ASSERTR(m_pEngine && m_pRenderer, S_NOTINIT);
	SP_ASSERTR(nIndices_ > 0, S_INVALIDPARAM);
	SP_ASSERTR(nInitialDataCount <= (usint32)nIndices_, S_INVALIDPARAM);

	if (m_pHWIndexBuffer || m_pShadowBuffer)
		return m_pEngine->LogE("Tried to create existing IB - Do a clear first!");

	if (m_Usage == eIBUSAGE_STATIC && !pInitialData)
		return m_pEngine->LogE("Cannot create static IB without given initial Data!");


	DirectX11Renderer* pDXRenderer = (DirectX11Renderer*)m_pRenderer;

	m_nIndices = nIndices_;
	m_pShadowBuffer = (SIndex*)(malloc(m_nIndices * sizeof(SIndex)));
	if (pInitialData && nInitialDataCount)
	{
		memcpy(m_pShadowBuffer, pInitialData, sizeof(SIndex) * nInitialDataCount);
		if ((usint32)m_nIndices > nInitialDataCount)
			memset(m_pShadowBuffer + nInitialDataCount - 1, 0, (m_nIndices - nInitialDataCount));

		m_nIndicesWritten = nInitialDataCount;
		m_bInitialDataWritten = true;
	}
	else
	{
		// zero memory
		memset(m_pShadowBuffer, 0, sizeof(SIndex) * m_nIndices);
		m_nIndicesWritten = 0;
	}

	D3D11_BUFFER_DESC bufferDesc;
	SDirectX11IBCreateFlags createFlags = GetCreateFlags();
	bufferDesc.BindFlags = createFlags.bindFlags;
	bufferDesc.CPUAccessFlags = createFlags.cpuAccessFlags;
	bufferDesc.Usage = createFlags.usage;
	bufferDesc.ByteWidth = m_nIndices * sizeof(SIndex);
	bufferDesc.StructureByteStride = 0;
	bufferDesc.MiscFlags = 0;


	D3D11_SUBRESOURCE_DATA IndexData;
	IndexData.pSysMem = (void*)m_pShadowBuffer;
	IndexData.SysMemPitch = 0;
	IndexData.SysMemSlicePitch = 0;


	if (Failure(pDXRenderer->GetD3D11Device()->CreateBuffer(&bufferDesc, &IndexData, &m_pHWIndexBuffer)))
	{
		return m_pEngine->LogE("Could not create Index buffer resource!");
	}

	return S_SUCCESS;
}

// --------------------------------------------------------------------------------
S_API SResult DirectX11IndexBuffer::Resize(unsigned long nIndices_)
{
	if (nIndices_ == m_nIndices)
		return S_SUCCESS;

	if (m_Usage == eIBUSAGE_STATIC)
		return m_pEngine->LogE("Tried to resize static IB!");

	SP_ASSERTR(m_pEngine && m_pRenderer, S_NOTINIT);

	int nIndicesOld = m_nIndices;
	usint32 nIndicesWrittenOld = m_nIndicesWritten;
	m_nIndices = nIndices_; // total size

	DirectX11Renderer* pDXRenderer = (DirectX11Renderer*)m_pRenderer;


	// check whether we acutally resize or create a new buffer
	bool bResize = nIndicesOld > 0 && m_pShadowBuffer && m_pHWIndexBuffer;

	// Resize Sys memory or create if not existing
	SIndex* vBackup = m_pShadowBuffer;
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

// --------------------------------------------------------------------------------
S_API SResult DirectX11IndexBuffer::Lock(UINT iBegin, UINT iLength, SIndex** buf)
{
	return Lock(iBegin, iLength, buf, eIBLOCK_NOOVERWRITE);
}

// --------------------------------------------------------------------------------
S_API SResult DirectX11IndexBuffer::Lock(UINT iBegin, UINT iLength, SIndex** buf, EIBLockType locktype)
{
	SP_ASSERTR(m_pEngine, S_NOTINIT);
	SP_ASSERTR(buf, S_INVALIDPARAM);

	// just don't allow any lock if not dynamic
	if (m_Usage == eIBUSAGE_STATIC)
	{
		m_pEngine->LogW("Tried to lock static IB!");
		return S_ABORTED;
	}

	SP_ASSERTR(IsInited() && m_pHWIndexBuffer != NULL && !m_bLocked, S_NOTINITED);

	// Update the HW buffer data
	DirectX11Renderer* pDXRenderer = (DirectX11Renderer*)m_pRenderer;

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
		return m_pEngine->LogE("Failed lock mapped HW-IB resource!");
	}
	m_bLocked = true;

	*buf = (SIndex*)mappedVBResource.pData;

	// everything went well
	return S_SUCCESS;
}


// --------------------------------------------------------------------------------
S_API SResult DirectX11IndexBuffer::Fill(SIndex* Indices, unsigned long nIndices_, bool bAppend)
{
	SP_ASSERTR(m_pEngine && m_pRenderer, S_NOTINIT);
	SP_ASSERTR(Indices, S_INVALIDPARAM);
	SP_ASSERTR(!m_bLocked, S_ABORTED);

	if (m_Usage == eIBUSAGE_STATIC)
	{


		if (m_bInitialDataWritten)
			return m_pEngine->LogReport(S_ABORTED, "Cannot fill static buffer which is already filled!");


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
				return m_pEngine->LogE("Resize for fill Index buffer failed (append)!");

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
				return m_pEngine->LogE("Resize for fill Index buffer failed!");

		}

		iBegin = 0;
		lockType = eIBLOCK_DISCARD;
	}


	// Lock buffer if not happened
	SIndex* pIndices = 0;
	SResult lockResult = Lock(iBegin * sizeof(SIndex), (UINT)(nIndices_ * sizeof(SIndex)), &pIndices, lockType);

	if (Failure(lockResult) || pIndices == 0)
		return m_pEngine->LogE("Could not lock Index buffer to fill with Index data!");


	// Now copy data		
	memcpy(pIndices, (void*)Indices, nIndices_ * sizeof(SIndex));

	// copy data to our copy in sys mem (shadow buffer)
	memcpy((void*)&m_pShadowBuffer[iBegin], (void*)Indices, nIndices_ * sizeof(SIndex));

	m_nIndicesWritten = (int)iBegin + nIndices_;
	m_bInitialDataWritten = true;

	// ... and unlock	
	if (Failure(Unlock()))
		return S_ERROR;

	return S_SUCCESS;
}

// --------------------------------------------------------------------------------
S_API SResult DirectX11IndexBuffer::Unlock(void)
{
	SP_ASSERTR(IsInited(), S_NOTINIT);
	SP_ASSERTR(m_bLocked, S_ABORTED);

	((DirectX11Renderer*)m_pRenderer)->GetD3D11DeviceContext()->Unmap(m_pHWIndexBuffer, 0);

	m_bLocked = false;
	return S_SUCCESS;
}

// --------------------------------------------------------------------------------
S_API SResult DirectX11IndexBuffer::Clear(void)
{
	if (IsInited())
	{
		SP_SAFE_RELEASE(m_pHWIndexBuffer);
		if (m_pShadowBuffer)
			free(m_pShadowBuffer);

		m_nIndices = 0;
		m_nIndicesWritten = 0;
		m_pShadowBuffer = 0;
	}

	return S_SUCCESS;
}

// --------------------------------------------------------------------------------
S_API SIndex* DirectX11IndexBuffer::GetShadowBuffer(void)
{
	if (IsInited())
	{
		return m_pShadowBuffer;
	}

	return NULL;
}

// --------------------------------------------------------------------------------
S_API SIndex* DirectX11IndexBuffer::GetIndex(unsigned long iIndex)
{
	if (IsInited() && m_nIndices > iIndex && iIndex >= 0)
	{
		return &m_pShadowBuffer[iIndex];
	}

	return NULL;
}

// --------------------------------------------------------------------------------
S_API unsigned long DirectX11IndexBuffer::GetIndexCount(void) const
{
	return m_nIndices;
}


SP_NMSPACE_END
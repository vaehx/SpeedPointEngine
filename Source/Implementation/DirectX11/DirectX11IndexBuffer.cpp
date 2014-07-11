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
m_bDynamic(false),
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
	m_bDynamic(o.m_bDynamic),
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
S_API S_INDEXBUFFER_FORMAT DirectX11IndexBuffer::GetFormat()
{

	
	
	// TODO: make this variable!
	
	return S_INDEXBUFFER_16;	



}

// --------------------------------------------------------------------------------
S_API SResult DirectX11IndexBuffer::Initialize(int nSize, bool bDyn, SpeedPointEngine* engine, IRenderer* renderer, S_INDEXBUFFER_FORMAT fmt /*=S_INDEXBUFFER_16*/)
{
	if (IsInited())
		Clear();

	m_pEngine = engine;
	m_pRenderer = renderer;

	if (Failure(Create(nSize, bDyn)))
	{
		return S_ERROR;
	}

	return S_SUCCESS;
}

// --------------------------------------------------------------------------------
S_API SResult DirectX11IndexBuffer::Create(int nIndices_, bool bDynamic_)
{
	SP_ASSERTR(m_pEngine && m_pRenderer, S_NOTINIT);

	m_nIndices = nIndices_;
	m_bDynamic = bDynamic_;

	DirectX11Renderer* pDXRenderer = (DirectX11Renderer*)m_pRenderer;


	m_pShadowBuffer = (SIndex*)(malloc(m_nIndices * sizeof(SIndex)));

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.ByteWidth = m_nIndices * sizeof(SIndex);
	bufferDesc.CPUAccessFlags = 0;
	if (bDynamic_)
		bufferDesc.CPUAccessFlags = /*D3D11_CPU_ACCESS_READ | */D3D11_CPU_ACCESS_WRITE;
	bufferDesc.StructureByteStride = 0;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	if (bDynamic_)
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
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
S_API SResult DirectX11IndexBuffer::Resize(int nIndices_)
{
	if (nIndices_ == m_nIndices)
		return S_SUCCESS;

	SP_ASSERTR(m_pEngine && m_pRenderer, S_NOTINIT);

	int nIndicesOld = m_nIndices;
	m_nIndices = nIndices_;

	DirectX11Renderer* pDXRenderer = (DirectX11Renderer*)m_pRenderer;


	// Resize Sys memory
	void* vBackup = (void*)(malloc(nIndicesOld * sizeof(SIndex)));
	memcpy(vBackup, m_pShadowBuffer, sizeof(SIndex)* nIndicesOld);
	m_pShadowBuffer = (SIndex*)(malloc(m_nIndices * sizeof(SIndex)));
	memcpy(m_pShadowBuffer, vBackup, sizeof(SIndex)* nIndicesOld);



	// Setup description of the new buffer
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.ByteWidth = nIndices_ * sizeof(SIndex);
	bufferDesc.CPUAccessFlags = 0;
	if (m_bDynamic)
		bufferDesc.CPUAccessFlags = /*D3D11_CPU_ACCESS_READ | */D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	if (m_bDynamic)
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;


	D3D11_SUBRESOURCE_DATA IndexData;
	IndexData.pSysMem = m_pShadowBuffer;
	IndexData.SysMemPitch = 0;
	IndexData.SysMemSlicePitch = 0;

	SP_SAFE_RELEASE(m_pHWIndexBuffer);

	if (Failure(pDXRenderer->GetD3D11Device()->CreateBuffer(&bufferDesc, &IndexData, &m_pHWIndexBuffer)))
	{
		return m_pEngine->LogE("Failed create resized Index buffer!");
	}

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

	// just don't allow HW Buffer updates if not dynamic
	if (!m_bDynamic)
		return S_ABORTED;

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
		return m_pEngine->LogE("Failed lock mapped HW-VB resource!");
	}
	m_bLocked = true;

	*buf = (SIndex*)mappedVBResource.pData;

	// everything went well
	return S_SUCCESS;
}


// --------------------------------------------------------------------------------
S_API SResult DirectX11IndexBuffer::Fill(SIndex* Indices, int nIndices_, bool append)
{
	SP_ASSERTR(m_pEngine && IsInited() && m_pHWIndexBuffer, S_NOTINIT);
	SP_ASSERTR(Indices, S_INVALIDPARAM);

	if (!m_bDynamic && nIndices_ > m_nIndices)
		return m_pEngine->LogReport(S_ABORTED, "Cannot fill non-dynamic Index buffer: Buffer overflow!");

	if (append && m_nIndicesWritten > 0 && !m_bDynamic)
		return m_pEngine->LogReport(S_ABORTED, "Cannot append Index data to non-dynamic Index Buffer");

	if (nIndices_ + m_nIndicesWritten > m_nIndices)
	{
		// Resize
		if (Failure(Resize(nIndices_ + m_nIndicesWritten)))
			return m_pEngine->LogE("Resize for fill Index buffer failed!");
	}

	// Lock buffer if not happened
	SIndex* pVert = 0;
	if (!m_bLocked)
	{
		SResult lockResult = Lock((UINT)(m_nIndicesWritten * sizeof(SIndex)), (UINT)(nIndices_ * sizeof(SIndex)), &pVert);
		if (Failure(lockResult) || pVert == 0)
			return m_pEngine->LogE("Could not lock Index buffer to fill with Index data!");
	}

	// Now copy data		
	memcpy(pVert, (void*)Indices, nIndices_ * sizeof(SIndex));

	// copy data to our copy in sys mem (shadow buffer)
	memcpy((void*)&m_pShadowBuffer[m_nIndicesWritten], (void*)Indices, nIndices_ * sizeof(SIndex));

	m_nIndicesWritten += nIndices_;

	// ... and unlock
	if (Failure(Unlock()))
		return S_ERROR;


	/*
	D3DIndexBUFFER_DESC dsc;
	pHWIndexBuffer->GetDesc(&dsc);
	int vbs = dsc.Size;
	*/

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
S_API SIndex* DirectX11IndexBuffer::GetIndex(int iIndex)
{
	if (IsInited() && m_nIndices > iIndex && iIndex >= 0)
	{
		return &m_pShadowBuffer[iIndex];
	}

	return NULL;
}

// --------------------------------------------------------------------------------
S_API INT DirectX11IndexBuffer::GetIndexCount(void)
{
	return m_nIndices;
}


SP_NMSPACE_END
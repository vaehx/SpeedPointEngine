//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2011-2014, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include <Implementation\DirectX11\DirectX11VertexBuffer.h>
#include <Implementation\DirectX11\DirectX11Renderer.h>
#include <Util\SVertex.h>
#include <Abstract\IGameEngine.h>

SP_NMSPACE_BEG

S_API DirectX11VertexBuffer::DirectX11VertexBuffer()
: m_pEngine(NULL),
m_pRenderer(NULL),
m_pHWVertexBuffer(NULL),
m_pShadowBuffer(NULL),
m_nVertices(0),
m_Usage(eVBUSAGE_STATIC)
{
}


S_API DirectX11VertexBuffer::DirectX11VertexBuffer(const DirectX11VertexBuffer& o)
: m_pEngine(o.m_pEngine),
m_pRenderer(o.m_pRenderer),
m_pHWVertexBuffer(o.m_pHWVertexBuffer),
m_pShadowBuffer(o.m_pShadowBuffer),
m_nVertices(o.m_nVertices),
m_Usage(o.m_Usage)
{
}


// --------------------------------------------------------------------------------
S_API DirectX11VertexBuffer::~DirectX11VertexBuffer()
{
	// Make sure resources are freed
	Clear();
}

// --------------------------------------------------------------------------------
S_API bool DirectX11VertexBuffer::IsInited(void)
{
	return m_pEngine && m_pRenderer && m_pHWVertexBuffer && m_pShadowBuffer;
}

// --------------------------------------------------------------------------------
S_API SResult DirectX11VertexBuffer::Initialize(IGameEngine* pEngine,
						IRenderer* renderer,
						EVBUsage usage,																						
						const SVertex* pInitialData /* = nullptr */,
						const unsigned long nInitialVertices /* = 0 */)
{
	if (IsInited())
		Clear();

	if (!IS_VALID_PTR((m_pEngine = pEngine)))
		return S_INVALIDPARAM;

	if (!IS_VALID_PTR((m_pRenderer = renderer)))
	{
		EngLog(S_WARN, m_pEngine, "Invalid ptr to renderer given in DirectX11VertexBuffer::Initialize! Quering from engine...");
		m_pRenderer = m_pEngine->GetRenderer();
		if (!IS_VALID_PTR(m_pRenderer))
			return EngLog(S_ERROR, m_pEngine, "Renderer of engine not initialized! Cannot Initialize DirectX11 Vertex Buffer!");
	}

	m_Usage = usage;

	m_pHWVertexBuffer = 0;
	m_pShadowBuffer = 0;

	if (IS_VALID_PTR(pInitialData) && nInitialVertices > 0)
	{
		if (Failure(Create(pInitialData, nInitialVertices)))
			return S_ERROR;
	}

	return S_SUCCESS;
}

// --------------------------------------------------------------------------------
S_API SDirectX11VBCreateFlags DirectX11VertexBuffer::GetCreateFlags()
{
	switch (m_Usage)
	{
	case eVBUSAGE_STATIC:
		return SDirectX11VBCreateFlags(
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_VERTEX_BUFFER,
			0);

	case eVBUSAGE_DYNAMIC_RARE:
		// TODO:
		//	Create and use D3D11_USAGE_STAGING for a second buffer.
		//	Fill the second buffer with Map() and Unmap().
		//	Use ID3D11DeviceContext::CopyResource to copy from second buffer to actual buffer.
		return SDirectX11VBCreateFlags(
			D3D11_USAGE_DYNAMIC,
			D3D11_BIND_VERTEX_BUFFER,
			D3D11_CPU_ACCESS_WRITE);

	case eVBUSAGE_DYNAMIC_FREQUENT:		
		return SDirectX11VBCreateFlags(
			D3D11_USAGE_DYNAMIC,
			D3D11_BIND_VERTEX_BUFFER,
			D3D11_CPU_ACCESS_WRITE);

	default:
		m_pEngine->LogW("Tried to retrieve vb create flags for u-nsupported vb usage!");
		return SDirectX11VBCreateFlags();
	}
}

// --------------------------------------------------------------------------------
S_API SResult DirectX11VertexBuffer::Create(const SVertex* pInitialData, const unsigned long nInitialDataCount)
{
	SP_ASSERTR(m_pEngine && m_pRenderer, S_NOTINIT);

	if (!IS_VALID_PTR(pInitialData))
		return EngLog(S_INVALIDPARAM, m_pEngine, "Cannot create DX11 Vertex Buffer with invaid initial data pointer!");

	if (nInitialDataCount == 0)
		return EngLog(S_ERROR, m_pEngine, "Cannot create DX11 Vertex Buffer with data count 0!");

	if (m_pHWVertexBuffer || m_pShadowBuffer)
		return EngLog(S_ERROR, m_pEngine, "Cannot Create DX11 Vertex Buffer: Already created.");

	
	DirectX11Renderer* pDXRenderer = (DirectX11Renderer*)m_pRenderer;

	m_nVertices = nInitialDataCount;
	m_pShadowBuffer = (SVertex*)(malloc(m_nVertices * sizeof(SVertex)));	

	// copy initial data to RAM copy
	memcpy(m_pShadowBuffer, pInitialData, sizeof(SVertex) * nInitialDataCount);	


	// Create Hardware Buffer:

	D3D11_BUFFER_DESC bufferDesc;
	SDirectX11VBCreateFlags createFlags = GetCreateFlags();
	bufferDesc.BindFlags = createFlags.bindFlags;	
	bufferDesc.CPUAccessFlags = createFlags.cpuAccessFlags;
	bufferDesc.Usage = createFlags.usage;
	bufferDesc.ByteWidth = m_nVertices * sizeof(SVertex);	
	bufferDesc.StructureByteStride = 0;	
	bufferDesc.MiscFlags = 0;	

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = (void*)m_pShadowBuffer;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	if (Failure(pDXRenderer->GetD3D11Device()->CreateBuffer(&bufferDesc, &vertexData, &m_pHWVertexBuffer)))
	{
		return m_pEngine->LogE("Could not create Vertex buffer resource!");
	}

	return S_SUCCESS;
}

// --------------------------------------------------------------------------------
/*
S_API SResult DirectX11VertexBuffer::Resize(unsigned long nVertices_)
{
	if (nVertices_ == m_nVertices)
		return S_SUCCESS;
	
	if (m_Usage == eVBUSAGE_STATIC)
		return m_pEngine->LogE("Tried to resize static VB!");

	SP_ASSERTR(m_pEngine && m_pRenderer, S_NOTINIT);	

	int nVerticesOld = m_nVertices;
	usint32 nVerticesWrittenOld = m_nVerticesWritten;
	m_nVertices = nVertices_; // total size

	DirectX11Renderer* pDXRenderer = (DirectX11Renderer*)m_pRenderer;


	// check whether we acutally resize or create a new buffer
	bool bResize = nVerticesOld > 0 && m_pShadowBuffer && m_pHWVertexBuffer;

	// Resize Sys memory or create if not existing
	SVertex* vBackup = m_pShadowBuffer;
	m_pShadowBuffer = 0;

	// Clear the old HW buffer
	SP_SAFE_RELEASE(m_pHWVertexBuffer);
	m_pHWVertexBuffer = 0;
	
	// Create the new buffer
	Create(m_nVertices, vBackup, nVerticesWrittenOld);
	
	// delete old shadow buffer
	if (vBackup)
		delete[] vBackup;

	return S_SUCCESS;
}
*/

// --------------------------------------------------------------------------------
/*
S_API SResult DirectX11VertexBuffer::Lock(UINT iBegin, UINT iLength, SVertex** buf)
{
	return Lock(iBegin, iLength, buf, eVBLOCK_NOOVERWRITE);
}
*/

// --------------------------------------------------------------------------------
/*
S_API SResult DirectX11VertexBuffer::Lock(UINT iBegin, UINT iLength, SVertex** buf, EVBLockType locktype)
{
	SP_ASSERTR(m_pEngine, S_NOTINIT);
	SP_ASSERTR(buf, S_INVALIDPARAM);

	// just don't allow any lock if not dynamic
	if (m_Usage == eVBUSAGE_STATIC)
	{
		m_pEngine->LogW("Tried to lock static VB!");
		return S_ABORTED;
	}

	SP_ASSERTR(IsInited() && m_pHWVertexBuffer != NULL && !m_bLocked, S_NOTINITED);

	// Update the HW buffer data
	DirectX11Renderer* pDXRenderer = (DirectX11Renderer*)m_pRenderer;

	// cast the Lock type
	D3D11_MAP mapType;
	switch (locktype)
	{
	case eVBLOCK_DISCARD:
		mapType = D3D11_MAP_WRITE_DISCARD;
		break;
	case eVBLOCK_NOOVERWRITE:
		mapType = D3D11_MAP_WRITE_NO_OVERWRITE;		
		break;
	case eVBLOCK_KEEP:
		mapType = D3D11_MAP_READ_WRITE;
		break;
	default:
		mapType = D3D11_MAP_WRITE_NO_OVERWRITE;
	}

	D3D11_MAPPED_SUBRESOURCE mappedVBResource;
	if (Failure(pDXRenderer->GetD3D11DeviceContext()->Map(m_pHWVertexBuffer, 0, mapType, 0, &mappedVBResource)))
	{
		return m_pEngine->LogE("Failed lock mapped HW-VB resource!");
	}
	m_bLocked = true;

	*buf = (SVertex*)mappedVBResource.pData;	

	// everything went well
	return S_SUCCESS;	
}
*/


// --------------------------------------------------------------------------------
S_API SResult DirectX11VertexBuffer::Fill(const SVertex* vertices, const unsigned long nVertices_)
{
	SP_ASSERTR(m_pEngine && m_pRenderer, S_NOTINIT);
	SP_ASSERTR(vertices, S_INVALIDPARAM, "Invalid buffer of vertices given");	
	SP_ASSERTR(nVertices_ < ULONG_MAX, S_INVALIDPARAM, "Invalid number of vertices given (ULONG_MAX)!");

	if (m_Usage == eVBUSAGE_STATIC
		|| m_nVertices < nVertices_ // need resize
		|| !IS_VALID_PTR(m_pHWVertexBuffer) || !IS_VALID_PTR(m_pShadowBuffer))
	{
		// Have to recreate buffer as this is a static one, has to be resized or never has been created
		if (IS_VALID_PTR(m_pHWVertexBuffer))
		{
			m_pHWVertexBuffer->Release();
			m_pHWVertexBuffer = 0;
		}

		if (IS_VALID_PTR(m_pShadowBuffer))
		{
			free(m_pShadowBuffer);
			m_pShadowBuffer = 0;
		}

		if (Failure(Create(vertices, nVertices_)))
			return EngLog(S_ERROR, m_pEngine, "Cannot fill static vertex buffer: Could not recreate buffer.");		
	}
	else
	{
		// first, update shadow buffer
		memcpy(m_pShadowBuffer, vertices, nVertices_ * sizeof(SVertex));

		// clear the rest
		if (nVertices_ < m_nVertices)
			memset(m_pShadowBuffer + nVertices_, 0, (m_nVertices - nVertices_ - 1) * sizeof(SVertex));


		// upload vertices
		DirectX11Renderer* pDXRenderer = (DirectX11Renderer*)m_pRenderer;
		if (!IS_VALID_PTR(pDXRenderer))
			return EngLog(S_ERROR, m_pEngine, "Cannot upload vertex data after fill: Renderer is invalid (not a DX11 one?)!");

		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		if (FAILED(pDXRenderer->GetD3D11DeviceContext()->Map(m_pHWVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource)))
			return EngLog(S_ERROR, m_pEngine, "Could not map vertex buffer subresource to upload vertex data after fill!");

		memcpy(mappedSubresource.pData, m_pShadowBuffer, m_nVertices * sizeof(SVertex));

		pDXRenderer->GetD3D11DeviceContext()->Unmap(m_pHWVertexBuffer, 0);
	}

	return S_SUCCESS;
}

// --------------------------------------------------------------------------------
S_API SResult DirectX11VertexBuffer::UploadVertexData(unsigned long iVtxStart /* = 0 */, unsigned long nVertices /* = 0 */)
{
	if (!IS_VALID_PTR(m_pHWVertexBuffer) || !IS_VALID_PTR(m_pShadowBuffer) || !IS_VALID_PTR(m_pRenderer) || !IS_VALID_PTR(m_pEngine) || m_Usage == eVBUSAGE_STATIC)
		return S_NOTINITED;

	if (m_nVertices == 0 || m_nVertices < (iVtxStart + nVertices))
		return S_INVALIDPARAM;

	
	DirectX11Renderer* pDXRenderer = (DirectX11Renderer*)m_pRenderer;
	if (!IS_VALID_PTR(pDXRenderer))
		return EngLog(S_ERROR, m_pEngine, "Cannot upload vertex data: Renderer is invalid (not a DX11 one?)!");


	// TODO:
	//	- We're not yet updating only a region in the vertex buffer. To do so, try UpdateSubresource. However
	//	  this requires the use of D3D11_USAGE_DEFAULT instead of DYNAMIC. This makes everything a lot more complicated and
	//	  maybe also a bit slower.

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	if (FAILED(pDXRenderer->GetD3D11DeviceContext()->Map(m_pHWVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource)))
		return EngLog(S_ERROR, m_pEngine, "Could not map vertex buffer subresource to upload vertex data!");

	memcpy(mappedSubresource.pData, m_pShadowBuffer, m_nVertices * sizeof(SVertex));

	pDXRenderer->GetD3D11DeviceContext()->Unmap(m_pHWVertexBuffer, 0);

	return S_SUCCESS;
}

// --------------------------------------------------------------------------------
/*
S_API SResult DirectX11VertexBuffer::Unlock(void)
{
	SP_ASSERTR(IsInited(), S_NOTINIT);
	SP_ASSERTR(m_bLocked, S_ABORTED);
	
	((DirectX11Renderer*)m_pRenderer)->GetD3D11DeviceContext()->Unmap(m_pHWVertexBuffer, 0);
	
	m_bLocked = false;
	return S_SUCCESS;
}
*/

// --------------------------------------------------------------------------------
S_API SResult DirectX11VertexBuffer::Clear(void)
{
	if (IsInited())
	{
		SP_SAFE_RELEASE(m_pHWVertexBuffer);		
		if (m_pShadowBuffer)
			free(m_pShadowBuffer);

		m_nVertices = 0;
		m_pShadowBuffer = 0;
	}

	return S_SUCCESS;
}

// --------------------------------------------------------------------------------
S_API SVertex* DirectX11VertexBuffer::GetShadowBuffer(void)
{
	if (IsInited())
	{
		return m_pShadowBuffer;
	}

	return NULL;
}

// --------------------------------------------------------------------------------
S_API SVertex* DirectX11VertexBuffer::GetVertex(unsigned long iIndex)
{
	if (IsInited() && m_nVertices > iIndex && iIndex >= 0)
	{
		return &m_pShadowBuffer[iIndex];
	}

	return NULL;
}

// --------------------------------------------------------------------------------
S_API unsigned long DirectX11VertexBuffer::GetVertexCount(void) const
{
	return m_nVertices;
}


SP_NMSPACE_END
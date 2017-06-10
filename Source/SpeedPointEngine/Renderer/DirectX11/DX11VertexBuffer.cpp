//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2016 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "DX11VertexBuffer.h"
#include "DX11Renderer.h"
#include <Common\SVertex.h>

SP_NMSPACE_BEG

S_API DX11VertexBuffer::DX11VertexBuffer()
: m_pRenderer(NULL),
m_pHWVertexBuffer(NULL),
m_pShadowBuffer(NULL),
m_nVertices(0),
m_Usage(eVBUSAGE_STATIC)
{
}


S_API DX11VertexBuffer::DX11VertexBuffer(const DX11VertexBuffer& o)
: m_pRenderer(o.m_pRenderer),
m_pHWVertexBuffer(o.m_pHWVertexBuffer),
m_pShadowBuffer(o.m_pShadowBuffer),
m_nVertices(o.m_nVertices),
m_Usage(o.m_Usage)
{
}

// -----------------------------------------------------------------------------------------------
S_API DX11VertexBuffer::~DX11VertexBuffer()
{
	if (m_RefCount > 0)
		CLog::Log(S_WARNING, "Warning: Destructing vertex buffer with refcount = %u", m_RefCount);

	Clear();
}

// -----------------------------------------------------------------------------------------------
S_API bool DX11VertexBuffer::IsInited(void)
{
	return m_pRenderer && m_pHWVertexBuffer && m_pShadowBuffer;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11VertexBuffer::Initialize(
						IRenderer* renderer,
						EVBUsage usage,																						
						const SVertex* pInitialData /* = nullptr */,
						const unsigned long nInitialVertices /* = 0 */)
{
	if (IsInited())
		Clear();

	if (!IS_VALID_PTR((m_pRenderer = renderer)))
	{
		return CLog::Log(S_ERROR, "Invalid ptr to renderer given in DX11VertexBuffer::Initialize!");		
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

// -----------------------------------------------------------------------------------------------
S_API SDX11VBCreateFlags DX11VertexBuffer::GetCreateFlags()
{
	switch (m_Usage)
	{
	case eVBUSAGE_STATIC:
		return SDX11VBCreateFlags(
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_VERTEX_BUFFER,
			0);

	case eVBUSAGE_DYNAMIC_RARE:
		// TODO:
		//	Create and use D3D11_USAGE_STAGING for a second buffer.
		//	Fill the second buffer with Map() and Unmap().
		//	Use ID3D11DeviceContext::CopyResource to copy from second buffer to actual buffer.
		return SDX11VBCreateFlags(
			D3D11_USAGE_DYNAMIC,
			D3D11_BIND_VERTEX_BUFFER,
			D3D11_CPU_ACCESS_WRITE);

	case eVBUSAGE_DYNAMIC_FREQUENT:		
		return SDX11VBCreateFlags(
			D3D11_USAGE_DYNAMIC,
			D3D11_BIND_VERTEX_BUFFER,
			D3D11_CPU_ACCESS_WRITE);

	default:
		CLog::Log(S_WARN, "Tried to retrieve vb create flags for u-nsupported vb usage!");
		return SDX11VBCreateFlags();
	}
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11VertexBuffer::Create(const SVertex* pInitialData, const unsigned long nInitialDataCount)
{
	SP_ASSERTR(m_pRenderer, S_NOTINIT);

	if (!IS_VALID_PTR(pInitialData))		
		return CLog::Log(S_INVALIDPARAM, "Cannot create DX11 Vertex Buffer with invaid initial data pointer!");

	if (nInitialDataCount == 0)
		return CLog::Log(S_ERROR, "Cannot create DX11 Vertex Buffer with data count 0!");

	if (m_pHWVertexBuffer || m_pShadowBuffer)
		return CLog::Log(S_ERROR, "Cannot Create DX11 Vertex Buffer: Already created.");

	
	DX11Renderer* pDXRenderer = dynamic_cast<DX11Renderer*>(m_pRenderer);
	SP_ASSERTR(pDXRenderer, S_NOTINIT);

	m_nVertices = nInitialDataCount;
	m_pShadowBuffer = (SVertex*)(malloc(m_nVertices * sizeof(SVertex)));	

	// copy initial data to RAM copy
	memcpy(m_pShadowBuffer, pInitialData, sizeof(SVertex) * nInitialDataCount);	


	// Create Hardware Buffer:

	D3D11_BUFFER_DESC bufferDesc;
	SDX11VBCreateFlags createFlags = GetCreateFlags();
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
		return CLog::Log(S_ERROR, "Could not create Vertex buffer resource!");
	}

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11VertexBuffer::Fill(const SVertex* vertices, const unsigned long nVertices_)
{
	SP_ASSERTR(m_pRenderer, S_NOTINIT);
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
			return CLog::Log(S_ERROR, "Cannot fill static vertex buffer: Could not recreate buffer.");
	}
	else
	{
		// first, update shadow buffer
		memcpy(m_pShadowBuffer, vertices, nVertices_ * sizeof(SVertex));

		// clear the rest
		if (nVertices_ < m_nVertices)
			memset(m_pShadowBuffer + nVertices_, 0, (m_nVertices - nVertices_ - 1) * sizeof(SVertex));


		// upload vertices
		DX11Renderer* pDXRenderer = dynamic_cast<DX11Renderer*>(m_pRenderer);
		if (!IS_VALID_PTR(pDXRenderer))
			return CLog::Log(S_ERROR, "Cannot upload vertex data after fill: Renderer is invalid (not a DX11 one?)!");

		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		if (FAILED(pDXRenderer->GetD3D11DeviceContext()->Map(m_pHWVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource)))
			return CLog::Log(S_ERROR, "Could not map vertex buffer subresource to upload vertex data after fill!");

		memcpy(mappedSubresource.pData, m_pShadowBuffer, m_nVertices * sizeof(SVertex));

		pDXRenderer->GetD3D11DeviceContext()->Unmap(m_pHWVertexBuffer, 0);
	}

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API SResult DX11VertexBuffer::UploadVertexData(unsigned long iVtxStart /* = 0 */, unsigned long nVertices /* = 0 */)
{
	if (!IS_VALID_PTR(m_pHWVertexBuffer) || !IS_VALID_PTR(m_pShadowBuffer) || !IS_VALID_PTR(m_pRenderer) || m_Usage == eVBUSAGE_STATIC)
		return S_NOTINITED;

	if (m_nVertices == 0 || m_nVertices < (iVtxStart + nVertices))
		return S_INVALIDPARAM;

	
	DX11Renderer* pDXRenderer = dynamic_cast<DX11Renderer*>(m_pRenderer);
	if (!IS_VALID_PTR(pDXRenderer))
		return CLog::Log(S_ERROR, "Cannot upload vertex data: Renderer is invalid (not a DX11 one?)!");


	// TODO:
	//	- We're not yet updating only a region in the vertex buffer. To do so, try UpdateSubresource. However
	//	  this requires the use of D3D11_USAGE_DEFAULT instead of DYNAMIC. This makes everything a lot more complicated and
	//	  maybe also a bit slower.

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	if (FAILED(pDXRenderer->GetD3D11DeviceContext()->Map(m_pHWVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource)))
		return CLog::Log(S_ERROR, "Could not map vertex buffer subresource to upload vertex data!");

	memcpy(mappedSubresource.pData, m_pShadowBuffer, m_nVertices * sizeof(SVertex));

	pDXRenderer->GetD3D11DeviceContext()->Unmap(m_pHWVertexBuffer, 0);

	return S_SUCCESS;
}

// -----------------------------------------------------------------------------------------------
S_API void DX11VertexBuffer::Clear()
{
	if (IsInited())
	{
		SP_SAFE_RELEASE(m_pHWVertexBuffer);		
		if (m_pShadowBuffer)
			free(m_pShadowBuffer);

		m_nVertices = 0;
		m_pShadowBuffer = 0;
	}
}

// -----------------------------------------------------------------------------------------------
S_API SVertex* DX11VertexBuffer::GetShadowBuffer(void)
{
	if (IsInited())
	{
		return m_pShadowBuffer;
	}

	return NULL;
}

// -----------------------------------------------------------------------------------------------
S_API SVertex* DX11VertexBuffer::GetVertex(unsigned long iIndex)
{
	if (IsInited() && m_nVertices > iIndex && iIndex >= 0)
	{
		return &m_pShadowBuffer[iIndex];
	}

	return NULL;
}

// -----------------------------------------------------------------------------------------------
S_API unsigned long DX11VertexBuffer::GetVertexCount(void) const
{
	return m_nVertices;
}


SP_NMSPACE_END

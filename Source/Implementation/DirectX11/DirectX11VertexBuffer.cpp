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
#include <SSpeedPointEngine.h>

SP_NMSPACE_BEG

S_API DirectX11VertexBuffer::DirectX11VertexBuffer()
: m_pEngine(NULL),
m_pRenderer(NULL),
m_pHWVertexBuffer(NULL),
m_pShadowBuffer(NULL),
m_nVertices(0),
m_nVerticesWritten(0),
m_bDynamic(false),
m_bLocked(false)
{
}


S_API DirectX11VertexBuffer::DirectX11VertexBuffer(const DirectX11VertexBuffer& o)
: m_pEngine(o.m_pEngine),
m_pRenderer(o.m_pRenderer),
m_pHWVertexBuffer(o.m_pHWVertexBuffer),
m_pShadowBuffer(o.m_pShadowBuffer),
m_nVertices(o.m_nVertices),
m_nVerticesWritten(o.m_nVerticesWritten),
m_bDynamic(o.m_bDynamic),
m_bLocked(o.m_bLocked)
{
}


// --------------------------------------------------------------------------------
S_API DirectX11VertexBuffer::~DirectX11VertexBuffer()
{
	// Make sure resources are freed
	Clear();
}

// --------------------------------------------------------------------------------
S_API BOOL DirectX11VertexBuffer::IsInited(void)
{
	return
		m_pEngine &&
		m_pRenderer &&
		m_pHWVertexBuffer &&
		m_pShadowBuffer;
}

// --------------------------------------------------------------------------------
S_API SResult DirectX11VertexBuffer::Initialize(int nSize, bool bDyn, SpeedPointEngine* engine, IRenderer* renderer)
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
S_API SResult DirectX11VertexBuffer::Create(int nVertices_, bool bDynamic_)
{
	SP_ASSERTR(m_pEngine && m_pRenderer, S_NOTINIT);			

	m_nVertices = nVertices_;
	m_bDynamic = bDynamic_;

	DirectX11Renderer* pDXRenderer = (DirectX11Renderer*)m_pRenderer;


	m_pShadowBuffer = (SVertex*)(malloc(m_nVertices * sizeof(SVertex)));

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.ByteWidth = m_nVertices * sizeof(SVertex);
	bufferDesc.CPUAccessFlags = 0;
	if (bDynamic_)
		bufferDesc.CPUAccessFlags = /*D3D11_CPU_ACCESS_READ | */D3D11_CPU_ACCESS_WRITE;
	bufferDesc.StructureByteStride = 0;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	if (bDynamic_)
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
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
S_API SResult DirectX11VertexBuffer::Resize(int nVertices_)
{
	if (nVertices_ == m_nVertices)
		return S_SUCCESS;

	SP_ASSERTR(m_pEngine && m_pRenderer, S_NOTINIT);	

	int nVerticesOld = m_nVertices;
	m_nVertices = nVertices_;

	DirectX11Renderer* pDXRenderer = (DirectX11Renderer*)m_pRenderer;


	// Resize Sys memory
	void* vBackup = (void*)(malloc(nVerticesOld * sizeof(SVertex)));
	memcpy(vBackup, m_pShadowBuffer, sizeof(SVertex)* nVerticesOld);
	m_pShadowBuffer = (SVertex*)(malloc(m_nVertices * sizeof(SVertex)));
	memcpy(m_pShadowBuffer, vBackup, sizeof(SVertex)* nVerticesOld);



	// Setup description of the new buffer
	D3D11_BUFFER_DESC bufferDesc;	
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.ByteWidth = nVertices_ * sizeof(SVertex);
	bufferDesc.CPUAccessFlags = 0;
	if (m_bDynamic)
		bufferDesc.CPUAccessFlags = /*D3D11_CPU_ACCESS_READ | */D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	if (m_bDynamic)
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;


	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = m_pShadowBuffer;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	SP_SAFE_RELEASE(m_pHWVertexBuffer);

	if (Failure(pDXRenderer->GetD3D11Device()->CreateBuffer(&bufferDesc, &vertexData, &m_pHWVertexBuffer)))
	{
		return m_pEngine->LogE("Failed create resized vertex buffer!");
	}	

	return S_SUCCESS;
}

// --------------------------------------------------------------------------------
S_API SResult DirectX11VertexBuffer::Lock(UINT iBegin, UINT iLength, SVertex** buf)
{
	return Lock(iBegin, iLength, buf, eVBLOCK_NOOVERWRITE);
}

// --------------------------------------------------------------------------------
S_API SResult DirectX11VertexBuffer::Lock(UINT iBegin, UINT iLength, SVertex** buf, EVBLockType locktype)
{
	SP_ASSERTR(m_pEngine, S_NOTINIT);
	SP_ASSERTR(buf, S_INVALIDPARAM);

	// just don't allow HW Buffer updates if not dynamic
	if (!m_bDynamic)
		return S_ABORTED;

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


// --------------------------------------------------------------------------------
S_API SResult DirectX11VertexBuffer::Fill(SVertex* vertices, int nVertices_, bool append)
{
	SP_ASSERTR(m_pEngine && IsInited() && m_pHWVertexBuffer, S_NOTINIT);
	SP_ASSERTR(vertices, S_INVALIDPARAM);
	
	if (!m_bDynamic && nVertices_ > m_nVertices)
		return m_pEngine->LogReport(S_ABORTED, "Cannot fill non-dynamic vertex buffer: Buffer overflow!");

	if (append && m_nVerticesWritten > 0 && !m_bDynamic)
		return m_pEngine->LogReport(S_ABORTED, "Cannot append Vertex data to non-dynamic Vertex Buffer");

	if (nVertices_ + m_nVerticesWritten > m_nVertices)
	{
		// Resize
		if (Failure(Resize(nVertices_ + m_nVerticesWritten)))
			return m_pEngine->LogE("Resize for fill vertex buffer failed!");
	}

	// Lock buffer if not happened
	SVertex* pVert = 0;
	if (!m_bLocked)
	{
		SResult lockResult = Lock((UINT)(m_nVerticesWritten * sizeof(SVertex)), (UINT)(nVertices_ * sizeof(SVertex)), &pVert);
		if (Failure(lockResult) || pVert == 0)
			return m_pEngine->LogE("Could not lock vertex buffer to fill with vertex data!");
	}

	// Now copy data		
	memcpy(pVert, (void*)vertices, nVertices_ * sizeof(SVertex));

	// copy data to our copy in sys mem (shadow buffer)
	memcpy((void*)&m_pShadowBuffer[m_nVerticesWritten], (void*)vertices, nVertices_ * sizeof(SVertex));

	m_nVerticesWritten += nVertices_;

	// ... and unlock
	if (Failure(Unlock()))
		return S_ERROR;	


	/*
	D3DVERTEXBUFFER_DESC dsc;
	pHWVertexBuffer->GetDesc(&dsc);
	int vbs = dsc.Size;
	*/

	return S_SUCCESS;
}

// --------------------------------------------------------------------------------
S_API SResult DirectX11VertexBuffer::Unlock(void)
{
	SP_ASSERTR(IsInited(), S_NOTINIT);
	SP_ASSERTR(m_bLocked, S_ABORTED);
	
	((DirectX11Renderer*)m_pRenderer)->GetD3D11DeviceContext()->Unmap(m_pHWVertexBuffer, 0);
	
	m_bLocked = false;
	return S_SUCCESS;
}

// --------------------------------------------------------------------------------
S_API SResult DirectX11VertexBuffer::Clear(void)
{
	if (IsInited())
	{
		SP_SAFE_RELEASE(m_pHWVertexBuffer);		
		if (m_pShadowBuffer)
			free(m_pShadowBuffer);

		m_nVertices = 0;
		m_nVerticesWritten = 0;
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
S_API SVertex* DirectX11VertexBuffer::GetVertex(int iIndex)
{
	if (IsInited() && m_nVertices > iIndex && iIndex >= 0)
	{
		return &m_pShadowBuffer[iIndex];
	}

	return NULL;
}

// --------------------------------------------------------------------------------
S_API INT DirectX11VertexBuffer::GetVertexCount(void)
{
	return m_nVertices;
}


SP_NMSPACE_END
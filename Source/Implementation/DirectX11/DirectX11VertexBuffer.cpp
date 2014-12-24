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
#include <SpeedPointEngine.h>

SP_NMSPACE_BEG

S_API DirectX11VertexBuffer::DirectX11VertexBuffer()
: m_pEngine(NULL),
m_pRenderer(NULL),
m_pHWVertexBuffer(NULL),
m_pShadowBuffer(NULL),
m_nVertices(0),
m_nVerticesWritten(0),
m_Usage(eVBUSAGE_STATIC),
m_bInitialDataWritten(false),
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
m_Usage(o.m_Usage),
m_bInitialDataWritten(o.m_bInitialDataWritten),
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
S_API SResult DirectX11VertexBuffer::Initialize(IGameEngine* pEngine,
						IRenderer* renderer,
						EVBUsage usage,
						int nSize,																	
						SVertex* pInitialData /* = nullptr */)
{
	if (IsInited())
		Clear();

	m_pEngine = pEngine;
	m_pRenderer = renderer;
	m_Usage = usage;

	if (usage == eVBUSAGE_STATIC)
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
S_API SDirectX11VBCreateFlags DirectX11VertexBuffer::GetCreateFlags()
{
	switch (m_Usage)
	{
	case eVBUSAGE_STATIC:
		return SDirectX11VBCreateFlags(
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_VERTEX_BUFFER,
			0);

	default:
		m_pEngine->LogW("Tried to retrieve vb create flags for unsupported vb usage!");
		return SDirectX11VBCreateFlags();
	}
}

// --------------------------------------------------------------------------------
S_API SResult DirectX11VertexBuffer::Create(int nVertices_, SVertex* pInitialData /* = 0 */, usint32 nInitialDataCount /* = 0 */)
{
	SP_ASSERTR(m_pEngine && m_pRenderer, S_NOTINIT);			
	SP_ASSERTR(nVertices_ > 0, S_INVALIDPARAM);
	SP_ASSERTR(nInitialDataCount <= (usint32)nVertices_, S_INVALIDPARAM);

	if (m_pHWVertexBuffer || m_pShadowBuffer)
		return m_pEngine->LogE("Tried to create existing VB - Do a clear first!");

	if (m_Usage == eVBUSAGE_STATIC && !pInitialData)
		return m_pEngine->LogE("Cannot create static VB without given initial Data!");

	
	DirectX11Renderer* pDXRenderer = (DirectX11Renderer*)m_pRenderer;

	m_nVertices = nVertices_;
	m_pShadowBuffer = (SVertex*)(malloc(m_nVertices * sizeof(SVertex)));	
	if (pInitialData && nInitialDataCount)
	{		
		memcpy(m_pShadowBuffer, pInitialData, sizeof(SVertex) * nInitialDataCount);
		if ((usint32)m_nVertices > nInitialDataCount)
			memset(m_pShadowBuffer + nInitialDataCount - 1, 0, (m_nVertices - nInitialDataCount));

		m_nVerticesWritten = nInitialDataCount;
		m_bInitialDataWritten = true;
	}
	else
	{
		// zero memory
		memset(m_pShadowBuffer, 0, sizeof(SVertex) * m_nVertices);
		m_nVerticesWritten = 0;
	}

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
S_API SResult DirectX11VertexBuffer::Resize(int nVertices_)
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
S_API SResult DirectX11VertexBuffer::Fill(SVertex* vertices, int nVertices_, bool bAppend)
{
	SP_ASSERTR(m_pEngine && m_pRenderer, S_NOTINIT);
	SP_ASSERTR(vertices, S_INVALIDPARAM);
	SP_ASSERTR(!m_bLocked, S_ABORTED);
	
	if (m_Usage == eVBUSAGE_STATIC)
	{


		if (m_bInitialDataWritten)
			return m_pEngine->LogReport(S_ABORTED, "Cannot fill static buffer which is already filled!");
		

		// this Fill()-Call writes initial data so create the buffer with the initial data
		SResult createRes;
		if (Failure((createRes = Create(nVertices_, vertices, nVertices_))))
			return createRes;

		m_nVerticesWritten = nVertices_;
		m_bInitialDataWritten = true;

		return S_SUCCESS;

	}


	// else:

	usint32 iBegin;
	EVBLockType lockType;

	if (bAppend) // add to end of existing data
	{
		if (nVertices_ + m_nVerticesWritten >
			m_nVertices) // m_nVertices actually stores the size of the existing VB, or 0 if not existing
		{


			// Resize
			// also creates the buffer if not existing yet
			if (Failure(Resize(nVertices_ + m_nVerticesWritten)))
				return m_pEngine->LogE("Resize for fill vertex buffer failed (append)!");			

		}

		iBegin = m_nVerticesWritten;
		lockType = eVBLOCK_NOOVERWRITE;
	}
	else // overwrite old data
	{
		if (nVertices_ > m_nVertices)
		{

			// also creates the buffer if not existing yet
			if (Failure(Resize(nVertices_)))
				return m_pEngine->LogE("Resize for fill vertex buffer failed!");			

		}

		iBegin = 0;	
		lockType = eVBLOCK_DISCARD;
	}


	// Lock buffer if not happened
	SVertex* pVert = 0;		
	SResult lockResult = Lock(iBegin * sizeof(SVertex), (UINT)(nVertices_ * sizeof(SVertex)), &pVert, lockType);
		
	if (Failure(lockResult) || pVert == 0)
		return m_pEngine->LogE("Could not lock vertex buffer to fill with vertex data!");
	

	// Now copy data		
	memcpy(pVert, (void*)vertices, nVertices_ * sizeof(SVertex));

	// copy data to our copy in sys mem (shadow buffer)
	memcpy((void*)&m_pShadowBuffer[iBegin], (void*)vertices, nVertices_ * sizeof(SVertex));

	m_nVerticesWritten = (int)iBegin + nVertices_;
	m_bInitialDataWritten = true;


	// ... and unlock	
	if (Failure(Unlock()))
		return S_ERROR;

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
S_API unsigned long DirectX11VertexBuffer::GetVertexCount(void) const
{
	return m_nVertices;
}


SP_NMSPACE_END
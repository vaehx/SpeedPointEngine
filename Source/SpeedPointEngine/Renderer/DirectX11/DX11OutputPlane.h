//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2016 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\IOutputPlane.h"
#include <Common\Mat44.h>
#include <Common\SPrerequisites.h>

SP_NMSPACE_BEG

class S_API DX11Renderer;
struct S_API IVertexBuffer;
struct S_API IIndexBuffer;
class S_API DX11Shader;



//////////////////////////////////////////////////////////////////////////////////////////////////


class DX11OutputPlane : public IOutputPlane
{
private:
	DX11Renderer* m_pDXRenderer;
	IVertexBuffer* m_pVertexBuffer;
	IIndexBuffer* m_pIndexBuffer;

	Mat44 m_mtxProjection;
	Mat44 m_mtxView;	

public:
	// Default constructor
	DX11OutputPlane()
		: m_pDXRenderer(0),
		m_pVertexBuffer(0),
		m_pIndexBuffer(0)
	{
	}

	~DX11OutputPlane()
	{
		Clear();
	}


	// Initialize the output plane
	virtual SResult Initialize(IRenderer* renderer, int nW, int nH);

	virtual bool IsInitialized();

	// Render the output plane using the renderer specified when initializing
	virtual SResult Render(IFBO* pGBufferAlbedo, IFBO* pLightingBuffer);

	// Clearout the output plane
	virtual SResult Clear(void);

	virtual IVertexBuffer* GetVertexBuffer()
	{
		return m_pVertexBuffer;
	}

	virtual IIndexBuffer* GetIndexBuffer()
	{
		return m_pIndexBuffer;
	}

	virtual Mat44& GetProjectionMatrix()
	{
		return m_mtxProjection;
	}
	virtual Mat44& GetViewMatrix()
	{
		return m_mtxView;
	}

	virtual usint32 GetIndexCount();
};




//////////////////////////////////////////////////////////////////////////////////////////////////

SP_NMSPACE_END

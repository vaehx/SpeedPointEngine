//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2011-2014, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Abstract\IOutputPlane.h>
#include <Abstract\Matrix.h>
#include <Abstract\SPrerequisites.h>

SP_NMSPACE_BEG

class S_API DirectX11Renderer;
struct S_API IVertexBuffer;
struct S_API IIndexBuffer;
class S_API DirectX11Shader;



//////////////////////////////////////////////////////////////////////////////////////////////////


class DirectX11OutputPlane : public IOutputPlane
{
private:
	DirectX11Renderer* m_pDXRenderer;
	IVertexBuffer* m_pVertexBuffer;
	IIndexBuffer* m_pIndexBuffer;

	SMatrix m_mtxProjection;
	SMatrix m_mtxView;	

public:
	// Default constructor
	DirectX11OutputPlane()
		: m_pDXRenderer(0),
		m_pVertexBuffer(0),
		m_pIndexBuffer(0)
	{
	}

	~DirectX11OutputPlane()
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

	virtual SMatrix& GetProjectionMatrix()
	{
		return m_mtxProjection;
	}
	virtual SMatrix& GetViewMatrix()
	{
		return m_mtxView;
	}

	virtual usint32 GetIndexCount();
};




//////////////////////////////////////////////////////////////////////////////////////////////////

SP_NMSPACE_END
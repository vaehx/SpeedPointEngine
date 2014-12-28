//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2014, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	RawGeometrical.h
// Created:	8/18/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#ifndef __rawgeometrical_h__
#define __rawgeometrical_h__

#if _MSC_VER > 1000
#pragma once
#endif


#include <SPrerequisites.h>
#include <Abstract\IGeometry.h>

SP_NMSPACE_BEG


// Summary:
//	This is the raw geometry (without material)
class S_API Geometry : public IGeometry
{
protected:
	IGameEngine* m_pEngine;
	IRenderer* m_pRenderer;

	unsigned short m_nIndexBuffers;
	SGeometryIndexBuffer* m_pIndexBuffers;

	IVertexBuffer* m_pVertexBuffer;

	EPrimitiveType m_PrimitiveType;

public:
	Geometry();
	virtual ~Geometry();

	virtual SResult Init(IGameEngine* pEngine, IRenderer* pRenderer, SInitialGeometryDesc* pInitialGeom = nullptr);

	virtual IRenderer* GetRenderer()
	{
		return m_pRenderer;
	}

	virtual SGeometryIndexBuffer* GetIndexBuffers()
	{
		return m_pIndexBuffers;
	}
	virtual unsigned short GetIndexBufferCount() const
	{
		return m_nIndexBuffers;
	}
	virtual IVertexBuffer* GetVertexBuffer()
	{
		return m_pVertexBuffer;
	}

	virtual SVertex* GetVertex(unsigned long index);
	virtual SIndex* GetIndex(unsigned long index);

	virtual SVertex* GetVertices();	

	virtual unsigned long GetVertexCount() const;
	virtual unsigned long GetIndexCount() const;

	
	virtual void Clear();

	virtual SResult HandleShutdown()
	{
		Clear();
		return S_SUCCESS;
	}
	virtual SString GetShutdownHandlerDesc() const
	{
		return SString("Geometry");
	}

	virtual EPrimitiveType GetPrimitiveType() const
	{
		return m_PrimitiveType;
	}

	virtual SResult CalculateNormalsGeometry(SInitialGeometryDesc& dsc, float fLineLength = 0.1f) const;
};


SP_NMSPACE_END
#endif
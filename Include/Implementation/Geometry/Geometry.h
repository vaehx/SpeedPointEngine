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
	IRenderer* m_pRenderer;

	SP_ID m_iIndexBuffer;
	SP_ID m_iVertexBuffer;	

public:
	Geometry();
	virtual ~Geometry();

	virtual SResult Init(IGameEngine* pEngine, IRenderer* pRenderer, SInitialGeometryDesc* pInitialGeom = nullptr);

	virtual IRenderer* GetRenderer()
	{
		return m_pRenderer;
	}

	virtual IIndexBuffer* GetIndexBuffer() const;
	virtual const SP_ID& GetIndexBufferResIndex() const { return m_iIndexBuffer; }
	virtual IVertexBuffer* GetVertexBuffer() const;
	virtual const SP_ID& GetVertexBufferResIndex() const { return m_iVertexBuffer; }
	
	virtual void Clear();
};


SP_NMSPACE_END
#endif
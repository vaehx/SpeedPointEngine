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
#include <Abstract\IGeometrical.h>

SP_NMSPACE_BEG


// Summary:
//	This is a raw geometrical object. You might want to inherit this and thereby avoid implementing all methods of IGeometrical.
class S_API RawGeometrical : public IGeometrical
{
protected:
	IRenderer* m_pRenderer;

	SP_ID m_iIndexBuffer;
	SP_ID m_iVertexBuffer;
	IMaterial* m_pMaterial;

public:
	RawGeometrical();
	virtual ~RawGeometrical();

	virtual SResult Init(IGameEngine* pEngine, IRenderer* pRenderer, SInitialGeometryDesc* pInitialGeom = nullptr);

	virtual IIndexBuffer* GetIndexBuffer() const;
	virtual const SP_ID& GetIndexBufferResIndex() const { return m_iIndexBuffer; }
	virtual IVertexBuffer* GetVertexBuffer() const;
	virtual const SP_ID& GetVertexBufferResIndex() const { return m_iVertexBuffer; }

	virtual EGeometryObjectType GetType() const
	{
		return eGEOMOBJ_RAW;
	}

	virtual IMaterial* GetMaterial() const
	{
		return m_pMaterial;
	}

	virtual SResult Render();
	virtual void Clear();
};


SP_NMSPACE_END
#endif
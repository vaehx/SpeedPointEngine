//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2014, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	IGeometry.h
// Created:	8/18/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#ifndef __igeometrical_h__
#define __igeometrical_h__

#if _MSC_VER > 1000
#pragma once
#endif


#include <SPrerequisites.h>
#include "IIndexBuffer.h"	// because we cannot forward-declare SIndex (due to delete command)
#include <Util\SVertex.h>	// because we cannot forward-declare SVertex (due to delete command)
#include <Util\SMatrix.h>
#include "IGameEngine.h"	// for IShutdownHandler
SP_NMSPACE_BEG

struct S_API ITexture;
struct S_API IVertexBuffer;

///////////////////////////////////////////////////////////////////////////////////

// Note:
//	This structure deletes the vertices and indices arrays when being destructed!
struct S_API SInitialGeometryDesc
{
	SVertex* pVertices;	// gets deleted automatically when constructed. Otherwise make sure to set pointer to 0
	usint32 nVertices;

	SIndex* pIndices;	// gets deleted automatically when constructed. Otherwise make sure to set pointer to 0
	usint32 nIndices;

	SInitialGeometryDesc()
		: pVertices(nullptr),
		nVertices(0),
		pIndices(nullptr),
		nIndices(0)
	{
	}

	~SInitialGeometryDesc()
	{
		if (IS_VALID_PTR(pVertices))	
			delete pVertices;	

		if (IS_VALID_PTR(pIndices))
			delete pIndices;
	}
};

///////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////


// Summary:
//	Transformation description
struct S_API STransformationDesc
{
	SMatrix4 translation;
	SMatrix4 rotation;
	SMatrix4 scale;

	STransformationDesc() {}
	STransformationDesc(const SMatrix& mtxTranslation, const SMatrix& mtxRotation, const SMatrix& mtxScale)
		: translation(mtxTranslation), rotation(mtxRotation), scale(mtxScale)
	{
	}

	STransformationDesc(const STransformationDesc& o)
		: translation(o.translation), rotation(o.rotation), scale(o.scale)
	{
	}

	SMatrix4 BuildTRS() const
	{
		return translation * rotation * scale;
	}

	SMatrix4 BuildSRT() const
	{
		return scale * rotation * translation;
	}
};


///////////////////////////////////////////////////////////////////////////////////

struct S_API IGeometry : public IShutdownHandler
{
	virtual ~IGeometry() {}

	virtual SResult Init(IGameEngine* pEngine, IRenderer* pRenderer, SInitialGeometryDesc* pInitialGeom = nullptr) = 0;	
	
	virtual IRenderer* GetRenderer() = 0;

	virtual IIndexBuffer* GetIndexBuffer() = 0;	
	virtual IVertexBuffer* GetVertexBuffer() = 0;	

	virtual void Clear() = 0;
};

SP_NMSPACE_END
#endif
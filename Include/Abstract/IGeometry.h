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
#include "Matrix.h"
//#include "Material.h"
#include "IGameEngine.h"	// for IShutdownHandler

SP_NMSPACE_BEG

struct S_API ITexture;
struct S_API IVertexBuffer;
struct S_API SMaterial;
struct SAxisAlignedBoundBox;
typedef struct S_API SAxisAlignedBoundBox AABB;

///////////////////////////////////////////////////////////////////////////////////

// Summary:
//	Stores a list of indices to indices that are affected by specific material.
struct S_API SMaterialIndices
{
	char* materialName;


	// TODO: Make this more abstract to avoid having invalid pointers
	SMaterial* pMaterial;	// accumulation



	unsigned long* pIdxIndices;	// array of indices to indices
	unsigned int nIdxIndices;
	unsigned int nFilledIdxIndices;

	SMaterialIndices()
		: nIdxIndices(0),
		pIdxIndices(0),
		materialName(0),
		nFilledIdxIndices(0),
		pMaterial(0) {}

	~SMaterialIndices()
	{
		if (IS_VALID_PTR(pIdxIndices))
			delete[] pIdxIndices;

		if (IS_VALID_PTR(materialName))
			delete[] materialName;

		materialName = 0;
		pIdxIndices = 0;
		nIdxIndices = 0;
		nFilledIdxIndices = 0;
		pMaterial = 0;
	}
};

///////////////////////////////////////////////////////////////////////////////////


enum S_API EPrimitiveType
{
	PRIMITIVE_TYPE_TRIANGLELIST,
	PRIMITIVE_TYPE_LINES,
	PRIMITIVE_TYPE_UNKNOWN
};

enum S_API EGeomUsage
{
	eGEOMUSE_STATIC,
	eGEOMUSE_MODIFY_FREQUENTLY,
	eGEOMUSE_MODIFY_RARELY
};


// Note:
//	This structure deletes the vertices and indices arrays when being destructed!
struct S_API SInitialGeometryDesc
{
	EGeomUsage vertexUsage;
	EGeomUsage indexUsage;

	SVertex* pVertices;	// gets deleted automatically when constructed. Otherwise make sure to set pointer to 0
	usint32 nVertices;

	SIndex* pIndices;	// gets deleted automatically when constructed. Otherwise make sure to set pointer to 0
	usint32 nIndices;

	SMaterial* pSingleMaterial;

	SMaterialIndices* pMatIndexAssigns;
	unsigned int nMatIndexAssigns;

	bool bRequireNormalRecalc;
	bool bRequireTangentRecalc;

	EPrimitiveType primitiveType;

	SInitialGeometryDesc()
		: vertexUsage(eGEOMUSE_STATIC),
		indexUsage(eGEOMUSE_STATIC),
		pVertices(nullptr),
		nVertices(0),
		pIndices(nullptr),
		nIndices(0),
		pMatIndexAssigns(0),
		nMatIndexAssigns(0),
		pSingleMaterial(0),
		bRequireNormalRecalc(false),
		bRequireTangentRecalc(false),
		primitiveType(PRIMITIVE_TYPE_TRIANGLELIST)
	{
	}

	~SInitialGeometryDesc()
	{
		if (IS_VALID_PTR(pVertices))	
			delete[] pVertices;			

		if (IS_VALID_PTR(pIndices))
			delete[] pIndices;

		if (IS_VALID_PTR(pMatIndexAssigns))
			delete[] pMatIndexAssigns;

		pVertices = 0;
		pIndices = 0;
		nVertices = 0;
		nIndices = 0;
		nMatIndexAssigns = 0;
		pSingleMaterial = 0;
		bRequireNormalRecalc = false;
		bRequireTangentRecalc = false;
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

// Summary:
//	Binds an Index Buffer to a specific material
struct S_API SGeometryIndexBuffer
{
	IIndexBuffer* pIndexBuffer;
	SMaterial* pMaterial;
};



///////////////////////////////////////////////////////////////////////////////////

struct S_API IGeometry : public IShutdownHandler
{
	virtual ~IGeometry() {}

	virtual SResult Init(IGameEngine* pEngine, IRenderer* pRenderer, SInitialGeometryDesc* pInitialGeom = nullptr) = 0;	
	
	virtual IRenderer* GetRenderer() = 0;

	virtual SGeometryIndexBuffer* GetIndexBuffers() = 0;
	virtual unsigned short GetIndexBufferCount() const = 0;
	virtual IVertexBuffer* GetVertexBuffer() = 0;	

	virtual SVertex* GetVertex(unsigned long index) = 0;
	virtual SIndex* GetIndex(unsigned long index) = 0;

	virtual SVertex* GetVertices() = 0;	

	virtual unsigned long GetVertexCount() const = 0;
	virtual unsigned long GetIndexCount() const = 0;

	virtual EPrimitiveType GetPrimitiveType() const = 0;

	virtual SResult CalculateNormalsGeometry(SInitialGeometryDesc& dsc, float fLineLength = 0.1f) const = 0;

	virtual void CalculateBoundBox(AABB& aabb) = 0;

	virtual void Clear() = 0;
};

SP_NMSPACE_END
#endif
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
struct S_API IMaterial;
struct SAxisAlignedBoundBox;
typedef struct S_API SAxisAlignedBoundBox AABB;

///////////////////////////////////////////////////////////////////////////////////

// Summary:
//	Stores a list of indices to indices that are affected by specific material.
struct S_API SMaterialIndices
{
	char* materialName;


	// TODO: Make this more abstract to avoid having invalid pointers
	IMaterial* pMaterial;	// accumulation



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

	SMaterialIndices(const SMaterialIndices& mi)		
	{
		CopyFrom(mi);		
	}

	SMaterialIndices& operator = (const SMaterialIndices& mi)
	{
		CopyFrom(mi);
		return *this;
	}

	void CopyFrom(const SMaterialIndices& mi)
	{
		nIdxIndices = mi.nIdxIndices;
		nFilledIdxIndices = mi.nFilledIdxIndices;
		pMaterial = mi.pMaterial;

		if (IS_VALID_PTR(mi.materialName))
			sp_strcpy(&materialName, mi.materialName);

		pIdxIndices = 0;
		if (IS_VALID_PTR(mi.pIdxIndices) && nIdxIndices > 0)
		{
			pIdxIndices = new unsigned long[nIdxIndices];
			memcpy(pIdxIndices, mi.pIdxIndices, nIdxIndices);
		}
	}
};

///////////////////////////////////////////////////////////////////////////////////


enum S_API EPrimitiveType
{
	PRIMITIVE_TYPE_TRIANGLELIST,
	PRIMITIVE_TYPE_LINES,
	PRIMITIVE_TYPE_LINESTRIP,
	PRIMITIVE_TYPE_UNKNOWN
};

enum S_API EGeomUsage
{
	eGEOMUSE_STATIC,
	eGEOMUSE_MODIFY_FREQUENTLY,
	eGEOMUSE_MODIFY_RARELY
};


struct S_API SInitialSubsetGeometryDesc
{
	SIndex* pIndices;
	u32 nIndices;
	IMaterial* pMaterial;

	SInitialSubsetGeometryDesc()
		: pIndices(0),
		nIndices(0),
		pMaterial(0)
	{
	}

	~SInitialSubsetGeometryDesc()
	{
		pIndices = 0;
		nIndices = 0;
		pMaterial = 0;
	}
};

struct S_API SInitialGeometryDesc
{
	EGeomUsage vertexUsage;
	EGeomUsage indexUsage;

	SVertex* pVertices;
	usint32 nVertices;

	SInitialSubsetGeometryDesc* pSubsets;
	u16 nSubsets;

	bool bRequireNormalRecalc;
	bool bRequireTangentRecalc;

	EPrimitiveType primitiveType;

	SInitialGeometryDesc()
		: vertexUsage(eGEOMUSE_STATIC),
		indexUsage(eGEOMUSE_STATIC),
		pVertices(nullptr),
		nVertices(0),
		pSubsets(0),
		nSubsets(0),
		bRequireNormalRecalc(false),
		bRequireTangentRecalc(false),
		primitiveType(PRIMITIVE_TYPE_TRIANGLELIST)
	{
	}

	~SInitialGeometryDesc()
	{
		pSubsets = 0;
		nSubsets = 0;
		pVertices = 0;
		nVertices = 0;
		bRequireNormalRecalc = false;
		bRequireTangentRecalc = false;
	}

	// Makes sure a 0-subset exists in this geometry desc and returns a reference to it.
	// If the pSubsets array is not a valid ptr or nSubsets == 0, the pSubsets array is initialized with a single-element array.
	SInitialSubsetGeometryDesc* GetZeroSubset()
	{
		if (!IS_VALID_PTR(pSubsets) || nSubsets == 0)
		{
			pSubsets = new SInitialSubsetGeometryDesc[1];
			nSubsets = 1;
		}

		return &pSubsets[0];
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

// Subset of a geometry
struct S_API SGeomSubset
{
	unsigned long indexOffset;
	IIndexBuffer* pIndexBuffer;	
	IMaterial* pMaterial;

	SGeomSubset()
		: indexOffset(0),
		pIndexBuffer(0),
		pMaterial(0)
	{
	}	
};



///////////////////////////////////////////////////////////////////////////////////

struct S_API IGeometry : public IShutdownHandler
{
	virtual ~IGeometry() {}

	virtual SResult Init(IGameEngine* pEngine, IRenderer* pRenderer, SInitialGeometryDesc* pInitialGeom = nullptr) = 0;	
	
	virtual IRenderer* GetRenderer() = 0;

	virtual SGeomSubset* GetSubsets() = 0;	
	virtual unsigned short GetSubsetCount() const = 0;

	// Returns 0 if not found
	virtual SGeomSubset* GetSubset(unsigned int index) = 0;

	virtual IVertexBuffer* GetVertexBuffer() = 0;	

	virtual SVertex* GetVertex(unsigned long index) = 0;
	virtual SIndex* GetIndex(unsigned long index) = 0;

	virtual SVertex* GetVertices() = 0;	

	virtual unsigned long GetVertexCount() const = 0;
	virtual unsigned long GetIndexCount() const = 0;

	virtual EPrimitiveType GetPrimitiveType() const = 0;	

	virtual SResult CalculateNormalsGeometry(SInitialGeometryDesc& dsc, float fLineLength = 0.1f) const = 0;

	virtual void CalculateBoundBox(AABB& aabb, const SMatrix& transform) = 0;

	virtual void Clear() = 0;
};

SP_NMSPACE_END
#endif
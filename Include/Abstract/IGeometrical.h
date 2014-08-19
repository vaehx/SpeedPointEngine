//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2014, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	IGeometrical.h
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
SP_NMSPACE_BEG

struct S_API IGameEngine;
struct S_API ITexture;
struct S_API IVertexBuffer;

///////////////////////////////////////////////////////////////////////////////////

// Note:
//	This structure deletes the vertices and indices arrays when being destructed!
struct S_API SInitialGeometryDesc
{
	SVertex* pVertices;
	usint32 nVertices;

	SIndex* pIndices;
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

enum S_API EGeometryObjectType
{
	eGEOMOBJ_RAW,
	eGEOMOBJ_VISIBLE,
	eGEOMOBJ_TERRAIN,
	eGEOMOBJ_VEGETATION,
	eGEOMOBJ_WATER
	// ...
};


///////////////////////////////////////////////////////////////////////////////////

struct S_API SMaterialLayerDesc
{
	SP_ID iDiffuseTex;
	SP_ID iSpecularTex;

	SMaterialLayerDesc() {}
	SMaterialLayerDesc(const SP_ID& diffTex, const SP_ID& specTex)
		: iDiffuseTex(diffTex), iSpecularTex(specTex)
	{
	}

	SMaterialLayerDesc(const SMaterialLayerDesc& o)
		: iDiffuseTex(o.iDiffuseTex), iSpecularTex(o.iSpecularTex)
	{
	}
};

typedef usint32 S_API MaterialLayerID;

struct S_API IMaterialLayer
{
	virtual ~IMaterialLayer() {}

	virtual MaterialLayerID GetLayerID() const = 0;
	virtual void Clear() = 0;
};

// Summary:
//	A material wrapper that contains and manages all the material layers
struct S_API IMaterial
{
	virtual ~IMaterial()
	{
	}

	virtual IMaterialLayer* GetLayer(const MaterialLayerID& id) const = 0;
	virtual SResult AddLayer(const SMaterialLayerDesc& layerDesc) = 0;
	virtual SResult DelLayer(const MaterialLayerID& id) = 0;
	virtual void Clear() = 0;
};



///////////////////////////////////////////////////////////////////////////////////


// Summary:
//	Transformation description
struct S_API STransformation
{
	SMatrix4 translation;
	SMatrix4 rotation;
	SMatrix4 scale;

	STransformation() {}
	STransformation(const SMatrix& mtxTranslation, const SMatrix& mtxRotation, const SMatrix& mtxScale)
		: translation(mtxTranslation), rotation(mtxRotation), scale(mtxScale)
	{
	}

	STransformation(const STransformation& o)
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

struct S_API IGeometrical
{
	virtual ~IGeometrical() {}

	virtual SResult Init(IGameEngine* pEngine, IRenderer* pRenderer, SInitialGeometryDesc* pInitialGeom = nullptr) = 0;	
	
	virtual IIndexBuffer* GetIndexBuffer() const = 0;
	virtual const SP_ID& GetIndexBufferResIndex() const = 0;
	virtual IVertexBuffer* GetVertexBuffer() const = 0;
	virtual const SP_ID& GetVertexBufferResIndex() const = 0;

	virtual EGeometryObjectType GetType() const = 0;

	virtual IMaterial* GetMaterial() const = 0;

	virtual SResult Render() = 0;
	virtual void Clear() = 0;
};

// Summary:
//	A mesh provides loading methods, a geometrical does not.
struct S_API IMesh : public IGeometrical
{
	virtual ~IMesh()
	{
	}

//	virtual SResult LoadFrom3DS(SString filename);
};



SP_NMSPACE_END
#endif
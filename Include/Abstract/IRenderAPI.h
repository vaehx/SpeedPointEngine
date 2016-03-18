//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2015, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	IRenderAPI.h
// Created:	6/26/2015 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SPrerequisites.h>
#include "IGeometry.h"

SP_NMSPACE_BEG


// Notice:
// - Constant Buffers must have a size which is a multiple of 16. That means you should
//   only use SMatrix4 or SVector4 (=float4) data types in here. Otherwise, make shure
//   to add proper padding bytes.

struct S_API SObjectConstantsBuffer
{

	// mtxView and mtxProjection could also be in a separate CB, but we want to
	// save memory bandwidth. Also it might be that the camera or projection changes
	// per scene.

	SMatrix4 mtxViewProj;	// 16 * 4 Byte

	// Pos used instead of Dir, to avoid struggling around with angles when calculating
	// sun traveling due to TOD.   sun dir = normalize(-sunPosition)
	float4 sunPosition;		// 4 * 4 Byte

	float4 eyePosition;		// 4 * 4 Byte

	SObjectConstantsBuffer& operator = (const SObjectConstantsBuffer& b)
	{
		mtxViewProj = b.mtxViewProj;
		sunPosition = b.sunPosition;
		eyePosition = b.eyePosition;
		return *this;
	}
};

struct S_API SHelperConstantBuffer
{
	SMatrix4 mtxTransform;	// 16 * 4 Byte
	float3 color;			// 3 * 4 Byte

	float struct_padding;	// 4 Byte
};

struct S_API SMaterialConstantsBuffer
{
	SMatrix4 mtxTransform;	// 4*4 *4 = 4 * 16 Byte
	float matAmbient;		// 4 Byte
	float3 matEmissive;		// 3 * 4 = 12 Byte

	SMaterialConstantsBuffer()
		: matAmbient(0.1f),
		matEmissive(0,0,0)
	{
	}

	SMaterialConstantsBuffer& operator = (const SMaterialConstantsBuffer& b)
	{
		mtxTransform = b.mtxTransform;
		matAmbient = b.matAmbient;
		matEmissive = b.matEmissive;
		return *this;
	}
};

struct S_API STerrainConstantBuffer
{
	float fTerrainDMFadeRadius;		// 4 Byte
	float fTerrainMaxHeight;		// 4 Byte
	unsigned int vtxHeightMapSz;	// 4 Byte
	float segmentSize;				// 4 Byte

	STerrainConstantBuffer& operator = (const STerrainConstantBuffer& b)
	{
		fTerrainDMFadeRadius = b.fTerrainDMFadeRadius;
		fTerrainMaxHeight = b.fTerrainMaxHeight;
		vtxHeightMapSz = b.vtxHeightMapSz;
		segmentSize = b.segmentSize;
		return *this;
	}
};



// Summary:
//	Used in IRenderer::Draw()
struct S_API SDrawCallDesc
{
	IVertexBuffer* pVertexBuffer;
	IIndexBuffer* pIndexBuffer;
	usint32 iStartIBIndex;
	usint32 iEndIBIndex;
	usint32 iStartVBIndex;
	usint32 iEndVBIndex;

	EPrimitiveType primitiveType;

	SDrawCallDesc()
		: pVertexBuffer(0),
		pIndexBuffer(0),
		primitiveType(PRIMITIVE_TYPE_TRIANGLELIST)
	{
	}

	SDrawCallDesc(const SDrawCallDesc& o)
		: pVertexBuffer(o.pVertexBuffer),
		pIndexBuffer(o.pIndexBuffer),
		iStartIBIndex(o.iStartIBIndex),
		iEndIBIndex(o.iEndIBIndex),
		iStartVBIndex(o.iStartVBIndex),
		iEndVBIndex(o.iEndVBIndex),
		//transform(o.transform),
		primitiveType(o.primitiveType)
	{
	}
};

enum ERenderTarget
{
	eRT_BACKBUFFER,
	eRT_GBUFFER
};


enum EShaderType
{
	eSHADER_ZPASS,
	eSHADER_ZPASS_VEGETATION,	// zpass effect with vegetation VS shader	
	
	eSHADER_OPAQUE,		// opaque shading pass
	eSHADER_TERRAIN,	// terrain shading pass	

	eSHADER_HELPER,		// forward helper shader
	eSHADER_FONT
};

enum EZPassEffect
{
	eZPASS_DEFAULT,
	eZPASS_VEGETATION // vegetation bending in VS
};

struct S_API IRenderAPI
{
	virtual ~IRenderAPI() {}

	ILINE virtual SResult SetRenderTarget(ERenderTarget rt) = 0;

	ILINE virtual SResult SetTexture(int slot, ITexture* pTexture) = 0;

	ILINE virtual SResult SetVertexBuffer(int slot, IVertexBuffer* pVB) = 0;
	ILINE virtual SResult SetIndexBuffer(int slot, IIndexBuffer* pIB) = 0;

	ILINE virtual SResult SetActiveZPassShader(EZPassEffect zPassEffect) = 0;
	ILINE virtual SResult SetActiveShader(EShaderType shaderType) = 0;

	ILINE virtual SResult UpdateMaterialConstantsBuffer(SMaterialConstantsBuffer* pCB) = 0;
	ILINE virtual SResult UpdateObjectConstantsBuffer(SObjectConstantsBuffer* pCB) = 0;	
	
	ILINE virtual SResult Draw(const SDrawCallDesc& dcd) = 0;
};


SP_NMSPACE_END
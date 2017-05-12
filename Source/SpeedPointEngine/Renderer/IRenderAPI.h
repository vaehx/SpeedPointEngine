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

#include "IShader.h"
#include <Common\SPrerequisites.h>

SP_NMSPACE_BEG

struct S_API IVertexBuffer;
struct S_API IIndexBuffer;
struct S_API ITexture;


// Notice:
// - Constant Buffers must have a size which is a multiple of 16. That means you should
//   only use SMatrix4 or SVector4 (=float4) data types in here. Otherwise, make shure
//   to add proper padding bytes.

struct S_API SSceneConstants
{
	Mat44 mtxView;	// 16 * 4 Byte
	Mat44 mtxProj;
	Mat44 mtxProjInv;
	
	// Pos used instead of Dir, to avoid struggling around with angles when calculating
	// sun traveling due to TOD.   sun dir = normalize(-sunPosition)
	float4 sunPosition;
	Mat44 mtxSunViewProj;	// 16 * 4 Byte

	unsigned int shadowMapRes[2]; // 8 Byte
	unsigned int screenRes[2]; // 8 Byte

	float4 eyePosition;		// 4 * 4 Byte

	float fogStart;
	float fogEnd;

	char __padding[8];

	SSceneConstants& operator = (const SSceneConstants& b)
	{
		mtxView = b.mtxView;
		mtxProj = b.mtxProj;
		mtxProjInv = b.mtxProjInv;
		sunPosition = b.sunPosition;
		eyePosition = b.eyePosition;
		return *this;
	}
};

struct S_API STerrainConstants
{
	float fTerrainDMFadeRadius;		// 4 Byte
	float fTerrainMaxHeight;		// 4 Byte
	unsigned int vtxHeightMapSz;	// 4 Byte
	float segmentSize;				// 4 Byte
	float detailmapSz[2];			// 8 Byte
	char __padding[8];

	STerrainConstants& operator = (const STerrainConstants& b)
	{
		fTerrainDMFadeRadius = b.fTerrainDMFadeRadius;
		fTerrainMaxHeight = b.fTerrainMaxHeight;
		vtxHeightMapSz = b.vtxHeightMapSz;
		segmentSize = b.segmentSize;
		return *this;
	}
};



enum S_API EPrimitiveType
{
	PRIMITIVE_TYPE_TRIANGLELIST,
	PRIMITIVE_TYPE_LINES,
	PRIMITIVE_TYPE_LINESTRIP,
	PRIMITIVE_TYPE_UNKNOWN
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

enum S_API EFrontFace
{
	eFF_CCW,	// counter-clock-wise
	eFF_CW		// clock-wise
};

enum S_API EAntiAliasingQuality
{
	eAAQUALITY_LOW,		// no antialiasing
	eAAQUALITY_MEDIUM,	// 2 sample MSAA
	eAAQUALITY_HIGH		// special AA techniques (Gauss, Quincunx, FXAA, Adaptive AA, ...)
};

// SpeedPoint MultiSample count
enum S_API EAntiAliasingCount
{
	eAACOUNT_NONE = 0,
	eAACOUNT_2 = 2,
	eAACOUNT_4 = 4,
	eAACOUNT_8 = 8,
	eAACOUNT_16 = 16
};

enum S_API ETextureSamplingType
{
	eTEXTURE_SAMPLING_POINT,
	eTEXTURE_SAMPLING_BILINEAR,
	eTEXTURE_SAMPLING_SHADOWING, // comparative sampling for shadowmapping
};

/*
enum EShaderType
{
	eSHADER_ZPASS,
	eSHADER_ZPASS_VEGETATION,	// zpass effect with vegetation VS shader	
	
	eSHADER_OPAQUE,		// opaque shading pass
	eSHADER_TERRAIN,	// terrain shading pass	

	eSHADER_HELPER,		// forward helper shader
	eSHADER_FONT
};*/

/*


// Initialization:






// Render terrain. Notice that there is no shaderpass for the terrain here.
// We setup all the resources directly in the renderer.
pRenderer->RenderTerrain(terrain.renderDesc);

// Draw objects for GBuffer pass
pRenderer->SetShaderPass(eSHADERPASS_GBUFFER);
foreach (renderObject : renderObjects)
{
	pRenderer->Render(renderObject.renderDesc);
}

// Render objects again for each shadow casting light and draw shadow map
pRenderer->SetShaderPass(eSHADERPASS_SHADOW);
foreach (light : shadowCastingLights)
{
	foreach (renderObject : light.visibleObjects)
	{
		pRenderer->Render(renderObject.renderDesc);
	}
}

// Now do the shading pass
pRenderer->SetShaderPass(eSHADERPASS_SHADING);
foreach (light : lights)
{
	pRenderer->Render(light.renderDesc);
}





pGBufferPass->Bind();

*/

struct S_API IRenderAPI
{
	virtual ~IRenderAPI() {}

	ILINE virtual SResult SetRenderTarget(ERenderTarget rt) = 0;

	ILINE virtual SResult SetTexture(unsigned int slot, ITexture* pTexture) = 0;

	ILINE virtual SResult SetVertexBuffer(unsigned int slot, IVertexBuffer* pVB) = 0;
	ILINE virtual SResult SetIndexBuffer(unsigned int slot, IIndexBuffer* pIB) = 0;	

	//ILINE virtual SResult UpdateMaterialConstantsBuffer(SMaterialConstants* pCB) = 0;
	//ILINE virtual SResult UpdateObjectConstantsBuffer(SObjectConstants* pCB) = 0;	
	
	ILINE virtual SResult Draw(const SDrawCallDesc& dcd) = 0;
};


SP_NMSPACE_END
// --------------------------------------------------------------------------------------------------------------------
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
// --------------------------------------------------------------------------------------------------------------------

#pragma once

#include <Common\SPrerequisites.h>
#include <Common\Mat44.h>

SP_NMSPACE_BEG

struct S_API IRenderer;
struct S_API SShaderResources;
struct S_API ITexture;

// --------------------------------------------------------------------------------------------------------------------

// The type of Vertex structure to use as input for a shader/effect
enum S_API EShaderInputLayout
{
	eSHADERINPUTLAYOUT_DEFAULT = 0,	// SVertex
	eSHADERINPUTLAYOUT_SIMPLE,		// SSimpleVertex
	eSHADERINPUTLAYOUT_PARTICLES	// uses instance pool for particles
};

struct S_API SShaderInfo
{
	EShaderInputLayout inputLayout;
	string filename;
	string entry;

	SShaderInfo(const string& _filename, const string& _entry, EShaderInputLayout _inputLayout)
		: filename(_filename),
		entry(_entry),
		inputLayout(_inputLayout)
	{
	}

	SShaderInfo(const string& _filename, const string& _entry)
		: SShaderInfo(_filename, _entry, eSHADERINPUTLAYOUT_DEFAULT)
	{
	}

	SShaderInfo()
		: SShaderInfo("", "")
	{
	}	
};


struct S_API IShader
{
	virtual ~IShader()
	{
	}

	// Loads the shader from the file and compiles it
	// It will also select the first available technique as the default
	virtual SResult Load(IRenderer* pRenderer, const SShaderInfo& info) = 0;	

	virtual void Clear() = 0;

	// Summary:
	//	 Enable this shader
	// Description:
	//	Will set the VS and PS of this effect as currently active ones
	virtual SResult Bind() = 0;
};


// --------------------------------------------------------------------------------------------------------------------

struct S_API SObjectConstants
{
	Mat44 mtxWorld;	// 16 * 4 Byte
};

// --------------------------------------------------------------------------------------------------------------------

struct S_API STerrainConstants
{
	float3 sunIntensity;
	float fTerrainDMFadeRadius;

	float fTerrainMaxHeight;
	unsigned int vtxHeightMapSz;
	float segmentSize;
	unsigned int numLayers;
	
	float detailmapSz[2];
	char __padding[8];
};

struct S_API STerrainShaderResources
{
	ITexture* pHeightmap;
	ITexture* pLayerMask;
	ITexture* pColormap;
	ITexture* pTexturemap;
	ITexture* pNormalmap;
	ITexture* pRoughnessmap;
	STerrainConstants constants;

	STerrainShaderResources()
		: pHeightmap(0),
		pLayerMask(0),
		pColormap(0),
		pTexturemap(0),
		pNormalmap(0),
		pRoughnessmap(0)
	{
	}
};

// --------------------------------------------------------------------------------------------------------------------

enum S_API EShaderPassType
{
	eSHADERPASS_FORWARD = 0,
	eSHADERPASS_GBUFFER,
	eSHADERPASS_LIGHTPREPASS,
	eSHADERPASS_SHADING, // Deferred shading pass
	eSHADERPASS_SHADOWMAP,
	eSHADERPASS_PARTICLES,
	eSHADERPASS_GUI,
	eSHADERPASS_POSTEFFECT,
#define NUM_SHADERPASS_TYPES (eSHADERPASS_POSTEFFECT + 1)
	eSHADERPASS_NONE
};

inline const char* GetShaderPassTypeName(EShaderPassType type)
{
	switch (type)
	{
	case eSHADERPASS_FORWARD: return "SHADERPASS_FORWARD";
	case eSHADERPASS_GBUFFER: return "SHADERPASS_GBUFFER";
	case eSHADERPASS_LIGHTPREPASS: return "SHADERPASS_LIGHTPREPASS";
	case eSHADERPASS_SHADING: return "SHADERPASS_SHADING";
	case eSHADERPASS_SHADOWMAP: return "SHADERPASS_SHADOWMAP";
	case eSHADERPASS_PARTICLES: return "SHADERPASS_PARTICLES";
	case eSHADERPASS_GUI: return "SHADERPASS_GUI";
	case eSHADERPASS_POSTEFFECT: return "SHADERPASS_POSTEFFECT";
	default:
		return "Unknown";
	}
}

struct S_API IShaderPass
{
	virtual ~IShaderPass()
	{		
	}

	virtual SResult Initialize(IRenderer* pRenderer) = 0;
	virtual void ReloadShaders() = 0;
	virtual void Clear() = 0;
	virtual SResult Bind() = 0;
	virtual void OnUnbind() {};
	virtual void OnEndFrame() {};
	virtual void SetShaderResources(const SShaderResources& pShaderResources, const Mat44& transform) = 0;
	virtual void BindTerrainResources(const STerrainShaderResources& terrainShaderResources, bool constantsUpdated) {};
};

SP_NMSPACE_END

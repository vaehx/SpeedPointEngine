// ********************************************************************************************

//	SpeedPoint HLSL Shader Effect

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include <string>

using std::string;

SP_NMSPACE_BEG

struct S_API IGameEngine;

// The type of Vertex structure to use as input for a shader/effect
enum S_API EShaderVertexType
{
	eSHADERVERTEX_DEFAULT = 0,	// SVertex
	eSHADERVERTEX_SIMPLE	// SSimpleVertex
};

struct S_API SShaderInfo
{
	EShaderVertexType vertexType;
	string filename;
	string entry;

	SShaderInfo()
		: vertexType(eSHADERVERTEX_DEFAULT)
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




enum S_API EShaderPassType
{
	eSHADERPASS_GBUFFER = 0,
	eSHADERPASS_SHADING,
	eSHADERPASS_SHADOWMAP,
	eSHADERPASS_POSTEFFECT
#define NUM_SHADERPASS_TYPES (eSHADERPASS_POSTEFFECT + 1)
};


struct S_API IShaderPass
{
	virtual ~IShaderPass()
	{		
	}

	virtual SResult Initialize(IRenderer* pRenderer) = 0;
	virtual void Clear() = 0;

	virtual SResult Bind() = 0;	
};

SP_NMSPACE_END
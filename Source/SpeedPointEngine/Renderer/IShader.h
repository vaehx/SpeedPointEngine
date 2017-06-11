// ********************************************************************************************

//	SpeedPoint HLSL Shader Effect

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <Common\SPrerequisites.h>
#include <Common\Mat44.h>

SP_NMSPACE_BEG

struct S_API IRenderer;
struct S_API SShaderResources;

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




struct S_API SObjectConstants
{
	Mat44 mtxWorld;	// 16 * 4 Byte
};

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


struct S_API IShaderPass
{
	virtual ~IShaderPass()
	{		
	}

	virtual SResult Initialize(IRenderer* pRenderer) = 0;
	virtual void Clear() = 0;
	virtual SResult Bind() = 0;
	virtual void OnUnbind() {};
	virtual void OnEndFrame() {};
	virtual void SetShaderResources(const SShaderResources& pShaderResources, const Mat44& transform) = 0;
};

SP_NMSPACE_END
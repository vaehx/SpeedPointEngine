//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R.
//	(c) 2011-2015, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SPrerequisites.h>

SP_NMSPACE_BEG


enum EShader
{
	eGAPI_SHADER_PIXELSHADER,
	eGAPI_SHADER_VERTEXSHADER
};


struct S_API IGraphicsAPI;

struct S_API IGAPIConstantsBuffer
{
	virtual SResult Init(IGraphicsAPI* pAPI, unsigned int typeByteSize) = 0;
	virtual void Clear() = 0;

	virtual void SetData(void* pData) = 0;
	virtual void* GetData() = 0;	
};



enum EGAPIType
{
	eGAPI_D3D11
};

struct S_API IGraphicsAPI
{
	virtual void SetConstantBuffers(EShader shader, unsigned int startSlot, unsigned int numCBs, IGAPIConstantsBuffer* pCBs) = 0;

	virtual EGAPIType GetType() const = 0;
};


inline bool CheckGraphicsAPI(IGraphicsAPI* pApi, EGAPIType type)
{
	return IS_VALID_PTR(pApi) && pApi->GetType() == type;
}


SP_NMSPACE_END
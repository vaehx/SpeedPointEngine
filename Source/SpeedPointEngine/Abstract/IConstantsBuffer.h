#pragma once

#include <SPrerequisites.h>

SP_NMSPACE_BEG

struct S_API IRenderer;

struct S_API IConstantsBuffer
{
public:
	virtual ~IConstantsBuffer()
	{
	}

	virtual SResult Initialize(IRenderer* pRenderer, unsigned int sz) = 0;	
	virtual void Clear() = 0;

	// Uploads the updated constants
	virtual void Update() = 0;

	virtual void* GetData() const = 0;
};

SP_NMSPACE_END
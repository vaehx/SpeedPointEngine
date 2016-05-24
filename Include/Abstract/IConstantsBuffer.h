#pragma once

#include <SPrerequisites.h>

SP_NMSPACE_BEG

struct S_API IRenderer;

template<typename T>
struct S_API IConstantsBuffer
{
protected:
	T m_Constants;

public:
	virtual ~IConstantsBuffer()
	{
	}

	virtual SResult Initialize(IRenderer* pRenderer) = 0;	
	virtual void Clear() = 0;

	// Uploads the updated constants
	virtual void Update() = 0;

	virtual T* GetConstants() const
	{
		return &m_Constants;
	}
};

SP_NMSPACE_END
#pragma once

#include <Common\SPrerequisites.h>

SP_NMSPACE_BEG

// This has to be in a separate file from IResourcePool. Otherwise we will end up with
// cyclic header dependencies.

struct S_API IResource
{
protected:
	unsigned int m_RefCount;

public:
	IResource() : m_RefCount(0) {}
	virtual ~IResource()
	{
		m_RefCount = 0;
	}

	inline void AddRef()
	{
		m_RefCount++;
	}

	inline unsigned int GetRefCount() const
	{
		return m_RefCount;
	}

	inline void Release()
	{
		if (m_RefCount == 0)
			return;
		m_RefCount--;
		if (m_RefCount == 0)
			Clear();
	}

	virtual void Clear() = 0;
};

SP_NMSPACE_END

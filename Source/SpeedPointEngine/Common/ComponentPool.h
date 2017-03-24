#pragma once

#include "ChunkedObjectPool.h"

template<typename ICmp>
struct IComponentPool
{
	virtual ICmp* Get() = 0;

	// Returns:
	//	The first used component object in the pool or 0 if there is none in the pool
	// Arguments:
	//	id - is set to the index of the first used object
	virtual ICmp* GetFirst(unsigned int& id) const = 0;

	// Returns:
	//	The next component in the pool after the given index or 0 if there is no more
	// Arguments:
	//	id - is set to the index of the next used object AFTER the given index
	virtual ICmp* GetNext(unsigned int& id) const = 0;

	// Returns:
	//	The number of used objects in the pool
	virtual unsigned int GetNumObjects() const = 0;

	// Returns true if the object was released, false if parameter is invalid
	virtual bool Release(ICmp** pObject) = 0;
	virtual void ReleaseAll() = 0;
};

// CmpObjImpl must be an implementation of ICmp
template<typename ICmp, class CmpObjImpl>
class ComponentPool : public IComponentPool<ICmp>
{
private:
	ChunkedObjectPool<CmpObjImpl> m_Pool;

public:
	virtual ICmp* Get()
	{
		return dynamic_cast<ICmp*>(m_Pool.Get());
	}

	virtual ICmp* GetFirst(unsigned int& id) const
	{
		return dynamic_cast<ICmp*>(m_Pool.GetFirstUsedObject(id));
	}

	virtual ICmp* GetNext(unsigned int& id) const
	{
		return dynamic_cast<ICmp*>(m_Pool.GetNextUsedObject(id));
	}

	virtual unsigned int GetNumObjects() const
	{
		return m_Pool.GetUsedObjectCount();
	}

	virtual bool Release(ICmp** ppObject)
	{
		if (!ppObject)
			return false;

		ICmp* pObject = *ppObject;
		if (!pObject)
			return false;

		CmpObjImpl* pImplObject = dynamic_cast<CmpObjImpl*>(pObject);
		m_Pool.Release(&pImplObject);

		*ppObject = 0;
		return true;
	}

	virtual void ReleaseAll()
	{
		m_Pool.ReleaseAll();
	}

	ChunkedObjectPool<CmpObjImpl>* GetPool() const
	{
		return &m_Pool;
	}
};

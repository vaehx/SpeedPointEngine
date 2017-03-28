//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Common\SPrerequisites.h>

SP_NMSPACE_BEG

struct S_API ITypelessInstanceBuffer;

//////////////////////////////////////////////////////////////////////////////////////////////////

struct S_API IInstanceBufferResource
{
	virtual ~IInstanceBufferResource() {}
	virtual void Clear() = 0;
};

//////////////////////////////////////////////////////////////////////////////////////////////////

// This buffer makes sure the instance data is stored contiguosly.
// Also, the buffer is only uploaded to the GPU if changed were actually made.
template<typename InstanceT>
struct S_API IInstanceBuffer : public IInstanceBufferResource
{
	virtual ~IInstanceBuffer() {}

	virtual ITypelessInstanceBuffer* GetTypelessInstanceBuffer() = 0;

	virtual bool IsLocked() const = 0;

	// Blocks until buffer can be locked
	virtual const InstanceT* Lock(unsigned int* pnInstances) = 0;

	// Automatically uploads the changed range - if any.
	virtual void Unlock() = 0;

	// Makes sure buffer is big enough to hold a new instance.
	// Returns pointer to new instance memory location.
	// Returns 0 if not locked.
	virtual InstanceT* AddInstance() = 0;

	// Read-only access to given instance
	// Returns 0 if not locked.
	virtual const InstanceT* GetInstance(unsigned int i) const = 0;

	// Write-acess to given instance. Buffer has to be uploaded on unlock!
	// Returns 0 if not locked.
	virtual InstanceT* GetInstanceWriteable(unsigned int i) = 0;

	// Removes the instance and moves all instances above this instance down
	// to make the memory contigous
	virtual void RemoveInstance(unsigned int i) = 0;

	// Returns UINT_MAX if not locked
	virtual unsigned int GetNumInstances() const = 0;
};

//////////////////////////////////////////////////////////////////////////////////////////////////

struct S_API ITypelessInstanceBuffer : public IInstanceBuffer<void>
{
	// strideSz - The size of one instance in bytes
	// chunkSize - How many instances to pre-allocate on resize. Also the initial capacity. Must not be 0
	virtual void Init(unsigned int strideSz, unsigned int chunkSize = 10) = 0;

	virtual unsigned int GetStrideSize() const = 0;
};

//////////////////////////////////////////////////////////////////////////////////////////////////

template<typename InstanceT, unsigned int chunkSz = 10>
struct S_API InstanceBufferWrapper : public IInstanceBuffer<InstanceT>
{
private:
	ITypelessInstanceBuffer* m_pBuffer;

public:
	InstanceBufferWrapper(ITypelessInstanceBuffer* pTypelessInstanceBuffer)
		: m_pBuffer(pTypelessInstanceBuffer)
	{
		if (m_pBuffer)
			m_pBuffer->Init(sizeof(InstanceT), chunkSz);
	}

	~InstanceBufferWrapper()
	{
		delete m_pBuffer;
		m_pBuffer = 0;
	}

	virtual ITypelessInstanceBuffer* GetTypelessInstanceBuffer()
	{
		return m_pBuffer;
	}

	virtual bool IsLocked() const
	{
		if (!m_pBuffer)
			return false;

		return m_pBuffer->IsLocked();
	}

	virtual const InstanceT* Lock(unsigned int* pnInstances = 0)
	{
		if (!m_pBuffer)
			return 0;

		unsigned int nInstances;
		const InstanceT* pInstances = reinterpret_cast<const InstanceT*>(m_pBuffer->Lock(&nInstances));

		if (pnInstances)
			*pnInstances = nInstances;

		return pInstances;
	}

	virtual void Unlock()
	{
		if (!m_pBuffer)
			return;

		m_pBuffer->Unlock();
	}

	virtual InstanceT* AddInstance()
	{
		if (!m_pBuffer || !m_pBuffer->IsLocked())
			return 0;

		InstanceT* pInstance = reinterpret_cast<InstanceT*>(m_pBuffer->AddInstance());
		if (!pInstance)
			return 0;

		return pInstance;
	}

	virtual void RemoveInstance(unsigned int i)
	{
		if (!m_pBuffer)
			return;

		m_pBuffer->RemoveInstance(i);
	}

	virtual const InstanceT* GetInstance(unsigned int i) const
	{
		if (!m_pBuffer)
			return 0;

		return reinterpret_cast<const InstanceT*>(m_pBuffer->GetInstance(i));
	}

	virtual InstanceT* GetInstanceWriteable(unsigned int i)
	{
		if (!m_pBuffer)
			return 0;

		return reinterpret_cast<InstanceT*>(m_pBuffer->GetInstanceWriteable(i));
	}

	virtual unsigned int GetNumInstances() const
	{
		if (!m_pBuffer)
			return 0;

		return m_pBuffer->GetNumInstances();
	}

	virtual void Clear()
	{
		if (!m_pBuffer)
			return;

		m_pBuffer->Clear();
	}
};

SP_NMSPACE_END

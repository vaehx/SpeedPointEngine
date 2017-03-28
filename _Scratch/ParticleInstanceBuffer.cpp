// ------------------------------------------------------------------------------------------------------
template<typename InstanceT>
struct IInstanceBuffer
{
protected:
    vector<InstanceT> m_Instances;

    const void* GetShadowBuffer() const
    {
        if (!m_Instance.empty())
            return (const void*)&m_Instances[0];
        else
            return 0;
    }

public:
	const InstanceT* GetInstance(unsigned int i) const
	{
		if (i < m_Instances.size())
            return &m_Instances[i];
        else
            return 0;
	}

	void Lock(unsigned int* pnInstances) = 0;
    virtual void Unlock() = 0;

    InstanceT* AddInstance(const InstanceT& data = InstanceT())
    {


        void* instance = AddInstancenIntrnl((void*)data);
        return reinterpret_cast<const InstanceT*>(instance);
    }

	virtual void RemoveInstance(unsigned int i) = 0;        // <<<------- THIS WILL PROBABLY NOT WORK, AS IT IS STILL A TEMPLATE METHOD.......
};

// ------------------------------------------------------------------------------------------------------
template<typename InstanceT>
class DX11InstanceBuffer : public IInstanceBuffer<InstanceT>
{
private:
	InstanceT* m_pInstances;
	unsigned int m_nMaxInstances; // Buffer capacity
	unsigned int m_nInstances;
	unsigned int m_n
protected:
	virtual const void* GetInstanceIntrnl(unsigned int i) const;
	virtual void Lock(void* pInstances, unsigned int* pnInstances);
public:
	virtual void RemoveInstance(unsigned int i);
};

template<typename InstanceT>
DX11InstanceBuffer<InstanceT>::RemoveInstance(unsigned int i)
{
	if (!IsLocked())
		return S_ERROR;

	if (i < m_nInstances - 1)
	{
		// Move everything above the instance down
		memcpy(m_pInstances + i, m_pInstances + (i + 1), sizeof(InstanceT) * (m_nInstances - (i + 1)));
	}

    --m_nInstances;
}






ParticleEmitter::Update(fTime)
{
	m_CurTime = QueryHighPerformanceCounter();

	ParticleInstance* pInstances = 0;
	unsigned int nInstances;

	m_pInstances->Lock(&pInstances, &nInstances);

	bool uploadNecessary = false;
	for (unsigned int i = 0; i < nInstances;)
	{
		if (m_CurTime - pInstances[i].startTime >= m_MaxParticleLifetime)
		{
			// Particle expired
			uploadNecessary = true;
			m_pInstanceBuffer->RemoveInstance(i);
			--nInstances;
		}
		else
		{
			++i;
		}
	}

	m_pInstances->Unlock(!uploadNecessary); // true to skip upload





	unsigned int iParticle = 0;
	Particle* pParticle = m_Particles.GetFirstUsedObject(iParticle);
	while (pParticle)
	{
		if (m_CurTime - pParticle->startTime >= m_MaxParticleLifetime)
		{
			pInstances[iParticle]
		}

		pParticle = m_Particles.GetNextUsedObject(iParticle);
	}
}
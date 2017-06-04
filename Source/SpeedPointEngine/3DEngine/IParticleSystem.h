//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IRenderObject.h"
#include <Renderer\IRenderer.h>
#include <Renderer\IInstanceBuffer.h>
#include <Common\ComponentPool.h>
#include <Common\SPrerequisites.h>

SP_NMSPACE_BEG

struct S_API ITexture;

// TODO: Currently only spreading in all directions
struct S_API SParticleEmitterParams
{
	Vec3f position;
	ITexture* particleTexture;
	float particleSize; // size of a side of a particle billboard
	float particleMaxDistance; // maximum distance of a particle before despawning
	float particleSpeed; // units/second
	unsigned int numConcurrentParticles; // must be at least 1
	bool spawnAutomatically; // if false, call SpawnParticle()

	SParticleEmitterParams()
		: spawnAutomatically(true),
		particleTexture(0),
		particleSize(1.0f),
		particleMaxDistance(0),
		particleSpeed(0),
		numConcurrentParticles(1)
	{
	}
};

struct S_API SParticleInstance
{
	Vec3f direction; // rotated by emitter rotation around emitter position, 12B
	unsigned __int32 startTime; // us time after emitter creation when particle was created, 4B
}; // 16B = 128bit

class S_API CParticleEmitter
{
protected:
	bool m_bTrash;
	bool m_bStarted;
	LARGE_INTEGER m_PerformanceFreq;
	LARGE_INTEGER m_StartTimestamp;
	unsigned __int32 m_CurTime;
	unsigned __int32 m_ParticleLifetime;
	unsigned __int32 m_SpawnDelay; // delay between particle spawns
	unsigned int m_nSpawnedParticles;
	unsigned int m_nForceSpawnParticles;
	SInstancedRenderDesc m_RenderDesc;
	IInstanceBuffer<SParticleInstance>* m_pInstanceBuffer;
	SParticleEmitterParams m_Params;

public:
	CParticleEmitter();
	virtual ~CParticleEmitter() { Clear(); }

	void Init(const SParticleEmitterParams& params);
	void Update(float fTime);

	void SpawnParticle();

	const SParticleEmitterParams& GetParams() const { return m_Params; }
	void SetParams(const SParticleEmitterParams& params);

	const Vec3f& GetPos() const { return m_Params.position; }
	void SetPos(const Vec3f& pos) { m_Params.position = pos; }

	const unsigned __int32& GetCurTime() const { return m_CurTime; }
	const unsigned __int32& GetParticleLifetime() const { return m_ParticleLifetime; }

	bool IsTrash() const { return m_bTrash; }

	// Flags this emitter as trash
	virtual void Clear();

	virtual SInstancedRenderDesc* GetRenderDesc();
};

struct S_API IParticleSystem
{
protected:
	virtual void SetEmitterPool(IComponentPool<CParticleEmitter>* pool) = 0;

public:
	virtual ~IParticleSystem() {}

	virtual SResult Init(IRenderer* pRenderer) = 0;

	template<class ParticleEmitterImpl>
	void SetEmitterImplementation() { SetEmitterPool(new ComponentPool<CParticleEmitter, ParticleEmitterImpl>()); }

	virtual CParticleEmitter* CreateEmitter(const SParticleEmitterParams& params = SParticleEmitterParams()) = 0;
	virtual void ClearEmitters() = 0;

	virtual void Clear() = 0;
};

SP_NMSPACE_END

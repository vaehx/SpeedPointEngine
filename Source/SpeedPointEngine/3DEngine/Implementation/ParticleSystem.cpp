//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "ParticleSystem.h"
#include "C3DEngine.h"
#include <Renderer\IResourcePool.h>
#include <Renderer\DirectX11\DX11Shader.h> // <-- TODO, remove this when shader passes are moved out of render api

SP_NMSPACE_BEG

//////////////////////////////////////////////////////////////////////////////////////////////////

S_API CParticleEmitter::CParticleEmitter()
	: m_pInstanceBuffer(0),
	m_bStarted(false)
{
	srand(0);
}

S_API void CParticleEmitter::Init(const SParticleEmitterParams& params)
{
	m_Params = params;

	IResourcePool* pResources = C3DEngine::Get()->GetRenderer()->GetResourcePool();

	if (Failure(pResources->AddInstanceBuffer<SParticleInstance, 500>(&m_pInstanceBuffer)))
	{
		CLog::Log(S_ERROR, "Failed init particle emitter: Failed AddInstanceBuffer()");
		return;
	}

	m_RenderDesc.pInstanceBuffer = m_pInstanceBuffer->GetTypelessInstanceBuffer();
	m_RenderDesc.pVertexBuffer = 0; // will be set by ParticleSystem when rendering
	m_RenderDesc.render = true;
	m_RenderDesc.enableDepthWrite = false;
	m_RenderDesc.shaderResources.textureMap = params.particleTexture;
	SMatrixIdentity(m_RenderDesc.transform);

	m_bStarted = false;
	m_nSpawnedParticles = 0;

	m_ParticleLifetime = (usint32)(1000000.0 * (double)m_Params.particleMaxDistance / (double)m_Params.particleSpeed); //us
	m_SpawnDelay = (usint32)(m_ParticleLifetime / (usint32)m_Params.numConcurrentParticles);
}

S_API void CParticleEmitter::Update(float fTime)
{
	// Update time since emitter init
	if (!m_bStarted)
	{
		// Take start timestamp
		QueryPerformanceFrequency(&m_PerformanceFreq);
		QueryPerformanceCounter(&m_StartTimestamp);
		m_CurTime = 0;
		m_bStarted = true;
	}

	static LARGE_INTEGER cur;
	QueryPerformanceCounter(&cur);

	usint32 newCurTime = (cur.QuadPart - m_StartTimestamp.QuadPart) * 1000000ll / m_PerformanceFreq.QuadPart;
	usint32 lastFrameDur = newCurTime - m_CurTime;
	m_CurTime = newCurTime;

	unsigned int nInstances;
	const SParticleInstance* pInstances = m_pInstanceBuffer->Lock(&nInstances);

	// Remove expired particles
	// Start from end, so in the best case we need to copy/move less memory
	if (nInstances > 0)
	{
		for (unsigned int i = nInstances - 1; i >= 0; --i)
		{
			const SParticleInstance* pInstance = &pInstances[i];
			if (m_CurTime - pInstance->startTime >= m_ParticleLifetime)
				m_pInstanceBuffer->RemoveInstance(i);

			if (i == 0)
				break;
		}
	}

	// Spawn new particle if necessary
	unsigned int numExpectedSpawns = (unsigned int)(m_CurTime / m_SpawnDelay);
	unsigned int numActualSpawns = m_nSpawnedParticles;
	if (numExpectedSpawns > numActualSpawns)
	{
		for (unsigned int i = 0; i < (numExpectedSpawns - numActualSpawns); ++i)
		{
			SParticleInstance* pInstance = m_pInstanceBuffer->AddInstance();

			// TODO: use more sophisticated parameters to control direction of particles
			pInstance->direction = Vec3f(
				(float)rand() / (RAND_MAX / 2.f) - 1.f,
				(float)rand() / (RAND_MAX / 2.f) - 1.f,
				(float)rand() / (RAND_MAX / 2.f) - 1.f
			).Normalized();

			pInstance->startTime = (m_CurTime - lastFrameDur) + i * m_SpawnDelay;

			++m_nSpawnedParticles;
		}
	}

	m_pInstanceBuffer->Unlock();
}

S_API void CParticleEmitter::Clear()
{
	IResourcePool* pResources = C3DEngine::Get()->GetRenderer()->GetResourcePool();

	if (m_pInstanceBuffer)
		pResources->RemoveInstanceBuffer(&m_pInstanceBuffer);

	m_bStarted = false;
}

S_API SInstancedRenderDesc* CParticleEmitter::GetRenderDesc()
{
	return &m_RenderDesc;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

S_API CParticleSystem::CParticleSystem()
	: m_pEmitters(0),
	m_pPlaneVB(0),
	m_pRenderer(0)
{
}

S_API SResult CParticleSystem::Init(IRenderer* pRenderer)
{
	m_pRenderer = pRenderer;

	// Create plane vertex buffer
	if (Failure(m_pRenderer->GetResourcePool()->AddVertexBuffer(&m_pPlaneVB)))
		return CLog::Log(S_ERROR, "Failed init particle system: Failed AddVertexBuffer()");

	SVertex planeVertices[] =
	{
		SVertex(-0.5f, -0.5f, 0, 0, 0, 1.0f, 1.0f, 0, 0, 0, 0),
		SVertex(0.5f, 0.5f, 0, 0, 0, 1.0f, 1.0f, 0, 0, 1.0f, 1.0f),
		SVertex(0.5f, -0.5f, 0, 0, 0, 1.0f, 1.0f, 0, 0, 1.0f, 0.0f),
		SVertex(-0.5f, -0.5f, 0, 0, 0, 1.0f, 1.0f, 0, 0, 0.0f, 0.0f),
		SVertex(-0.5f, 0.5f, 0, 0, 0, 1.0f, 1.0f, 0, 0, 0.0f, 1.0f),
		SVertex(0.5f, 0.5f, 0, 0, 0, 1.0f, 1.0f, 0, 0, 1.0f, 1.0f)
	};

	if (Failure(m_pPlaneVB->Initialize(m_pRenderer, eVBUSAGE_STATIC, planeVertices, 6)))
		return CLog::Log(S_ERROR, "Failed init particle system: Failed initialize plane vertex buffer");

	return S_SUCCESS;
}

S_API void CParticleSystem::Update(float fTime)
{
	if (!m_pEmitters)
		return;

	unsigned int iEmitter = 0;
	CParticleEmitter* pEmitter = m_pEmitters->GetFirst(iEmitter);
	while (pEmitter)
	{
		pEmitter->Update(fTime);
		pEmitter = m_pEmitters->GetNext(iEmitter);
	}
}

S_API void CParticleSystem::Render()
{
	if (!m_pEmitters)
		return;

	IRenderer* pRenderer = C3DEngine::Get()->GetRenderer();

	ParticleShaderPass* pParticleShaderPass = dynamic_cast<ParticleShaderPass*>(pRenderer->GetCurrentShaderPass());
	if (!pParticleShaderPass)
	{
		CLog::Log(S_ERROR, "Particle shader pass not bound");
		return;
	}

	unsigned int iEmitter = 0;
	CParticleEmitter* pEmitter = m_pEmitters->GetFirst(iEmitter);
	while (pEmitter)
	{
		// Update constants
		const SParticleEmitterParams& params = pEmitter->GetParams();

		SParticleEmitterConstants constants;
		constants.time = pEmitter->GetCurTime();
		constants.particleSpeed = params.particleSpeed;
		constants.particleSize = params.particleSize;
		constants.particleLifetime = pEmitter->GetParticleLifetime();

		pParticleShaderPass->SetConstants(constants);

		// Set vertex buffer
		SInstancedRenderDesc* pRenderDesc = pEmitter->GetRenderDesc();
		pRenderDesc->pVertexBuffer = m_pPlaneVB;

		// Render
		pRenderDesc->pInstanceBuffer->Lock(0);
		{
			pRenderer->RenderInstanced(*pRenderDesc);
			pRenderDesc->pInstanceBuffer->Unlock();
		}

		pEmitter = m_pEmitters->GetNext(iEmitter);
	}
}

S_API void CParticleSystem::SetEmitterPool(IComponentPool<CParticleEmitter>* pool)
{
	m_pEmitters = pool;
}

S_API CParticleEmitter* CParticleSystem::CreateEmitter(const SParticleEmitterParams& params /*= SParticleEmitterParams()*/)
{
	if (!m_pEmitters)
	{
		CLog::Log(S_ERROR, "Cannot create particle emitter: Emitter pool not initialized");
		return 0;
	}

	CParticleEmitter* pEmitter = m_pEmitters->Get();
	pEmitter->Init(params);
	return pEmitter;
}

S_API void CParticleSystem::ClearEmitters()
{
	if (m_pEmitters)
	{
		unsigned int i;
		CParticleEmitter* emitter = m_pEmitters->GetFirst(i);
		while (emitter)
		{
			emitter->Clear();
			emitter = m_pEmitters->GetNext(i);
		}

		m_pEmitters->ReleaseAll();
	}
}

S_API void CParticleSystem::Clear()
{
	ClearEmitters();
	delete m_pEmitters;
	m_pEmitters = 0;

	if (m_pPlaneVB)
	{
		if (m_pRenderer)
			m_pRenderer->GetResourcePool()->RemoveVertexBuffer(&m_pPlaneVB);

		m_pPlaneVB = 0;
	}
}


SP_NMSPACE_END

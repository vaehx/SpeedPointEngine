//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\IParticleSystem.h"

SP_NMSPACE_BEG

class S_API CParticleSystem : public IParticleSystem
{
private:
	IRenderer* m_pRenderer;
	IVertexBuffer* m_pPlaneVB;
	IComponentPool<CParticleEmitter>* m_pEmitters;

protected:
	virtual void SetEmitterPool(IComponentPool<CParticleEmitter>* pool);

public:
	CParticleSystem();

	void Update(float fTime);
	void Render();

public: // IParticleSystem
	virtual SResult Init(IRenderer* pRenderer);
	virtual CParticleEmitter* CreateEmitter(const SParticleEmitterParams& params = SParticleEmitterParams());
	virtual void ClearEmitters();
	virtual void Clear();
};

SP_NMSPACE_END

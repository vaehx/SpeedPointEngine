#include "..\RenderableComponent.h"
#include <GameFramework\IGameEngine.h>
#include <Renderer\ITexture.h>
#include <Renderer\IResourcePool.h>

SP_NMSPACE_BEG

S_API void CRenderMeshComponent::Serialize(ISerContainer* ser, bool serialize /*= true*/)
{
	if (serialize)
	{
		IGeometry* pGeometry = GetGeometry();
		ser->SetString("geometry", pGeometry ? pGeometry->GetFilePath() : "");
	}
	else
	{
		string geomFile = ser->GetString("geometry");
		if (!geomFile.empty())
		{
			IGeometry* pGeometry = SpeedPointEnv::Get3DEngine()->GetGeometryManager()->LoadGeometry(geomFile);
			if (pGeometry)
				SetGeometry(pGeometry);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////

S_API SInstancedRenderDesc* CParticleEmitterComponent::GetRenderDesc()
{
	SInstancedRenderDesc* renderDesc = CParticleEmitter::GetRenderDesc();
	if (m_pEntity)
		renderDesc->transform = m_pEntity->GetTransform() * renderDesc->transform;

	return renderDesc;
}

S_API void CParticleEmitterComponent::Clear()
{
	CParticleEmitter::Clear();
	IComponent::Release();
}

S_API void CParticleEmitterComponent::Serialize(ISerContainer* ser, bool serialize)
{
	if (serialize)
	{
		SParticleEmitterParams params = GetParams();
		ser->SetUInt("numConcurrentParticles", params.numConcurrentParticles);
		ser->SetFloat("particleMaxDistance", params.particleMaxDistance);
		ser->SetFloat("particleSize", params.particleSize);
		ser->SetFloat("particleSpeed", params.particleSpeed);

		if (params.particleTexture)
			ser->SetString("particleTexture", params.particleTexture->GetSpecification());
	}
	else
	{
		SParticleEmitterParams params = GetParams();
		params.numConcurrentParticles = ser->GetUInt("numConcurrentParticles", params.numConcurrentParticles);
		params.particleMaxDistance = ser->GetFloat("particleMaxDistance", params.particleMaxDistance);
		params.particleSize = ser->GetFloat("particleSize", params.particleSize);
		params.particleSpeed = ser->GetFloat("particleSpeed", params.particleSpeed);

		string curtexture = "";
		if (params.particleTexture)
			curtexture = params.particleTexture->GetSpecification();

		string texture = ser->GetString("particleTexture", curtexture);
		if (!texture.empty() && texture != curtexture)
		{
			ITexture* pTexture = SpeedPointEnv::GetRenderer()->GetResourcePool()->GetTexture(texture);
			if (pTexture)
				params.particleTexture = pTexture;
		}

		SetParams(params);
	}
}

SP_NMSPACE_END

#include "..\RenderableComponent.h"
#include "SPMManager.h"
#include <GameFramework\IGameEngine.h>
#include <Renderer\ITexture.h>
#include <Renderer\IResourcePool.h>
#include <Common\strutils.h>

SP_NMSPACE_BEG


//////////////////////////////////////////////////////////////////////////////////////////////////
//	RenderMesh
//////////////////////////////////////////////////////////////////////////////////////////////////

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
			SPMManager* pSPMManager = SpeedPointEnv::GetEngine()->GetSPMManager();
			const CSPMLoader* pSPM = pSPMManager->Load(geomFile);
			if (pSPM)
			{
				SInitialGeometryDesc geomDesc;
				SPMManager::FillInitialGeometryDescFromSPM(pSPM, geomDesc);

				I3DEngine* p3DEngine = SpeedPointEnv::Get3DEngine();
				IGeometry* pGeometry = p3DEngine->GetGeometryManager()->CreateGeometry(geomDesc, geomFile);
				if (pGeometry)
					SetGeometry(pGeometry);

				SPMManager::ClearInitialGeometryDesc(geomDesc);
			}
		}
	}
}



//////////////////////////////////////////////////////////////////////////////////////////////////
//	RenderLight
//////////////////////////////////////////////////////////////////////////////////////////////////

S_API void CRenderLightComponent::OnRelease()
{
	CRenderLight::Clear();
}

S_API void CRenderLightComponent::OnRender()
{
	if (!m_pEntity)
		return;

	m_Params.position = m_pEntity->GetPos();
	
	const Vec3f& scale = m_pEntity->GetScale();
	m_Params.radius = max(max(scale.x, scale.y), scale.z);
}

const char* SerializeLightType(ELightType type)
{
	switch (type)
	{
	case eLIGHT_TYPE_OMNIDIRECTIONAL:
	default:
		return "omni";
	}
}

ELightType DeserializeLightType(const string& expr)
{
	/*if (striequals(expr, "spot"))
		return eLIGHT_TYPE_SPOTLIGHT;
	else*/
		return eLIGHT_TYPE_OMNIDIRECTIONAL;
}

S_API void CRenderLightComponent::Serialize(ISerContainer* ser, bool serialize /*= true*/)
{
	if (serialize)
	{
		ser->SetString("type", SerializeLightType(m_Params.type));
		ser->SetVec3f("position", m_Params.position);
		ser->SetFloat("radius", m_Params.radius);
		ser->SetVec3f("intensity", m_Params.intensity.ToFloat3());
	}
	else
	{
		string typeStr = ser->GetString("type", "");
		if (!typeStr.empty())
			m_Params.type = DeserializeLightType(typeStr);

		m_Params.position = ser->GetVec3f("position", m_Params.position);
		m_Params.radius = ser->GetFloat("radius", m_Params.radius);
		m_Params.intensity = ser->GetVec3f("intensity", m_Params.intensity.ToFloat3());
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//	ParticleEmitter
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

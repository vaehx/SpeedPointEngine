#include <Implementation\GameFacilities\RenderableComponent.h>
#include <Abstract\I3DEngine.h>
#include <Abstract\IRenderer.h>
#include <Abstract\IMaterial.h>
#include <Abstract\IVertexBuffer.h>
#include <Abstract\IIndexBuffer.h>
#include <Util\SerializationTools.h>

SP_NMSPACE_BEG

S_API CRenderableComponent::CRenderableComponent()
	: IComponent()
{
}

S_API CRenderableComponent::~CRenderableComponent()
{
	Clear();
}

// -----------------------------------------------------------------------------------------------------
S_API void CRenderableComponent::Serialize(map<string, string>& params) const
{
	params["geomFile"] = SerializeString(m_pGeometry ? m_pGeometry->GetGeomFile() : "");
}

// -----------------------------------------------------------------------------------------------------
S_API void CRenderableComponent::Deserialize(const map<string, string>& params)
{
	I3DEngine* p3DEngine = SpeedPointEnv::GetEngine()->Get3DEngine();
	if (!p3DEngine)
		return;

	IGeometryManager* pGeoMgr = p3DEngine->GetGeometryManager();
	if (!pGeoMgr)
		return;

	auto geomFileIt = params.find("geomFile");
	if (geomFileIt == params.end())
	{
		CLog::Log(S_ERROR, "Cannot deserialize renderable: geomFile invalid");
		return;
	}

	string geomFile = DeserializeString(geomFileIt->second);

	SInitialGeometryDesc* pGeomDesc = pGeoMgr->LoadGeometry(geomFile);
	if (!pGeomDesc)
	{
		CLog::Log(S_ERROR, "Cannot deserialize renderable: geomFile could not be loaded");
		return;
	}

	SRenderMeshParams meshParams;
	meshParams.pGeomDesc = pGeomDesc;
#ifdef _DEBUG
	meshParams._name = geomFile;
#endif
	Init(meshParams);
}

// -----------------------------------------------------------------------------------------------------
S_API void CRenderableComponent::OnRelease()
{
	Clear();
}

// -----------------------------------------------------------------------------------------------------
S_API void CRenderableComponent::OnRender()
{
	if (!m_pEntity)
		return;

	m_RenderDesc.transform = m_pEntity->GetTransform();
}

// -----------------------------------------------------------------------------------------------------
S_API void CRenderableComponent::OnEntityTransformed()
{
}

SP_NMSPACE_END

#include <Implementation\GameFacilities\RenderableComponent.h>
#include <Abstract\I3DEngine.h>
#include <Abstract\IRenderer.h>
#include <Abstract\IMaterial.h>
#include <Abstract\IVertexBuffer.h>
#include <Abstract\IIndexBuffer.h>

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
S_API void CRenderableComponent::OnRelease()
{
	Clear();
}

// -----------------------------------------------------------------------------------------------------
S_API void CRenderableComponent::OnEntityTransformed()
{
	if (!m_pEntity)
		return;

	//TODO: Do not update the transform here immediately to support multithreading!

	m_RenderDesc.transform = m_pEntity->GetTransform();
}

SP_NMSPACE_END

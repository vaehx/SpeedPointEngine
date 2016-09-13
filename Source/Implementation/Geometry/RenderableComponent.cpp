#include <Implementation\Geometry\RenderableComponent.h>
#include <Abstract\I3DEngine.h>
#include <Abstract\IRenderer.h>
#include <Abstract\IMaterial.h>
#include <Abstract\IVertexBuffer.h>
#include <Abstract\IIndexBuffer.h>

SP_NMSPACE_BEG

S_API CRenderableComponent::CRenderableComponent()
	: IComponent(),
	m_bTrash(false)
{
}

S_API CRenderableComponent::~CRenderableComponent()
{
	Clear();
}

S_API void CRenderableComponent::Release()
{
	Clear();
	m_bTrash = true;

	if (m_pEntity)
	{
		m_pEntity->ReleaseComponent(this);
		m_pEntity = 0;
	}
}

S_API bool CRenderableComponent::IsTrash() const
{
	return m_bTrash;
}

// -------------------------------------------------------------------------------------------

S_API void CRenderableComponent::OnEntityTransformed()
{
	if (!m_pEntity)
		return;

	//TODO: Do not update the transform here immediately to support multithreading!

	m_RenderDesc.transform = m_pEntity->GetTransform();
}

SP_NMSPACE_END

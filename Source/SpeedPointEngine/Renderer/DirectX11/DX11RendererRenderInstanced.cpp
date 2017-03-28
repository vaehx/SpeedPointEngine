#include "DX11Renderer.h"
#include "DX11VertexBuffer.h"
#include "DX11InstanceBuffer.h"

SP_NMSPACE_BEG

S_API SResult DX11Renderer::RenderInstanced(const SInstancedRenderDesc& renderDesc)
{
	if (!m_bInScene)
		return CLog::Log(S_ERROR, "Failed RenderInstanced(): Not in scene!");

	if (!renderDesc.render)
		return S_SUCCESS;

	// Set depth stencil state
	EnableDepthTest(renderDesc.enableDepthTest, renderDesc.enableDepthWrite);
	SetDepthTestFunction(eDEPTH_TEST_LESS);

	// Set rasterizer state
	D3D11_CULL_MODE origCullMode = m_rsDesc.CullMode;
	m_rsDesc.CullMode = D3D11_CULL_NONE;
	UpdateRasterizerState();

	// Set sampler state
	SetSamplerState(eTEX_SAMPLE_BILINEAR);

	// Set blend state
	D3D11_SetBlendState(m_pAlphaTestBlendState);

	// Update scene constants
	SetViewProjMatrix(m_pTargetViewport);
	BindSceneCB(m_SceneConstants.GetCB());

	// Set shader resources and object constants
	GetCurrentShaderPass()->SetShaderResources(renderDesc.shaderResources, renderDesc.transform);

	// Set vertex- and instance stream
	SetIBStream(0);
	SetVBStream(renderDesc.pVertexBuffer);
	SetInstanceStream(renderDesc.pInstanceBuffer);

	// Set primitive type
	if (m_SetPrimitiveType != PRIMITIVE_TYPE_TRIANGLELIST)
	{
		m_pD3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_SetPrimitiveType = PRIMITIVE_TYPE_TRIANGLELIST;
	}

	// Draw
	m_pD3DDeviceContext->DrawInstanced(
		renderDesc.pVertexBuffer->GetVertexCount(),
		renderDesc.pInstanceBuffer->GetNumInstances(),
		0, 0);


	// Reset states
	m_rsDesc.CullMode = origCullMode;
	UpdateRasterizerState();

	SetVBStream(0, 1);

	return S_SUCCESS;
}

SP_NMSPACE_END

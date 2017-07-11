// --------------------------------------------------------------------------------------------------------------------
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
// --------------------------------------------------------------------------------------------------------------------

#include "DX11Renderer.h"
#include "DX11VertexBuffer.h"
#include "DX11InstanceBuffer.h"

SP_NMSPACE_BEG

/*

	Render()
		RenderGeometry()
			DrawSubsets()
				pCurrentShaderPass->SetShaderResources()
				Draw()
					pD3DDeviceContext->Draw()

*/

// --------------------------------------------------------------------------------------------------------------------
S_API void DX11Renderer::RenderGeometry(const SRenderDesc& renderDesc, bool overrideBlendState)
{
	EnableDepthTest(renderDesc.bDepthStencilEnable);

	EDepthTestFunction depthTestFunc = (renderDesc.bInverseDepthTest ? eDEPTH_TEST_GREATER : eDEPTH_TEST_LESS);
	SetDepthTestFunction(depthTestFunc);

	// If renderDesc specifies a custom viewProj matrix, backup current scene constants and use it
	SSceneConstants origSceneConstants;
	if (renderDesc.bCustomViewProjMtx)
	{
		FrameDump("Setting custom viewproj mtx");

		// Backup current scene constants, so we can restore them later
		SSceneConstants* pSceneConstants = m_SceneConstants.GetConstants();
		memcpy(&origSceneConstants, pSceneConstants, sizeof(SSceneConstants));

		SetViewProjMatrix(renderDesc.viewMtx, renderDesc.projMtx);
	}


	// In case something else was bound to the slot before...
	BindSceneCB(m_SceneConstants.GetCB());


	DrawSubsets(renderDesc, overrideBlendState);


	// Restore scene constants if necessary
	if (renderDesc.bCustomViewProjMtx)
	{
		SSceneConstants* pSceneConstants = m_SceneConstants.GetConstants();
		memcpy(pSceneConstants, &origSceneConstants, sizeof(SSceneConstants));
		m_SceneConstants.Update();
	}
}

// --------------------------------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::Render(const SRenderDesc& renderDesc)
{
	if (!m_bInScene)
	{
		FrameDump("Cannot Render Object: Not in scene!");
		return S_ERROR;
	}

	// Skip render desc without subsets
	if (renderDesc.nSubsets == 0 || !IS_VALID_PTR(renderDesc.pSubsets))
	{
		FrameDump("Skipping Render: No subsets");
		return S_SUCCESS;
	}

	EnableBackfaceCulling(false);

	RenderGeometry(renderDesc);

	return S_SUCCESS;
}

// --------------------------------------------------------------------------------------------------------------------
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

// --------------------------------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::RenderTerrain(const STerrainRenderDesc& terrainRenderDesc)
{
	if (!m_bInScene)
	{
		FrameDump("Cannot Render Terrain: Not in scene!");
		return S_INVALIDSTAGE;
	}

	if (!terrainRenderDesc.bRender)
	{
		FrameDump("Terrain not scheduled to be rendered this frame.");
		return S_SUCCESS;
	}

	if (m_CurrentPass != eSHADERPASS_GBUFFER && m_CurrentPass != eSHADERPASS_SHADING)
	{
		FrameDump("Terrain can't be rendered in current shader pass (%s)", GetShaderPassTypeName(m_CurrentPass));
		return S_ERROR;
	}

	if (!terrainRenderDesc.pDrawCallDescs || terrainRenderDesc.nDrawCallDescs == 0)
	{
		return S_SUCCESS;
	}

	FrameDump("Rendering Terrain...");

	// Update scene constants
	SetViewProjMatrix(m_pTargetViewport);
	BindSceneCB(m_SceneConstants.GetCB());

	// Bind terrain shader resources
	STerrainShaderResources sr;
	sr.pHeightmap		= terrainRenderDesc.pVtxHeightMap;
	sr.pLayerMask		= terrainRenderDesc.pLayerMask;
	sr.pColormap		= terrainRenderDesc.pColorMap;
	sr.pTexturemap		= terrainRenderDesc.pTextureMaps;
	sr.pNormalmap		= terrainRenderDesc.pNormalMaps;
	sr.pRoughnessmap	= terrainRenderDesc.pRoughnessMaps;

	if (terrainRenderDesc.bUpdateCB)
		sr.constants = terrainRenderDesc.constants;

	IShaderPass* pShaderPass = GetShaderPass(m_CurrentPass);
	pShaderPass->BindTerrainResources(sr, terrainRenderDesc.bUpdateCB);


	EnableDepthTest(true);
	SetDepthTestFunction(eDEPTH_TEST_LESS);

	EnableBackfaceCulling(true);
	D3D11_SetBlendState(m_pDefBlendState);

	// Draw all chunks
	for (unsigned int c = 0; c < terrainRenderDesc.nDrawCallDescs; ++c)
	{
		DrawTerrainSubset(terrainRenderDesc.pDrawCallDescs[c]);
	}

	return S_SUCCESS;
}

// --------------------------------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::RenderDeferredLight(const SRenderDesc& renderDesc)
{
	EnableBackfaceCulling(true);
	D3D11_SetBlendState(m_pDeferredLightBlendState);

	RenderGeometry(renderDesc, false);

	return S_SUCCESS;
}

// --------------------------------------------------------------------------------------------------------------------
S_API SResult DX11Renderer::RenderFullScreenQuad(bool enableDepthTest /*= false*/)
{
	EnableBackfaceCulling(false);
	D3D11_SetBlendState(m_pDefBlendState);
	
	m_FullscreenPlane.bDepthStencilEnable = enableDepthTest;
	RenderGeometry(m_FullscreenPlane);

	return S_SUCCESS;
}



SP_NMSPACE_END

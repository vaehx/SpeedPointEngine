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
		return;
	}

	// Skip render desc without subsets
	if (renderDesc.nSubsets == 0 || !IS_VALID_PTR(renderDesc.pSubsets))
	{
		FrameDump("Skipping Render: No subsets");
		return;
	}

	EnableBackfaceCulling(true);

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


	// Update Per-Scene Constants Buffer
	SetViewProjMatrix(m_pTargetViewport);

	// In case something else was bound to the slot before...
	BindSceneCB(m_SceneConstants.GetCB());


	// Render Terrain
	if (terrainRenderDesc.bRender)
	{
		bool bTerrainRenderState = true;	// true = success
		FrameDump("Rendering Terrain...");

		// TODO: Make terrain rendering deferred as well
		m_CurrentPass = eSHADERPASS_NONE;

		// Render Terrain directly to the backbuffer
		BindSingleRT(m_pTargetViewport);

		if (!(bTerrainRenderState = IS_VALID_PTR(terrainRenderDesc.pVtxHeightMap))) CLog::Log(S_ERROR, "Invalid terrain vtx heightmap in render desc!");
		if (!(bTerrainRenderState = IS_VALID_PTR(terrainRenderDesc.pColorMap))) CLog::Log(S_ERROR, "Invalid color map in Terrin render Desc!");
		if (!(bTerrainRenderState = IS_VALID_PTR(terrainRenderDesc.pLayerMasks))) CLog::Log(S_ERROR, "Invalid layer masks array in Terarin Render Desc!");
		if (!(bTerrainRenderState = IS_VALID_PTR(terrainRenderDesc.pDetailMaps))) CLog::Log(S_ERROR, "Invalid detail maps array in Terarin Render Desc!");
		if (!(bTerrainRenderState = (terrainRenderDesc.nLayers > 0))) CLog::Log(S_ERROR, "Invalid layer count in Terrain Render Desc!");

		BindTexture(terrainRenderDesc.pVtxHeightMap, 0);
		BindVertexShaderTexture(terrainRenderDesc.pVtxHeightMap, 0);
		BindTexture(terrainRenderDesc.pColorMap, 1);

		// ~~
		// TODO: Avoid referencing a shader pass here
		//			- probably fixed with deferred rendering, because the shadowmap is then only bound in shading pass.
		ShadowmapShaderPass* pShadowmapPass = dynamic_cast<ShadowmapShaderPass*>(GetShaderPass(eSHADERPASS_SHADOWMAP));
		if (pShadowmapPass)
			BindDepthBufferAsTexture(pShadowmapPass->GetShadowmap(), 4);

		if (terrainRenderDesc.bUpdateCB)
		{
			memcpy(m_TerrainConstants.GetConstants(), &terrainRenderDesc.constants, sizeof(terrainRenderDesc.constants));
			m_TerrainConstants.Update();
		}

		EnableDepthTest(true);
		SetDepthTestFunction(eDEPTH_TEST_LESS);

		EnableBackfaceCulling(true);

		// bind terrain cb
		BindConstantsBuffer(m_TerrainConstants.GetCB());

		// render all chunks
		if (IS_VALID_PTR(terrainRenderDesc.pDrawCallDescs) && terrainRenderDesc.nDrawCallDescs > 0)
		{
			for (unsigned int c = 0; c < terrainRenderDesc.nDrawCallDescs; ++c)
			{
				// Draw first layer without alpha blend and default depth state
				D3D11_SetBlendState(m_pDefBlendState);
				m_pD3DDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);

				// For each layer
				for (unsigned int iLayer = 0; iLayer < terrainRenderDesc.nLayers; ++iLayer)
				{
					if (iLayer == 1)
					{
						// Draw each further layer with alpha blending and terrain depth stencil state
						D3D11_SetBlendState(m_pTerrainBlendState);
						m_pD3DDeviceContext->OMSetDepthStencilState(m_pTerrainDepthState, 1);
					}

					// Bind textures
					BindTexture(terrainRenderDesc.pDetailMaps[iLayer], 2);
					BindTexture(terrainRenderDesc.pLayerMasks[iLayer], 3);

					// Draw the subset
					DrawTerrainSubset(terrainRenderDesc.pDrawCallDescs[c]);
				}
			}

			// Unbind terrain layer textures
			UnbindTexture(2);
			UnbindTexture(3);

			// Unbind Vertex Shader texture
			BindVertexShaderTexture((ITexture*)0, 0);
		}

		UnbindTexture(4);

		D3D11_SetBlendState(m_pDefBlendState);
	}
	else
	{
		FrameDump("Terrain not scheduled to be rendered this frame.");
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
S_API SResult DX11Renderer::RenderFullScreenQuad()
{
	Render(m_FullscreenPlane);
}



SP_NMSPACE_END

#include "Geometry.h"
#include "..\RenderMesh.h"
#include "..\IMaterial.h"
#include "C3DEngine.h"
#include <Renderer\IVertexBuffer.h>
#include <Renderer\IResourcePool.h>

SP_NMSPACE_BEG

S_API CRenderMesh::CRenderMesh()
	: m_pGeometry(0),
	m_bBoundBoxInvalid(true)
{
}

S_API void CRenderMesh::Clear()
{
	if (IS_VALID_PTR(m_pGeometry))
	{
		m_pGeometry->Release();
		m_pGeometry = 0;
	}

	if (m_RenderDesc.nSubsets > 0 && m_RenderDesc.pSubsets)
	{
		for (unsigned int i = 0; i < m_RenderDesc.nSubsets; ++i)
		{
			SShaderResources& sr = m_RenderDesc.pSubsets[i].shaderResources;
			if (sr.textureMap) sr.textureMap->Release();
			if (sr.normalMap) sr.normalMap->Release();
			if (sr.roughnessMap) sr.roughnessMap->Release();
		}
	}

	m_RenderDesc.Clear();
}


S_API SResult CRenderMesh::Init(const SInitialGeometryDesc* pGeomDesc)
{
	IGeometry* pGeom = 0;
	if (pGeomDesc)
	{
		IGeometryManager* pGeomMgr = C3DEngine::Get()->GetGeometryManager();
		pGeom = pGeomMgr->CreateGeometry(*pGeomDesc);
	}

	return Init(pGeom);
}

S_API SResult CRenderMesh::Init(IGeometry* pGeometry)
{
	Clear();

	if (!pGeometry)
	{
		// Do not initialize yet
		return S_SUCCESS;
	}

	SetGeometry(pGeometry);

	return S_SUCCESS;
}


S_API SResult CRenderMesh::SetGeometry(IGeometry* pGeometry)
{
	I3DEngine* p3DEngine = C3DEngine::Get();
	if (!IS_VALID_PTR(p3DEngine))
		return CLog::Log(S_ERROR, "Failed CRenderMesh::Init(): 3D Engine not initialized");

	IRenderer* pRenderer = p3DEngine->GetRenderer();
	IResourcePool* pRes = pRenderer->GetResourcePool();

	Clear();

	m_pGeometry = pGeometry;
	m_bBoundBoxInvalid = true;

	// Fill render desc from geometry
	SRenderDesc* rd = GetRenderDesc();
	rd->renderPipeline = eRENDER_FORWARD;
	rd->bDepthStencilEnable = true;
	rd->bInverseDepthTest = false;
	rd->bCustomViewProjMtx = false;

	// copy over subsets
	rd->nSubsets = m_pGeometry->GetSubsetCount();
	rd->pSubsets = new SRenderSubset[rd->nSubsets];
	for (unsigned int i = 0; i < rd->nSubsets; ++i)
	{
		SRenderSubset& renderSubset = rd->pSubsets[i];
		SGeomSubset* subset = m_pGeometry->GetSubset(i);
		if (!IS_VALID_PTR(subset))
		{
			renderSubset.render = false;
			continue;
		}

		renderSubset.drawCallDesc.primitiveType = m_pGeometry->GetPrimitiveType();

		renderSubset.drawCallDesc.pVertexBuffer = m_pGeometry->GetVertexBuffer();
		renderSubset.drawCallDesc.pIndexBuffer = subset->pIndexBuffer;

		if (renderSubset.drawCallDesc.primitiveType == PRIMITIVE_TYPE_LINES)
		{
			renderSubset.render = true;
		}
		else if (IS_VALID_PTR(renderSubset.drawCallDesc.pIndexBuffer))
		{
			renderSubset.render = true;
			renderSubset.drawCallDesc.iStartIBIndex = 0;
			renderSubset.drawCallDesc.iEndIBIndex = renderSubset.drawCallDesc.pIndexBuffer->GetIndexCount() - 1;
		}
		else
		{
			renderSubset.render = false;
		}

		if (!IS_VALID_PTR(renderSubset.drawCallDesc.pVertexBuffer))
		{
			renderSubset.render = false;
		}
		else
		{
			renderSubset.drawCallDesc.iStartVBIndex = 0;
			renderSubset.drawCallDesc.iEndVBIndex = renderSubset.drawCallDesc.pVertexBuffer->GetVertexCount() - 1;
		}

		// Load material or use default
		IMaterial* mat = subset->pMaterial;
		if (!mat)
		{
			// try to get default material, may still result 0
			IMaterialManager* pMatMgr = p3DEngine->GetMaterialManager();
			if (IS_VALID_PTR(pMatMgr))
				mat = pMatMgr->GetDefaultMaterial();
		}

		if (mat)
		{
			// Material is copied over. Warning: Avoid modifying the material during rendering.
			//TODO: To update a material, implement a method to flag a subset material to be updated,
			//TODO:		then do it below when updating the transform.

			SMaterialLayer* matLayer = mat->GetLayer(0);
			renderSubset.shaderResources.roughness = matLayer->roughness;

			if (pRes)
			{
				renderSubset.shaderResources.textureMap = pRes->GetTexture(matLayer->textureMap);
				renderSubset.shaderResources.normalMap = pRes->GetTexture(matLayer->normalMap);
				renderSubset.shaderResources.roughnessMap = pRes->GetTexture(matLayer->textureMap);
			}
		}
	}

	return S_SUCCESS;
}

S_API IGeometry* CRenderMesh::GetGeometry()
{
	return m_pGeometry;
}

S_API SRenderDesc* CRenderMesh::GetRenderDesc()
{
	return &m_RenderDesc;
}

S_API AABB CRenderMesh::GetAABB()
{
	if (m_bBoundBoxInvalid)
	{
		if (IS_VALID_PTR(m_pGeometry))
		{
			m_pGeometry->CalculateBoundBox(m_AABB, Mat44::Identity);
			m_bBoundBoxInvalid = false;
		}
		else
		{
			m_AABB = AABB();
		}
	}

	return m_AABB;
}

S_API void CRenderMesh::SetCustomViewProjMatrix(const Mat44& viewMtx, const Mat44& projMtx)
{
	m_RenderDesc.bCustomViewProjMtx = true;
	m_RenderDesc.viewMtx = viewMtx;
	m_RenderDesc.projMtx = projMtx;
}

S_API IVertexBuffer* CRenderMesh::GetVertexBuffer()
{
	if (m_pGeometry)
		return m_pGeometry->GetVertexBuffer();
	else
		return 0;
}

S_API IIndexBuffer* CRenderMesh::GetIndexBuffer(unsigned int subset)
{
	if (m_pGeometry)
	{
		SGeomSubset* pSubset = m_pGeometry->GetSubset(subset);
		if (pSubset)
			return pSubset->pIndexBuffer;
	}

	return 0;
}

SP_NMSPACE_END

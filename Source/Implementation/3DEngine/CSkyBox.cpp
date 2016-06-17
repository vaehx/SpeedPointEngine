#include <Abstract\IResourcePool.h>
#include <Implementation\3DEngine\CSkyBox.h>
#include <Util\SVertex.h>

SP_NMSPACE_BEG

S_API CSkyBox::CSkyBox()
	: m_pRenderer(0),
	m_Position(0, 0, 0)
{
}

S_API void CSkyBox::InitRenderDesc()
{

}

S_API SResult CSkyBox::InitGeometry(IRenderer* pRenderer, const SShaderResources& shaderResources)
{
	if (!IS_VALID_PTR(pRenderer))
		return S_INVALIDPARAM;

	m_pRenderer = pRenderer;


	// Create the sphere

	float radius = 15.0f;
	unsigned int nStripes = 16; // vertical stripes
	unsigned int nRings = 16;

	unsigned int nVertices = (nStripes + 1) * (nRings + 1);
	SVertex* pVertices = new SVertex[nVertices];

	unsigned int nIndices = (nStripes * nRings) * 6;
	SIndex* pIndices = new SIndex[nIndices];

	EPrimitiveType primitiveType = SpeedPoint::PRIMITIVE_TYPE_TRIANGLELIST;

	float dTheta = (float)SP_PI / (float)nRings;
	float dPhi = (float)SP_PI * 2.0f / (float)nStripes;

	float dU = 1.0f / (float)nRings;
	float dV = 1.0f / (float)nStripes;

	unsigned int indexAccum = 0;
	for (unsigned int ring = 0; ring <= nRings; ++ring)
	{
		float theta = (float)ring * dTheta;
		for (unsigned int stripe = 0; stripe <= nStripes; ++stripe)
		{
			float phi = (float)stripe * dPhi;

			Vec3f normal;
			normal.x = sinf(theta) * cosf(phi);
			normal.y = sinf(theta) * sinf(phi);
			normal.z = cosf(theta);

			pVertices[ring * (nStripes + 1) + stripe] = SVertex(
				radius * normal.x, radius * normal.y, radius * normal.z,
				-normal.x, -normal.y, -normal.z, 0, 0, 0, (float)stripe * dU, (float)ring * dV);

			if (ring < nRings && stripe < nStripes)
			{
				pIndices[indexAccum] = ring * (nStripes + 1) + stripe;
				pIndices[indexAccum + 1] = pIndices[indexAccum] + (nStripes + 1) + 1;
				pIndices[indexAccum + 2] = pIndices[indexAccum] + 1;
				pIndices[indexAccum + 3] = pIndices[indexAccum];
				pIndices[indexAccum + 4] = pIndices[indexAccum] + (nStripes + 1);
				pIndices[indexAccum + 5] = pIndices[indexAccum + 1];
				indexAccum += 6;
			}
		}
	}

	

	// Fill render desc

	IResourcePool* resources = pRenderer->GetResourcePool();

	SRenderDesc& rd = m_RenderDesc;
	rd.renderPipeline = eRENDER_FORWARD;
	rd.bCustomViewProjMtx = false;
	rd.bDepthStencilEnable = false;
	rd.bInverseDepthTest = false;
	rd.nSubsets = 1;
	rd.pSubsets = new SRenderSubset[rd.nSubsets];
	
	SRenderSubset& subset = rd.pSubsets[0];
	resources->AddVertexBuffer(&subset.drawCallDesc.pVertexBuffer);
	subset.drawCallDesc.pVertexBuffer->Initialize(pRenderer, eVBUSAGE_STATIC, pVertices, nVertices);	

	resources->AddIndexBuffer(&subset.drawCallDesc.pIndexBuffer);
	subset.drawCallDesc.pIndexBuffer->Initialize(pRenderer, eIBUSAGE_STATIC, nIndices, pIndices);	

	subset.drawCallDesc.iStartVBIndex = 0;
	subset.drawCallDesc.iEndVBIndex = nVertices - 1;
	subset.drawCallDesc.iStartIBIndex = 0;
	subset.drawCallDesc.iEndIBIndex = nIndices - 1;
	subset.drawCallDesc.primitiveType = PRIMITIVE_TYPE_TRIANGLELIST;

	delete[] pVertices;
	delete[] pIndices;



	// Setup Material

	subset.shaderResources = shaderResources;
	subset.shaderResources.emissive = float3(0.5f, 0.5f, 0.5f);
	subset.shaderResources.illumModel = eILLUM_SKYBOX;

	return S_SUCCESS;
}

S_API void CSkyBox::SetTexture(ITexture* pTexture)
{
	if (m_RenderDesc.nSubsets == 0 || !IS_VALID_PTR(m_RenderDesc.pSubsets))
		return;

	SShaderResources& sr = m_RenderDesc.pSubsets[0].shaderResources;
	sr.textureMap = pTexture;
	sr.enableBackfaceCulling = false;
}

S_API void CSkyBox::SetPosition(const Vec3f& pos)
{
	m_Position = pos;
}

S_API void CSkyBox::Clear()
{
	if (IS_VALID_PTR(m_RenderDesc.pSubsets))
	{
		SDrawCallDesc* dcd = &m_RenderDesc.pSubsets[0].drawCallDesc;
		if (IS_VALID_PTR(m_pRenderer))
		{
			IResourcePool* resources = m_pRenderer->GetResourcePool();
			resources->RemoveVertexBuffer(&dcd->pVertexBuffer);
			resources->RemoveIndexBuffer(&dcd->pIndexBuffer);
		}

		delete[] m_RenderDesc.pSubsets;
	}

	m_RenderDesc.pSubsets = 0;
	m_RenderDesc = SRenderDesc();

	m_Position = Vec3f(0);

	m_pRenderer = 0;
}

S_API SRenderDesc* CSkyBox::GetRenderDesc()
{
	return &m_RenderDesc;
}

S_API void CSkyBox::Update()
{
	// set / update transformation
	STransformationDesc& transformDesc = m_RenderDesc.transform;
	transformDesc.translation = SMatrix::MakeTranslationMatrix(m_Position);

	SMatrixIdentity(transformDesc.rotation);
	SMatrixIdentity(transformDesc.scale);	
}

SP_NMSPACE_END
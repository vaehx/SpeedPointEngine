//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2015, Pascal Rosenkranz
// ------------------------------------------------------------------------------
// Filename:	CReferenceObject.cpp
// Created:	2/14/2015 by Pascal Rosenkranz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#include <Implementation\Geometry\RenderableObjects.h>

SP_NMSPACE_BEG


S_API IRenderableComponent* CReferenceObject::GetRenderable() const
{
	return IS_VALID_PTR(m_pBase) ? m_pBase->GetRenderable() : 0;
}

S_API IPhysicalComponent* CReferenceObject::GetPhysical() const
{
	return IS_VALID_PTR(m_pBase) ? m_pBase->GetPhysical() : 0;
}

S_API IAnimateableComponent* CReferenceObject::GetAnimateable() const
{
	return IS_VALID_PTR(m_pBase) ? m_pBase->GetAnimateable() : 0;
}

S_API IScriptComponent* CReferenceObject::GetScriptable() const
{
	return IS_VALID_PTR(m_pBase) ? m_pBase->GetScriptable() : 0;
}







S_API CSkyBox::CSkyBox()
	: m_pEngine(0),
	m_Position(0,0,0)
{
}

S_API SResult CSkyBox::InitGeometry(IGameEngine* pEngine)
{	
	if (!IS_VALID_PTR(pEngine))
		return S_INVALIDPARAM;

	m_pEngine = pEngine;

	// Create the sphere
	SInitialGeometryDesc initGeom;
	initGeom.bRequireNormalRecalc = false;
	initGeom.bRequireTangentRecalc = false;
	initGeom.indexUsage = eGEOMUSE_STATIC;
	initGeom.vertexUsage = eGEOMUSE_STATIC;
		
	float radius = 15.0f;
	unsigned int nStripes = 16; // vertical stripes
	unsigned int nRings = 16;

	initGeom.nVertices = (nStripes + 1) * (nRings + 1);
	initGeom.pVertices = new SVertex[initGeom.nVertices];

	initGeom.nSubsets = 1;
	initGeom.pSubsets = new SInitialSubsetGeometryDesc[1];

	initGeom.pSubsets[0].nIndices = (nStripes * nRings) * 6;
	initGeom.pSubsets[0].pIndices = new SIndex[initGeom.pSubsets[0].nIndices];

	initGeom.primitiveType = SpeedPoint::PRIMITIVE_TYPE_TRIANGLELIST;	

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

			initGeom.pVertices[ring * (nStripes + 1) + stripe] = SVertex(
				radius * normal.x, radius * normal.y, radius * normal.z,
				-normal.x, -normal.y, -normal.z, 0, 0, 0, (float)stripe * dU, (float)ring * dV);

			if (ring < nRings && stripe < nStripes)
			{
				initGeom.pSubsets[0].pIndices[indexAccum] = ring * (nStripes + 1) + stripe;
				initGeom.pSubsets[0].pIndices[indexAccum + 1] = initGeom.pSubsets[0].pIndices[indexAccum] + (nStripes + 1) + 1;
				initGeom.pSubsets[0].pIndices[indexAccum + 2] = initGeom.pSubsets[0].pIndices[indexAccum] + 1;
				initGeom.pSubsets[0].pIndices[indexAccum + 3] = initGeom.pSubsets[0].pIndices[indexAccum];
				initGeom.pSubsets[0].pIndices[indexAccum + 4] = initGeom.pSubsets[0].pIndices[indexAccum] + (nStripes + 1);
				initGeom.pSubsets[0].pIndices[indexAccum + 5] = initGeom.pSubsets[0].pIndices[indexAccum + 1];
				indexAccum += 6;
			}
		}
	}	

	m_Renderable.GetGeometry()->Init(pEngine, pEngine->GetRenderer(), &initGeom);

	delete[] initGeom.pVertices;

	delete[] initGeom.pSubsets[0].pIndices;
	delete[] initGeom.pSubsets;


	// Setup material
	SGeomSubset* pSubset = m_Renderable.GetGeometry()->GetSubset(0);
	if (IS_VALID_PTR(pSubset))
	{
		pSubset->pMaterial = pEngine->GetMaterialManager()->GetMaterial("skybox_mat");

		SShaderResources& matResources = pSubset->pMaterial->GetLayer(0)->resources;
		matResources.emissive = float3(0.5f, 0.5f, 0.5f);
		matResources.illumModel = eILLUM_SKYBOX;
	}


	return S_SUCCESS;
}

S_API void CSkyBox::SetTexture(ITexture* pTexture)
{
	SGeomSubset* pSubset = m_Renderable.GetGeometry()->GetSubset(0);
	if (!IS_VALID_PTR(pSubset))
		return;

	IMaterial* pMaterial = pSubset->pMaterial;
	if (!IS_VALID_PTR(pMaterial))
	{
		CLog::Log(S_ERROR, "Can't set Skybox texture: Material is 0");
		return;
	}

	SMaterialLayer* pMaterialLayer = pMaterial->GetLayer(0);
	if (!IS_VALID_PTR(pMaterialLayer))
		return;

	SShaderResources* pShaderResources = &pMaterialLayer->resources;
	pShaderResources->textureMap = pTexture;
	pShaderResources->enableBackfaceCulling = false;
}

S_API void CSkyBox::SetPosition(const Vec3f& pos)
{
	m_Position = pos;
}

S_API void CSkyBox::Clear()
{
	m_Renderable.Clear();
}

S_API SRenderDesc* CSkyBox::GetUpdatedRenderDesc()
{
	if (!IS_VALID_PTR(m_pEngine))
	{
		EngLog(S_NOTINIT, m_pEngine, "Cannot Update Static Object REnder Desc: Engine not set!");
		return 0;
	}

	if (!m_Renderable.RenderDescFilled())
	{
		m_Renderable.FillRenderDesc(m_pEngine);		
	}	

	SRenderDesc* pDestDesc = m_Renderable.GetUpdatedRenderDesc();

	//pDestDesc->textureSampling = eTEX_SAMPLE_POINT;

	pDestDesc->bInverseDepthTest = false;
	pDestDesc->bDepthStencilEnable = false;	

	// set / update transformation
	STransformationDesc& transformDesc = pDestDesc->transform;
	transformDesc.translation = SMatrix::MakeTranslationMatrix(m_Position);	
	
	SMatrixIdentity(transformDesc.rotation);
	SMatrixIdentity(transformDesc.scale);

	return pDestDesc;
}


SP_NMSPACE_END
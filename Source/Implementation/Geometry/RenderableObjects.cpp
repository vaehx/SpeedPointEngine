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


S_API SResult CReferenceObject::Render()
{
	if (!IS_VALID_PTR(m_pEngine) || !IS_VALID_PTR(m_pBase))
		return S_NOTINIT;

	IRenderableComponent* pRenderable = m_pBase->GetRenderable();
	if (!IS_VALID_PTR(pRenderable))
		return S_NOTINIT;

	if (!IS_VALID_PTR(m_pRenderSlot))
	{
		m_pRenderSlot = m_pEngine->GetRenderer()->GetRenderSlot();
		if (!IS_VALID_PTR(m_pRenderSlot))
			return S_ERROR;

		pRenderable->FillRenderSlot(m_pEngine, m_pRenderSlot);		
	}

	STransformationDesc& transformDesc = m_pRenderSlot->renderDesc.transform;
	transformDesc.translation = SMatrix::MakeTranslationMatrix(vPosition);
	transformDesc.rotation = SMatrix::MakeRotationMatrix(vRotation);
	transformDesc.scale = SMatrix::MakeScaleMatrix(vSize);

	return S_SUCCESS;
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
		
	float radius = 5.0f;
	unsigned int nStripes = 8; // vertical stripes
	unsigned int nRings = 8;

	initGeom.nVertices = (nStripes + 1) * (nRings + 1);
	initGeom.nIndices = (nStripes * nRings) * 6;

	initGeom.pVertices = new SVertex[initGeom.nVertices];
	initGeom.pIndices = new SIndex[initGeom.nIndices];

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
				initGeom.pIndices[indexAccum] = ring * (nStripes + 1) + stripe;
				initGeom.pIndices[indexAccum + 1] = initGeom.pIndices[indexAccum] + (nStripes + 1) + 1;
				initGeom.pIndices[indexAccum + 2] = initGeom.pIndices[indexAccum] + 1;
				initGeom.pIndices[indexAccum + 3] = initGeom.pIndices[indexAccum];
				initGeom.pIndices[indexAccum + 4] = initGeom.pIndices[indexAccum] + (nStripes + 1);
				initGeom.pIndices[indexAccum + 5] = initGeom.pIndices[indexAccum + 1];
				indexAccum += 6;
			}
		}
	}

	m_Renderable.GetGeometry()->Init(pEngine, pEngine->GetRenderer(), &initGeom);

	delete[] initGeom.pVertices;
	delete[] initGeom.pIndices;

	return S_SUCCESS;
}

S_API void CSkyBox::SetTexture(ITexture* pTexture)
{
	SGeomSubset* pSubset = m_Renderable.GetGeometry()->GetSubset(0);
	if (!IS_VALID_PTR(pSubset))
		return;

	IMaterial* pMaterial = pSubset->pMaterial;
	if (!IS_VALID_PTR(pMaterial))
		return;

	SMaterialLayer* pMaterialLayer = pMaterial->GetLayer(0);
	if (!IS_VALID_PTR(pMaterialLayer))
		return;

	SShaderResources* pShaderResources = &pMaterialLayer->resources;
	pShaderResources->textureMap = pTexture;
}

S_API void CSkyBox::Clear()
{
	m_Renderable.Clear();
}

S_API SResult CSkyBox::Render()
{
	SRenderSlot* pRenderSlot = m_Renderable.GetRenderSlot();
	SRenderDesc* pRenderDesc = 0;

	IRenderer* pRenderer = 0;
	if (!IS_VALID_PTR(m_pEngine) || !IS_VALID_PTR((pRenderer = m_pEngine->GetRenderer())))
		return EngLog(S_NOTINIT, m_pEngine, "Cannot Render Static Object: Engine or Renderer not set!");

	// Add RenderScheduleSlot if not there already
	if (m_Renderable.GetRenderSlot() == 0)
	{
		pRenderSlot = pRenderer->GetRenderSlot();
		if (!IS_VALID_PTR(pRenderSlot))
			return S_ERROR;

		pRenderDesc = &pRenderSlot->renderDesc;

		m_Renderable.FillRenderSlot(m_pEngine, pRenderSlot);
		m_Renderable.SetRenderSlot(pRenderSlot);
	}
	else
	{
		pRenderDesc = &pRenderSlot->renderDesc;
	}

	// set / update transformation
	STransformationDesc& transformDesc = pRenderDesc->transform;
	transformDesc.translation = SMatrix::MakeTranslationMatrix(vPosition);
	transformDesc.rotation = SMatrix::MakeRotationMatrix(vRotation);
	transformDesc.scale = SMatrix::MakeScaleMatrix(vSize);

	return S_SUCCESS;
}

S_API IGeometry* CSkyBox::GetGeometry()
{
	return m_Renderable.GetGeometry();
}

S_API IRenderableComponent* CSkyBox::GetRenderable()
{
	return &m_Renderable;
}



SP_NMSPACE_END
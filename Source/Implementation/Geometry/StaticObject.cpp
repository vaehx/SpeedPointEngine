//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2014, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	StaticObject.cpp
// Created:	8/19/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#include <Implementation\Geometry\StaticObject.h>
#include <Implementation\Geometry\Material.h>
#include <Abstract\IResourcePool.h>
#include <Abstract\IRenderer.h>
#include <Abstract\IVertexBuffer.h>
#include <Abstract\IIndexBuffer.h>

SP_NMSPACE_BEG

// ----------------------------------------------------------------------------------------
S_API StaticObject::StaticObject()
{
}

// ----------------------------------------------------------------------------------------
S_API StaticObject::~StaticObject()
{
	Clear();
}

// ----------------------------------------------------------------------------------------
S_API void StaticObject::Clear()
{
	m_Geometry.Clear();
	if (IS_VALID_PTR(m_pMaterials))
	{
		delete[] m_pMaterials;
		/*
		if (m_nMaterials > 1)
			delete[] m_pMaterials;
		else if (m_nMaterials == 1)
			delete m_pMaterials;
			*/
	}

	m_pMaterials = 0;
	m_nMaterials = 0;
	m_pEngine = 0;
}

// ----------------------------------------------------------------------------------------
S_API SResult StaticObject::Init(IGameEngine* pEngine, IRenderer* pRenderer, const SInitialMaterials* pInitialMaterials /*=0*/, SInitialGeometryDesc* pInitialGeom /*=0*/)
{
	Clear();

	if (!IS_VALID_PTR((m_pEngine = pEngine)))
		return S_INVALIDPARAM;

	if (IS_VALID_PTR(pInitialMaterials) && pInitialMaterials->nMaterials > 0)
	{
		m_pMaterials = new SMaterial[pInitialMaterials->nMaterials];
		m_nMaterials = pInitialMaterials->nMaterials;
		memcpy(m_pMaterials, pInitialMaterials->pMaterials, sizeof(SMaterial) * pInitialMaterials->nMaterials);

		if (IS_VALID_PTR(pInitialGeom) && IS_VALID_PTR(pInitialGeom->pMatIndexAssigns) && pInitialGeom->nMatIndexAssigns > 0)
		{
			// find matching material pointers
			for (unsigned int iMatIndexAssign = 0; iMatIndexAssign < pInitialGeom->nMatIndexAssigns; ++iMatIndexAssign)
			{
				SMaterialIndices& matIndexAssign = pInitialGeom->pMatIndexAssigns[iMatIndexAssign];
				matIndexAssign.pMaterial = 0;
				for (unsigned short iMat = 0; iMat < m_nMaterials; ++iMat)
				{
					if (strcmp(m_pMaterials[iMat].name, matIndexAssign.materialName) != 0)
						continue;

					matIndexAssign.pMaterial = &m_pMaterials[iMat];
					break;
				}
			}
		}
	}

	return m_Geometry.Init(pEngine, pRenderer, pInitialGeom);
}


// ----------------------------------------------------------------------------------------
S_API void StaticObject::SetMaterial(const SMaterial& singleMat)
{
	if (!IS_VALID_PTR(m_pMaterials))
		m_pMaterials = new SMaterial[1];			

	*m_pMaterials = singleMat;

	if (m_nMaterials == 0)
		m_nMaterials = 1;
}

// ----------------------------------------------------------------------------------------
S_API SMaterial* StaticObject::GetSingleMaterial()
{
	if (!IS_VALID_PTR(m_pMaterials))
		SetMaterial(SMaterial());

	return m_pMaterials;
}






// ----------------------------------------------------------------------------------------
S_API SResult StaticObject::Render()
{
	if (m_Geometry.GetIndexBufferCount() == 0)
		return S_ERROR;

	IRenderer* pRenderer = m_Geometry.GetRenderer();
	if (!IS_VALID_PTR(pRenderer))
		return S_NOTINIT;

	IVertexBuffer* pVB = m_Geometry.GetVertexBuffer();
	if (!IS_VALID_PTR(pVB))
		return S_ERROR;	

	if (!pVB->IsInited())
		return EngLog(S_ERROR, m_pEngine, "Cannot render Static Object: VB never initialized!");

	SRenderDesc dsc;
	dsc.drawCallDesc.pVertexBuffer = pVB;
	dsc.drawCallDesc.iStartVBIndex = 0;
	dsc.drawCallDesc.iEndVBIndex = pVB->GetVertexCount() - 1;

	SGeometryIndexBuffer* pGeomIndexBuffers = m_Geometry.GetIndexBuffers();
	if (!IS_VALID_PTR(pGeomIndexBuffers))
		return S_ERROR;

	dsc.pGeometry = &m_Geometry;
	dsc.technique = eRENDER_FORWARD;
	dsc.drawCallDesc.transform.translation = SMatrix::MakeTranslationMatrix(vPosition);
	dsc.drawCallDesc.transform.rotation = SMatrix::MakeRotationMatrix(vRotation);
	dsc.drawCallDesc.transform.scale = SMatrix::MakeScaleMatrix(vSize);

	for (unsigned short iIndexBuffer = 0; iIndexBuffer < m_Geometry.GetIndexBufferCount(); ++iIndexBuffer)
	{		
		dsc.drawCallDesc.pIndexBuffer = pGeomIndexBuffers[iIndexBuffer].pIndexBuffer;
		if (!dsc.drawCallDesc.pIndexBuffer->IsInited())
		{
			EngLog(S_ERROR, m_pEngine, "Could not render Static object: IB never initialized!");
			continue;
		}

		dsc.drawCallDesc.iStartIBIndex = 0;
		dsc.drawCallDesc.iEndIBIndex = dsc.drawCallDesc.pIndexBuffer->GetIndexCount() - 1;		

		// here, index buffer index has to match with material index
		if (IS_VALID_PTR(pGeomIndexBuffers[iIndexBuffer].pMaterial))
			dsc.material = *pGeomIndexBuffers[iIndexBuffer].pMaterial;
		else
			dsc.material = SMaterial();

		RETURN_ON_ERR(pRenderer->RenderGeometry(dsc));
	}

	return S_SUCCESS;
}








SP_NMSPACE_END
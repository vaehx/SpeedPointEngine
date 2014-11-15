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
	m_Material.Clear();
}

// ----------------------------------------------------------------------------------------
S_API SResult StaticObject::Init(IGameEngine* pEngine, IRenderer* pRenderer, const Material& material, SInitialGeometryDesc* pInitialGeom /* = nullptr*/)
{
	m_Material = material;
	return Init(pEngine, pRenderer, pInitialGeom);
}

// ----------------------------------------------------------------------------------------
S_API SResult StaticObject::Init(IGameEngine* pEngine, IRenderer* pRenderer, SInitialGeometryDesc* pInitialGeom /* = nullptr */)
{
	Clear();
	return m_Geometry.Init(pEngine, pRenderer, pInitialGeom);	
}











// ----------------------------------------------------------------------------------------
S_API SResult StaticObject::Render()
{
	IRenderer* pRenderer = m_Geometry.GetRenderer();
	if (!IS_VALID_PTR(pRenderer))
		return S_NOTINIT;

	IVertexBuffer* pVB = m_Geometry.GetVertexBuffer();
	if (!IS_VALID_PTR(pVB))
		return S_ERROR;

	IIndexBuffer* pIB = m_Geometry.GetIndexBuffer();
	if (!IS_VALID_PTR(pIB))
		return S_ERROR;

	SRenderDesc dsc;
	dsc.drawCallDesc.pVertexBuffer = pVB;
	dsc.drawCallDesc.iStartVBIndex = 0;
	dsc.drawCallDesc.iEndVBIndex = pVB->GetVertexCount() - 1;
	dsc.drawCallDesc.pIndexBuffer = pIB;
	dsc.drawCallDesc.iStartIBIndex = 0;
	dsc.drawCallDesc.iEndIBIndex = pIB->GetIndexCount() - 1;

	dsc.pGeometry = &m_Geometry;
	dsc.technique = eRENDER_FORWARD;	
	dsc.drawCallDesc.transform.translation = SMatrix::MakeTranslationMatrix(vPosition);
	dsc.drawCallDesc.transform.rotation = SMatrix::MakeRotationMatrix(vRotation);
	dsc.drawCallDesc.transform.scale = SMatrix::MakeScaleMatrix(vSize);

	dsc.material = m_Material;

	return pRenderer->RenderGeometry(dsc);
}








SP_NMSPACE_END
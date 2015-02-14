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

		pRenderable->FillRenderSlot(m_pRenderSlot);		
	}

	STransformationDesc& transformDesc = m_pRenderSlot->renderDesc.transform;
	transformDesc.translation = SMatrix::MakeTranslationMatrix(vPosition);
	transformDesc.rotation = SMatrix::MakeRotationMatrix(vRotation);
	transformDesc.scale = SMatrix::MakeScaleMatrix(vSize);

	return S_SUCCESS;
}


SP_NMSPACE_END
//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2015, Pascal Rosenkranz
// ------------------------------------------------------------------------------
// Filename:	RenderableObjects.h
// Created:	2/14/2015 by Pascal Rosenkranz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <Abstract\Renderable.h>
#include "StaticObject.h"

SP_NMSPACE_BEG


class S_API CReferenceObject : public IReferenceObject
{
private:
	IRenderableObject* m_pBase;
	SRenderDesc m_RenderDesc;
	IGameEngine* m_pEngine;

	// IReferenceObject:
public:
	CReferenceObject(IGameEngine* pEngine)
		: m_pEngine(pEngine) {}

	virtual ~CReferenceObject()
	{
	}

	virtual IRenderableObject* GetBase()
	{
		return m_pBase;
	}

	virtual SResult SetBase(IRenderableObject* base)
	{	
		m_pBase = base;
		return S_SUCCESS;
	}	

	// IRenderableObject:
public:
	virtual SRenderDesc* GetUpdatedRenderDesc();

	virtual IGeometry* GetGeometry()
	{
		if (!IS_VALID_PTR(m_pBase))
			return 0;

		return m_pBase->GetGeometry();
	}

	virtual IRenderableComponent* GetRenderable()
	{
		if (IS_VALID_PTR(m_pBase))
			return m_pBase->GetRenderable();

		return 0;
	}

	// IObject:
public:
	virtual void RecalcBoundBox()
	{
		if (IS_VALID_PTR(m_pBase))
		{
			m_pBase->RecalcBoundBox();
			m_AABB = m_pBase->GetBoundBox();
		}
	}
};


// SpeedPoint SkyBox Implementation
class S_API CSkyBox : public ISkyBox
{
private:
	CStaticObjectRenderable m_Renderable;
	IGameEngine* m_pEngine;

public:
	~CSkyBox()
	{
		Clear();
	}

	virtual SResult InitGeometry(IGameEngine* pEngine);
	virtual void SetTexture(ITexture* pTexture);
	virtual void Clear();

	virtual SRenderDesc* GetUpdatedRenderDesc(const SCamera* pCamera);
};


SP_NMSPACE_END
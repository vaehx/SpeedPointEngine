//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2015, Pascal Rosenkranz
// ------------------------------------------------------------------------------
// Filename:	Renderable.h
// Created:	2/8/2015 by Pascal Rosenkranz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SPrerequisites.h>
#include "IObject.h"
#include <vector>
#include "IRenderer.h"

using std::vector;

SP_NMSPACE_BEG

struct S_API IReferenceObject;

// only stores data for renderable, no functionality
struct IRenderableComponent
{
	virtual ~IRenderableComponent()
	{
	}

	virtual IGeometry* GetGeometry() = 0;	
	virtual void Clear() = 0;
	
	virtual SRenderDesc* GetRenderDesc() = 0;

	ILINE virtual bool RenderDescFilled() const = 0;

	// Fills the render Desc and returns reference to it
	// pEngine is required to access several resources
	// USE THIS FUNCTION TO INITIALLY FILL THE RENDER DESC OF THE OBJECT, THEN USE GetUpdatedRenderDesc()
	virtual SRenderDesc* FillRenderDesc(IGameEngine* pEngine) = 0;	

	virtual void SetVisible(bool visible) = 0;

	// Should be called by RenderableObject::GetUpdatedRenderDesc()
	virtual SRenderDesc* GetUpdatedRenderDesc() = 0;

	virtual IVertexBuffer* GetVertexBuffer() = 0;
	virtual SGeomSubset* GetSubset(unsigned int i) = 0;
	virtual unsigned int GetSubsetCount() const = 0;
	virtual EPrimitiveType GetGeometryPrimitiveType() const = 0;

	virtual void SetViewProjMatrix(const SMatrix& mtx) = 0;	
	virtual void UnsetViewProjMatrix() = 0;
};




struct S_API IRenderableObject : public IObject
{
	virtual bool IsRenderable() const { return true; }

	virtual IRenderableComponent* GetRenderable() = 0;	

	virtual void SetVisible(bool visible)
	{
		GetRenderable()->SetVisible(visible);
	}

	virtual IGeometry* GetGeometry() = 0;

	virtual SRenderDesc* GetUpdatedRenderDesc() = 0;

	// Warning: References created on heap. PAY ATTENTION TO DELETE THEM PROPERLY!
	// If the base object is destructed, the base pointer in the reference object might get
	// automatically zero'ed, depending on its implementation.
	virtual IReferenceObject* CreateReferenceObject() { return 0; }
};



// Summary:
//	An simple object that can be rendered and positioned in the world
//	When implementing, also inherit from STransformable to add support for transformation tools
struct S_API IStaticObject : public IRenderableObject
{
	virtual ~IStaticObject()
	{
	}

	virtual EObjectType GetType() const
	{
		return eGEOMOBJ_STATIC;
	}
	
	// Looks for the materials in the material manager based on the material names set in pInitialGeom.
	// Set material pointers are set regardless of whether they've been set yet or not.
	virtual SResult Init(IGameEngine* pEngine, SInitialGeometryDesc* pInitialGeom = nullptr) = 0;

	virtual IMaterial* GetSubsetMaterial(unsigned int subset = 0) = 0;
	virtual unsigned int GetSubsetCount() const = 0;

	virtual void Clear() = 0;
};



// Renderable object that is just a reference (an instance) to another renderable object
struct S_API IReferenceObject : public IRenderableObject
{
	virtual ~IReferenceObject()
	{
	}

	virtual IRenderableObject* GetBase() = 0;		
	virtual SResult SetBase(IRenderableObject* base) = 0;

	// always returns 0 as you cannot create a reference from a reference!
	virtual IReferenceObject* CreateReferenceObject()
	{
		return 0;
	}	

	// To get the type of the referenced object use GetBase()->GetType()
	virtual EObjectType GetType() const
	{
		return eGEOMOBJ_REFERENCE;
	}
};


struct S_API ISkyBox
{
	virtual ~ISkyBox() {}	

	virtual SResult InitGeometry(IGameEngine* pEngine) = 0;
	virtual void SetTexture(ITexture* pTexture) = 0;
	virtual void Clear() = 0;

	// Updates the renderDesc at the given Cameras position
	virtual SRenderDesc* GetUpdatedRenderDesc(const SCamera* pCamera) = 0;
};


SP_NMSPACE_END
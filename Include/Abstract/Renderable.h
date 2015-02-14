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

// only stores data for renderable, no functionality
struct IRenderableComponent
{
	virtual ~IRenderableComponent()
	{
	}

	virtual IGeometry* GetGeometry() = 0;
	virtual MaterialPtrList& GetMaterials() = 0;
	virtual unsigned int GetMaterialCount() const = 0;
	virtual void Clear() = 0;
	
	virtual SRenderSlot* GetRenderSlot() = 0;
	virtual void SetRenderSlot(SRenderSlot* pSlot) = 0;

	virtual IVertexBuffer* GetVertexBuffer() = 0;
	virtual SGeomSubset* GetSubset(unsigned int i) = 0;
	virtual unsigned int GetSubsetCount() const = 0;
	virtual EPrimitiveType GetGeometryPrimitiveType() const = 0;
};




struct S_API IRenderableObject : public IObject
{
	// Summary:
	//	Renders the object the way, the implementation defines it.		
	virtual SResult Render() = 0;

	virtual bool IsRenderable() const { return true; }	

	// Moved this to Scene! Get normal geometry from GetGeometry();
	//virtual SResult CreateNormalsGeometry(IRenderableObject** pNormalGeometryObject) const = 0;

	virtual IGeometry* GetGeometry() = 0;
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

	// pInitialMaterials - If nullptr, then the engine's default material is taken
	virtual SResult Init(IGameEngine* pEngine, SInitialGeometryDesc* pInitialGeom = nullptr, MaterialPtrList* pInitialMaterials = nullptr) = 0;

	virtual MaterialPtrList& GetMaterials() = 0;
	virtual unsigned int GetMaterialCount() const = 0;

	// Summary:
	//	Sets first material, which is the single material if this object has no subsets
	virtual void SetSingleMaterial(SMaterial* singleMat) = 0;

	// Summary:
	//	Verifies that there is a single material and always returns a non-null pointer
	virtual SMaterial* GetSingleMaterial() = 0;

	virtual void Clear() = 0;
};


SP_NMSPACE_END
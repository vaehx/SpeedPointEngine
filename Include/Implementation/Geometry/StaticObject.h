// ********************************************************************************************

//	SpeedPoint Basic Solid

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\Renderable.h>
#include <Abstract\IOctree.h>
#include <Util\SPool.h>
#include <Util\SPrimitive.h>
#include "Geometry.h"
#include <Abstract\Transformable.h>
#include <Abstract\BoundBox.h>
#include <vector>


SP_NMSPACE_BEG

using std::vector;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class S_API CStaticObjectRenderable : public IRenderableComponent
{
private:	
	Geometry m_Geometry;
	MaterialPtrList m_Materials;
	SRenderSlot* m_pRenderSlot;

public:
	virtual ~CStaticObjectRenderable()
	{
		Clear();
	}

	virtual IGeometry* GetGeometry() { return (IGeometry*)&m_Geometry; };
	virtual MaterialPtrList& GetMaterials() { return m_Materials; };
	virtual unsigned int GetMaterialCount() const { return m_Materials.GetCount(); };

	virtual void Clear()
	{
		m_Geometry.Clear();
		m_Materials.Clear();
	}

	virtual SRenderSlot* GetRenderSlot() { return m_pRenderSlot; }
	virtual void SetRenderSlot(SRenderSlot* pSlot) { m_pRenderSlot = pSlot; }

	virtual void FillRenderSlot(SRenderSlot* pSlot);

	virtual IVertexBuffer* GetVertexBuffer();
	virtual SGeomSubset* GetSubset(unsigned int i);
	virtual unsigned int GetSubsetCount() const;
	virtual EPrimitiveType GetGeometryPrimitiveType() const;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// SpeedPoint Solid implementation
class S_API StaticObject : public IStaticObject
{
private:
	IGameEngine* m_pEngine;
	CStaticObjectRenderable m_Renderable;

	vector<IReferenceObject*> m_RefObjects;

public:			

	StaticObject();
	virtual ~StaticObject();
	
	virtual SResult Init(IGameEngine* pEngine, SInitialGeometryDesc* pInitialGeom = nullptr, MaterialPtrList* pInitialMaterials = nullptr);

	virtual EObjectType GetType() const
	{
		return eGEOMOBJ_STATIC;
	}

	virtual IGeometry* GetGeometry()
	{
		return m_Renderable.GetGeometry();
	}
	virtual MaterialPtrList& GetMaterials()
	{
		return m_Renderable.GetMaterials();
	}	
	virtual unsigned int GetMaterialCount() const
	{
		return m_Renderable.GetMaterialCount();
	}
	
	virtual void SetSingleMaterial(SMaterial* pMaterial);
	virtual SMaterial* GetSingleMaterial();

	//virtual SResult CreateNormalsGeometry(IRenderableObject** pNormalGeometryObject) const;	

	virtual SResult Render();
	virtual void Clear();

	virtual IReferenceObject* CreateReferenceObject();

	virtual IRenderableComponent* GetRenderable()
	{
		return (IRenderableComponent*)&m_Renderable;
	}

	// from IObject:
public:
	virtual void RecalcBoundBox();	


/*	virtual SResult SetGeometryData( SVertex* pVertices, UINT nVertices, SIndex* pdwIndices, UINT nIndices );
	virtual SP_ID GetVertexBuffer( void );
	virtual SP_ID GetIndexBuffer( void );

	virtual SResult RenderSolid( SpeedPointEngine* pEngineReplacement = 0 );

	// -- Material --
	virtual void SetMaterial( const SMaterial& mat );
	virtual SMaterial* GetMaterial( void );

	// -- BoundBox --
	virtual SBoundBox* RecalculateBoundBox( void );
	virtual SBoundBox* GetBoundBox( void );
	virtual SBoundBox* SetBoundBox( const SBoundBox& bb );

	// -- Octree --
	virtual IOctree* GetOctree( void );

	// -- Animations --
	virtual void SetAnimationBundle( SP_ID iBundle );
	virtual SP_ID GetAnimationBundle( void );
	virtual SResult RunAnimation( SString cName );
	virtual SResult GetCurrentAnimation( SString* dest );
	virtual SResult TickAnimation( float fFrameDelay );
*/
};


SP_NMSPACE_END
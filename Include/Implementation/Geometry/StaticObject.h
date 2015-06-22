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
	SMatrix m_ViewProjMtx;
	bool m_bUseCustomViewProjMtx;
	SRenderDesc m_RenderDesc;
	bool m_bRenderDescFilled;
	IObject* m_pObject;	

	bool m_bVisible;

public:	
	CStaticObjectRenderable();

	virtual ~CStaticObjectRenderable()
	{
		m_bRenderDescFilled = false;
		Clear();
	}

	virtual IGeometry* GetGeometry() { return (IGeometry*)&m_Geometry; };

	virtual void Clear()
	{
		m_Geometry.Clear();		
	}

	virtual SRenderDesc* GetRenderDesc();

	ILINE virtual void SetVisible(bool visible);

	ILINE virtual bool RenderDescFilled() const
	{
		return m_bRenderDescFilled;
	}	

	virtual SRenderDesc* FillRenderDesc(IGameEngine* pEngine);
	virtual SRenderDesc* GetUpdatedRenderDesc();

	virtual IVertexBuffer* GetVertexBuffer();
	virtual SGeomSubset* GetSubset(unsigned int i);
	virtual unsigned int GetSubsetCount() const;
	virtual EPrimitiveType GetGeometryPrimitiveType() const;

	virtual void SetViewProjMatrix(const SMatrix& mtx)
	{
		m_ViewProjMtx = mtx;
		m_bUseCustomViewProjMtx = true; 
	}
	virtual void UnsetViewProjMatrix()
	{
		m_bUseCustomViewProjMtx = false;
	}
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
	
	virtual SResult Init(IGameEngine* pEngine, SInitialGeometryDesc* pInitialGeom = nullptr);

	virtual EObjectType GetType() const
	{
		return eGEOMOBJ_STATIC;
	}

	virtual IGeometry* GetGeometry()
	{
		return m_Renderable.GetGeometry();
	}
	
	virtual IMaterial* GetSubsetMaterial(unsigned int subset = 0);
	virtual unsigned int GetSubsetCount() const;		

	//virtual SResult CreateNormalsGeometry(IRenderableObject** pNormalGeometryObject) const;	

	virtual SRenderDesc* GetUpdatedRenderDesc();
	virtual void Clear();

	virtual IReferenceObject* CreateReferenceObject();

	virtual IRenderableComponent* GetRenderable()
	{
		return (IRenderableComponent*)&m_Renderable;
	}

	// from IObject:
public:
	virtual void RecalcBoundBox();
};


SP_NMSPACE_END
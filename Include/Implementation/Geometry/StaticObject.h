// ********************************************************************************************

//	SpeedPoint Basic Solid

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\IObject.h>
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

	
	// IRenderableComponent:
public:
	virtual ~CStaticObjectRenderable()
	{
		m_bRenderDescFilled = false;
		Clear();
	}


	virtual void Clear()
	{
		m_Geometry.Clear();
	}

	ILINE virtual void SetVisible(bool visible);

	ILINE virtual void GetUpdatedRenderDesc(SRenderDesc* pDescDest);

	virtual IGeometry* GetGeometry() { return (IGeometry*)&m_Geometry; };

	virtual IVertexBuffer* GetVertexBuffer();
	virtual SGeomSubset* GetSubset(unsigned int i);
	virtual unsigned int GetSubsetCount() const;

	virtual IMaterial* GetSubsetMaterial(unsigned int subset = 0) const;

	virtual void SetViewProjMatrix(const SMatrix& mtx)
	{
		m_ViewProjMtx = mtx;
		m_bUseCustomViewProjMtx = true;
	}
	virtual void UnsetViewProjMatrix()
	{
		m_bUseCustomViewProjMtx = false;
	}

public:	
	CStaticObjectRenderable();

	SRenderDesc* GetRenderDesc();

	ILINE virtual bool RenderDescFilled() const
	{
		return m_bRenderDescFilled;
	}

	SRenderDesc* FillRenderDesc(IGameEngine* pEngine);	
};



class StaticObject : public IStaticObject
{
private:
	AABB m_AABB;
	IGameEngine* m_pEngine;
	CStaticObjectRenderable m_Renderable;

	vector<IReferenceObject*> m_RefObjects;

public:

	StaticObject();	

	SResult Init(IGameEngine* pEngine, SInitialGeometryDesc* pInitialGeom = nullptr);

	IGeometry* GetGeometry()
	{
		return m_Renderable.GetGeometry();
	}

	IMaterial* GetSubsetMaterial(unsigned int subset = 0);
	unsigned int GetSubsetCount() const;

	IRenderableComponent* GetRenderable()
	{
		return (IRenderableComponent*)&m_Renderable;
	}

	void RecalcBoundBox();

	// IStaticObject:
public:
	virtual ~StaticObject();

	virtual const AABB& GetBoundBox() const
	{
		return m_AABB;
	}

	virtual SRenderDesc* GetRenderDesc();
	virtual void Clear();
};


SP_NMSPACE_END
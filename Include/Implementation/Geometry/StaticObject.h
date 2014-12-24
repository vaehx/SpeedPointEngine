// ********************************************************************************************

//	SpeedPoint Basic Solid

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\IObject.h>
#include <Abstract\IOctree.h>
#include <Util\SBoundBox.h>
#include <Util\SPool.h>
#include <Util\SPrimitive.h>
#include "Geometry.h"
#include <Abstract\Transformable.h>


SP_NMSPACE_BEG


// SpeedPoint Solid implementation
class S_API StaticObject : public IStaticObject
{
private:
	IGameEngine* m_pEngine;

	Geometry m_Geometry;	
	
	unsigned short m_nMaterials;
	SMaterial* m_pMaterials;

public:			

	StaticObject();
	virtual ~StaticObject();
	
	virtual SResult Init(IGameEngine* pEngine, IRenderer* pRenderer, const SInitialMaterials* pInitialMaterials = nullptr, SInitialGeometryDesc* pInitialGeom = nullptr);

	virtual EObjectType GetType() const
	{
		return eGEOMOBJ_STATIC;
	}

	virtual IGeometry* GetGeometry()
	{
		return &m_Geometry;
	}
	virtual SMaterial* GetMaterials()
	{
		return m_pMaterials;
	}	
	virtual unsigned short GetMaterialCount() const
	{
		return m_nMaterials;
	}

	virtual void SetMaterial(const SMaterial& singleMat);
	virtual SMaterial* GetSingleMaterial();

	virtual SResult CreateNormalsGeometry(IRenderableObject** pNormalGeometryObject) const;

	virtual SResult Render();
	virtual void Clear();


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
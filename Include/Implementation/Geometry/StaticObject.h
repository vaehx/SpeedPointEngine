// ********************************************************************************************

//	SpeedPoint Basic Solid

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\Objects.h>
#include <Abstract\IOctree.h>
#include <Util\SBoundBox.h>
#include "Material.h"
#include <Util\SPool.h>
#include <Util\SPrimitive.h>
#include "Geometry.h"
#include <Util\STransformable.h>


SP_NMSPACE_BEG


// SpeedPoint Solid implementation
class S_API StaticObject : public IStaticObject, public STransformable
{
private:
	Geometry m_Geometry;
	Material m_Material;	// maybe use index to an element in a buffer of materials	

public:		
	StaticObject();
	virtual ~StaticObject();

	virtual SResult Init(IGameEngine* pEngine, IRenderer* pRenderer, SInitialGeometryDesc* pInitialGeom = nullptr);
	virtual SResult Init(IGameEngine* pEngine, IRenderer* pRenderer, const Material& material, SInitialGeometryDesc* pInitialGeom = nullptr) = 0;

	virtual EObjectType GetType() const
	{
		return eGEOMOBJ_STATIC;
	}

	virtual IGeometry* GetGeometry()
	{
		return &m_Geometry;
	}
	virtual IMaterial* GetMaterial()
	{
		return &m_Material;
	}	

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
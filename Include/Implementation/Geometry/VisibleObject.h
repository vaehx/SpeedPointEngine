// ********************************************************************************************

//	SpeedPoint Basic Solid

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\IVisbleObject.h>
#include <Abstract\IOctree.h>
#include <Util\STransformable.h>
#include <Util\SBoundBox.h>
#include <Util\SMaterial.h>
#include <Util\SPool.h>
#include <Util\SPrimitive.h>
#include "RawGeometrical.h"


SP_NMSPACE_BEG


// SpeedPoint Solid implementation
class S_API VisibleObject : public IVisibleObject
{
private:
	IRenderer* m_pRenderer;

	SP_ID m_iIndexBuffer;
	SP_ID m_iVertexBuffer;
	IMaterial* m_pMaterial;

public:		
	VisibleObject();
	virtual ~VisibleObject();

	virtual SResult Init(IGameEngine* pEngine, IRenderer* pRenderer, SInitialGeometryDesc* pInitialGeom = nullptr);

	virtual IIndexBuffer* GetIndexBuffer() const;
	virtual const SP_ID& GetIndexBufferResIndex() const { return m_iIndexBuffer; }
	virtual IVertexBuffer* GetVertexBuffer() const;
	virtual const SP_ID& GetVertexBufferResIndex() const { return m_iVertexBuffer; }

	virtual EGeometryObjectType GetType() const
	{
		return eGEOMOBJ_VISIBLE;
	}

	virtual IMaterial* GetMaterial() const;

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
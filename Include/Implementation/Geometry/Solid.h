// ********************************************************************************************

//	SpeedPoint Basic Solid

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\ISolid.h>
#include <Abstract\IOctree.h>
#include <Util\STransformable.h>
#include <Util\SBoundBox.h>
#include <Util\SMaterial.h>
#include <Util\SPool.h>
#include <Util\SPrimitive.h>

namespace SpeedPoint
{
	// SpeedPoint Solid implementation
	class S_API Solid : public ISolid
	{
	private:
		bool m_bInitialized;
		SpeedPointEngine* m_pEngine;
		
		SP_ID m_iAnimationBundle;
		
		SP_ID m_iVertexBuffer;
		SP_ID m_iIndexBuffer;
		
		SPool<SPrimitive> m_plPrimitives;
		bool m_bDynamic;
		
		SBoundBox m_bbBoundBox;							
		IOctree* m_pOctree;

		SMaterial m_matMaterial;				

		ERenderPipelineStrategy m_RenderPipeStrategy;



	public:		
		Solid();		
		~Solid();

		// -- General --
		virtual SResult Initialize( SpeedPointEngine* pEngine, bool bDynamic );
		virtual SResult Clear( void );

		virtual SResult SetGeometryData( SVertex* pVertices, UINT nVertices, DWORD* pdwIndices, UINT nIndices );
		virtual SP_ID GetVertexBuffer( void );
		virtual SP_ID GetIndexBuffer( void );

		virtual SResult RenderSolid( SpeedPointEngine* pEngineReplacement = 0 );

		// -- Primitive --
		virtual SP_ID AddPrimitive( const SPrimitive& primitive );
		virtual SPrimitive* GetPrimitive( SP_ID id );
		virtual SPrimitive* GetPrimitive( SP_UNIQUE index );
		virtual usint32 GetPrimitiveCount( void );

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
	};
}
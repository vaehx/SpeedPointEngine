// ********************************************************************************************

//	SpeedPoint Basic Solid

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\SSolid.h>
#include <Abstract\SOctree.h>
#include <STransformable.h>
#include <SBoundBox.h>
#include <SMaterial.h>
#include <SPool.h>
#include <SPrimitive.h>

namespace SpeedPoint
{
	// SpeedPoint Solid
	class S_API SBasicSolid : public SSolid
	{
	public:
		bool			bInitialized;
		SpeedPointEngine*	pEngine;
		SP_ID			iAnimationBundle;
		SP_ID			iVertexBuffer;
		SP_ID			iIndexBuffer;
		SBoundBox		bbBoundBox;
		SMaterial		matMaterial;
		SPool<SPrimitive>	plPrimitives;
		bool			bDynamic;
		SOctree*		pOctree;

		// Default constructor
		SBasicSolid()
			: pEngine( NULL ),
			bInitialized( false ),
			pOctree( NULL ) {};

		// -- General --
		SResult Initialize( SpeedPointEngine* pEngine, bool bDynamic );
		SResult Clear( void );

		SResult SetGeometryData( SVertex* pVertices, UINT nVertices, DWORD* pdwIndices, UINT nIndices );
		SP_ID GetVertexBuffer( void );
		SP_ID GetIndexBuffer( void );

		virtual SResult RenderSolid( SpeedPointEngine* pEngineReplacement = 0 );

		// -- Primitive --
		SP_ID AddPrimitive( const SPrimitive& primitive );
		SPrimitive* GetPrimitive( SP_ID id );
		SPrimitive* GetPrimitive( SP_UNIQUE index );
		int GetPrimitiveCount( void );

		// -- Material --
		void SetMaterial( const SMaterial& mat );
		SMaterial* GetMaterial( void );

		// -- BoundBox --
		SBoundBox* RecalculateBoundBox( void );
		SBoundBox* GetBoundBox( void );
		SBoundBox* SetBoundBox( const SBoundBox& bb );

		// -- Octree --
		SOctree* GetOctree( void );

		// -- Animations --
		void SetAnimationBundle( SP_ID iBundle );
		SP_ID GetAnimationBundle( void );
		SResult RunAnimation( SString cName );
		SResult GetCurrentAnimation( SString* dest );
		SResult TickAnimation( float fFrameDelay );
	};
}
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
	// SpeedPoint Solid
	class S_API SBasicSolid : public ISolid
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
		IOctree*		pOctree;

		// Default constructor
		SBasicSolid()
			: pEngine( NULL ),
			bInitialized( false ),
			pOctree( NULL )
		{
		};

		// Default destructor
		~SBasicSolid();

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
		usint32 GetPrimitiveCount( void );

		// -- Material --
		void SetMaterial( const SMaterial& mat );
		SMaterial* GetMaterial( void );

		// -- BoundBox --
		SBoundBox* RecalculateBoundBox( void );
		SBoundBox* GetBoundBox( void );
		SBoundBox* SetBoundBox( const SBoundBox& bb );

		// -- Octree --
		IOctree* GetOctree( void );

		// -- Animations --
		void SetAnimationBundle( SP_ID iBundle );
		SP_ID GetAnimationBundle( void );
		SResult RunAnimation( SString cName );
		SResult GetCurrentAnimation( SString* dest );
		SResult TickAnimation( float fFrameDelay );
	};
}
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

namespace SpeedPoint
{
	// SpeedPoint Solid
	class S_API SBasicSolid : public SSolid
	{
	public:
		SpeedPointEngine*	pEngine;
		SP_ID			iAnimationBundle;
		SP_ID			iVertexBuffer;
		SP_ID			iIndexBuffer;
		SBoundBox		bbBoundBox;
		SMaterial		matMaterial;
		SPool<SPrimitive>	aPrimitives;
		bool			bDynamic;
		SOctree*		pOctree;

		// Default constructor
		SBasicSolid() {};

		// -- General --
		SResult Initialize( SpeedPointEngine* pEngine, bool bDynamic );
		SResult Clear( void );
		SP_ID GetVertexBuffer( void );
		SP_ID GetIndexBuffer( void );

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
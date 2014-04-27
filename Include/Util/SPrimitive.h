// ********************************************************************************************

//	SpeedPoint Primitive

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>

namespace SpeedPoint
{
	// SpeedPoint Primitive
	struct S_API SPrimitive
	{
		S_PRIMITIVE_TYPE	tType;
		S_PRIMITIVE_RENDER_TYPE	tRenderType;
		SP_ID			iTexture;	// The texture for the polygons assigned to this primitive

		DWORD			iFirstIndex;	// ID of the first index
		DWORD			iLastIndex;	// ID of the last index
		DWORD			iFirstVertex;	// Index of the first vertex
		DWORD			iLastVertex;	// Index of the last vertex
		int			nPolygons;	// Number of polygons
		bool			bDraw;		// Is this primitive drawn?

		// Default constructor
		SPrimitive()
			: tType( S_PRIM_COMPLEX ),
			tRenderType( S_PRIM_RENDER_TRIANGLELIST ),
			iFirstIndex( 0 ),
			iLastIndex( 0 ),
			iFirstVertex( 0 ),
			iLastVertex( 0 ),
			nPolygons( 0 ) {}

		// Copy constructor
		SPrimitive( const SPrimitive& other )
			: tType( other.tType ),
			tRenderType( other.tRenderType ),
			iFirstIndex( other.iFirstIndex ),
			iLastIndex( other.iLastIndex ),
			iFirstVertex( other.iFirstVertex ),
			iLastVertex( other.iLastVertex ),
			nPolygons( other.nPolygons ) {}
	};

}
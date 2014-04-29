// ********************************************************************************************

//	SpeedPoint Solid

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include <Util\STransformable.h>

namespace SpeedPoint
{
	// SpeedPoint Solid (abstract)
	class S_API ISolid : public STransformable
	{
	public:
		// -- General --

		// Clear everything out, that is stored inside this solid
		// including Buffers, primitives and material BUT NOT textures or Animations
		virtual SResult Clear( void ) = 0;

		// Initialize this solid with given pointer to the Engine instance
		virtual SResult Initialize( SpeedPointEngine* pEngine, bool bDynamic ) = 0;

		// Set both (Hardware) Vertex and Index Buffer data immediately
		virtual SResult SetGeometryData( SVertex* pVertices, UINT nVertices, DWORD* pdwIndices, UINT nIndices ) = 0;

		// Get the id of the vertex buffer
		virtual SP_ID GetVertexBuffer( void ) = 0;				

		// Get the id of the index buffer
		virtual SP_ID GetIndexBuffer( void ) = 0;

		// Render this Solid
		virtual SResult RenderSolid( SpeedPointEngine* pEngineReplacement = 0 ) = 0;

		// -- Primitive --

		// Get pointer to a single primitive by its id
		virtual SPrimitive* GetPrimitive( SP_ID id ) = 0;

		// Get pointer to a single primitive by its index
		virtual SPrimitive* GetPrimitive( SP_UNIQUE index ) = 0;

		// Add a primitive
		virtual SP_ID AddPrimitive( const SPrimitive& primitive ) = 0;

		// Get Primitive count
		virtual usint32 GetPrimitiveCount( void ) = 0;		

		// -- Material --

		// Set the material of this solid
		virtual void SetMaterial( const SMaterial& mat ) = 0;

		// Get the material of this solid
		virtual SMaterial* GetMaterial( void ) = 0;		

		// -- BoundBox --

		// Recalculate the Axis BoundBox
		virtual SBoundBox* RecalculateBoundBox( void ) = 0;

		// Get the current BoundBox
		virtual SBoundBox* GetBoundBox( void ) = 0;

		// Set the BoundBox to a custom one
		virtual SBoundBox* SetBoundBox( const SBoundBox& bbox ) = 0;

		// -- Octree --

		// Get the octree
		virtual IOctree* GetOctree( void ) = 0;

		// -- Animations --

		// Set the id of the animation bundle to be assigned with this solid
		virtual void SetAnimationBundle( SP_ID iBundle ) = 0;

		// Get the id of the animation bundle assigned to this solid
		virtual SP_ID GetAnimationBundle( void ) = 0;

		// Set the currently running animation by its specification
		virtual SResult RunAnimation( SString cName ) = 0;

		// Get the specification of the currently running animation
		virtual SResult GetCurrentAnimation( SString* cOutput ) = 0;

		// Tick the current animation sequence
		virtual SResult TickAnimation( float fFrameDelay ) = 0;			
	};
}
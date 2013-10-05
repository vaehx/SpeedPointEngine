// ********************************************************************************************

//	SpeedPoint Animation Joint

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include <SVector3.h>

namespace SpeedPoint
{
	// SpeedPoint Animation Joint (abstract)
	class S_API SAnimationJoint
	{
	public:
		// Define which solid the vertex indices belong to
		virtual void SetSolid( SSolid* pSolid ) = 0;

		// Assign a vertex index to this Joint
		virtual void AssignVertex( SP_UNIQUE iVertex ) = 0; 

		// Set the name of this joint
		virtual void SetName( const SString& name ) = 0;

		// Get the name of this joint
		virtual SString* GetName( void ) = 0;
		
		// Clearout associated vertices of this joint
		virtual void Clear( void ) = 0;
		
		// Move all vertices by the specified vector
		virtual void MoveVertices( const SVector3& vec ) = 0;

		//// TODO
	};
}
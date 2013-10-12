// ********************************************************************************************

//	SpeedPoint Transformable object

// ********************************************************************************************

#pragma once
#include "SPrerequisites.h"
#include "SVector3.h"

namespace SpeedPoint
{
	// SpeedPoint Transformable Object
	class S_API STransformable
	{
	public:
		SVector3 vPosition;
		SVector3 vRotation;
		SVector3 vSize;

		// Default constructor
		STransformable() : vSize( 1.0f ) {};

		// Constructor with position
		STransformable( const SVector3& pos ) : vPosition( pos ), vSize( 1.0f ) {};

		// Constructor with position and rotation
		STransformable( const SVector3& pos, const SVector3& rot ) : vPosition( pos ), vRotation( rot ), vSize( 1.0f ) {};

		// Constructor with position, rotation and scale
		STransformable( const SVector3& pos, const SVector3& rot, const SVector3& scale )
			: vPosition( pos ),
			vRotation( rot ),
			vSize( scale ) {};

		// Copy constructor
		STransformable( const STransformable& other )
			: vPosition( other.vPosition ),
			vRotation( other.vRotation ),
			vSize( other.vSize ) {};

		// Move absolutely
		void Move( const SVector3& pos );

		// Move absolutely
		void Move( float x, float y, float z );

		// Translate relatively
		void Translate( const SVector3& vec );

		// Translate relatively
		void Translate( float x, float y, float z );

		// Rotate absolutely
		void Rotate( const SVector3& rotation );

		// Rotate absolutely
		void Rotate( float p, float y, float r );

		// Turn relatively
		void Turn( const SVector3& vec );

		// Turn relatively
		void Turn( float p, float y, float r );

		// Scale absolutely
		void Size( const SVector3& size );

		// Scale absolutely
		void Size( float w, float h, float d );

		// Scale relatively
		void Scale( const SVector3& vec );

		// Scale relatively
		void Scale( float w, float h, float d );
	};
}
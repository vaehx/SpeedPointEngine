// SpeedPoint Transformable object

#include <STransformable.h>

namespace SpeedPoint
{
	// **********************************************************************************

	S_API void STransformable::Move( const SVector3& pos )
	{
		vPosition = pos;		
		RecalculateWorldMatrix();
	}

	// **********************************************************************************

	S_API void STransformable::Move( float x, float y, float z )
	{
		vPosition = SVector3( x, y, z );
		RecalculateWorldMatrix();
	}

	// **********************************************************************************

	S_API void STransformable::Translate( const SVector3& vec )
	{
		vPosition += vec;
		RecalculateWorldMatrix();
	}

	// **********************************************************************************

	S_API void STransformable::Translate( float x, float y, float z )
	{
		vPosition += SVector3( x, y, z );
		RecalculateWorldMatrix();
	}

	// **********************************************************************************

	S_API void STransformable::Rotate( const SVector3& rotation )
	{
		vRotation = rotation;
		RecalculateWorldMatrix();
	}

	// **********************************************************************************

	S_API void STransformable::Rotate( float p, float y, float r )
	{
		vRotation = SVector3( p, y, r );
		RecalculateWorldMatrix();
	}

	// **********************************************************************************

	S_API void STransformable::Turn( const SVector3& vec )
	{
		vRotation += vec;
		RecalculateWorldMatrix();
	}

	// **********************************************************************************

	S_API void STransformable::Turn( float p, float y, float r )
	{
		vRotation += SVector3( p, y, r );
		RecalculateWorldMatrix();
	}

	// **********************************************************************************

	S_API void STransformable::Size( const SVector3& size )
	{
		vSize = size;
		RecalculateWorldMatrix();
	}

	// **********************************************************************************

	S_API void STransformable::Size( float w, float h, float d )
	{
		vSize = SVector3( w, h, d );
		RecalculateWorldMatrix();
	}

	// **********************************************************************************

	S_API void STransformable::Scale( const SVector3& vec )
	{
		vSize += vec;
		RecalculateWorldMatrix();
	}

	// **********************************************************************************

	S_API void STransformable::Scale( float w, float h, float d )
	{
		vSize += SVector3( w, h, d );
		RecalculateWorldMatrix();
	}

	// **********************************************************************************

	S_API SMatrix4 STransformable::GetWorldMatrix()
	{
		return m_WorldMatrix;
	}

	// **********************************************************************************

	S_API SMatrix4 STransformable::RecalculateWorldMatrix()
	{
		// ~~~~~~~~~~~
		/// TODO !!!!!!!!!!		
		// ~~~~~~~~~~~
	}

	// **********************************************************************************
}
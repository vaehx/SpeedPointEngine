// SpeedPoint Transformable object

#include <Util\STransformable.h>

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

	S_API inline SMatrix4& STransformable::GetWorldMatrix()
	{
		if (!m_WorldMatrixCalculated)
			RecalculateWorldMatrix();

		return m_WorldMatrix;
	}

	// **********************************************************************************

	S_API inline SMatrix4& STransformable::RecalculateWorldMatrix()
	{
		// World / Form matrix
		//D3DXMATRIX mTrans, mRot, mScale, mOrig;
		//
		//D3DXMatrixIdentity(&mOrig);
		//D3DXMatrixRotationYawPitchRoll(&mRot, form->vRotation.y, form->vRotation.x, form->vRotation.z);
		//D3DXMatrixTranslation(&mTrans, form->vPosition.x, form->vPosition.y, form->vPosition.z);
		//D3DXMatrixScaling(&mScale, form->vSize.x, form->vSize.y, form->vSize.z);
		//
		//D3DXMATRIX mW = mOrig * mScale * mRot * mTrans;
		//
		//mWorld = DXMatrixToDMatrix(mW);

		m_WorldMatrix = SMatrix4(	// identity
			SVector4(1.0f,   0,     0,    0),
			SVector4(   0, 1.0f,    0,    0),
			SVector4(   0,    0, 1.0f,    0),
			SVector4(   0,    0,    0, 1.0f)
			) * SMatrix4(	// Size
			SVector4(vSize.x, 0, 0, 0),
			SVector4(0, vSize.y, 0, 0),
			SVector4(0, 0, vSize.z, 0),
			SVector4(0, 0, 0, 1.0f)
			) * SMatrix4(	// Yaw
			SVector4(cosf(vRotation.y), 0, sinf(vRotation.y), 0),
			SVector4(0, 1.0f, 0, 0),
			SVector4(-sinf(vRotation.y), 0, cosf(vRotation.y), 0),
			SVector4(0, 0, 0, 1.0f)
			) * SMatrix4(		// Pitch
			SVector4(1.0f, 0, 0, 0),
			SVector4(0, cosf(vRotation.x), -sinf(vRotation.x), 0),
			SVector4(0, sinf(vRotation.x), cosf(vRotation.x), 0),
			SVector4(0, 0, 0, 1.0f)
			) * SMatrix4(		// Roll
			SVector4(cosf(vRotation.z), -sinf(vRotation.z), 0, 0),
			SVector4(sinf(vRotation.z), cosf(vRotation.z), 0, 0),
			SVector4(0, 0, 1.0f, 0),
			SVector4(0, 0, 0, 1.0f)
			) * SMatrix4(	// Translation
			SVector4(1.0f, 0, 0, vPosition.x),
			SVector4(0, 1.0f, 0, vPosition.y),
			SVector4(0, 0, 1.0f, vPosition.z),
			SVector4(0, 0, 0, 1.0f)
			);

		m_WorldMatrixCalculated = true;

		return m_WorldMatrix;
	}

	// **********************************************************************************
}
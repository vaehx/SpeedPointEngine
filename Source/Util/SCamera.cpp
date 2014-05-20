// ********************************************************************************************

//	SpeedPoint Camera

// ********************************************************************************************

#include <Util\SCamera.h>

namespace SpeedPoint
{	
	// ********************************************************************************************

	S_API SMatrix4& SCamera::GetViewMatrix()
	{
		if (!m_bViewMatrixCalculated)
			RecalculateViewMatrix();

		return m_ViewMatrix;
	}

	// ********************************************************************************************

	S_API SMatrix4& SCamera::RecalculateViewMatrix()
	{
		if (!m_bViewMatrixCalculated)
		{
			m_vLookAt = vPosition.x + sin(vRotation.y) * cos(vRotation.x);
			m_vLookAt = vPosition.y + sin(vRotation.x);
			m_vLookAt = vPosition.z + cos(vRotation.y) * cos(vRotation.x);

			SPMatrixLookAtRH(&m_ViewMatrix, vPosition, m_vLookAt, SVector3(0, 1, 0));
			m_bViewMatrixCalculated = true;
		}

		return m_ViewMatrix;
	}

	// ********************************************************************************************

	S_API void SCamera::Move(const SVector3& pos)	// abs
	{
		if (pos != vPosition)
		{
			STransformable::Move(pos);
			m_bViewMatrixCalculated = false;
		}
	}

	// ********************************************************************************************

	S_API void SCamera::Move(float x, float y, float z)	// abs
	{
		if (vPosition.x != x || vPosition.y != y || vPosition.z != z)
		{
			STransformable::Move(x, y, z);
			m_bViewMatrixCalculated = false;
		}
	}

	// ********************************************************************************************

	S_API void SCamera::Translate(const SVector3& vec)	// rel
	{
		if (vec.x != 0 || vec.y != 0 || vec.z != 0)
		{
			STransformable::Translate(vec);
			m_bViewMatrixCalculated = false;
		}
	}

	// ********************************************************************************************

	S_API void SCamera::Translate(float x, float y, float z)	// rel
	{
		if (x != 0 || y != 0 || z != 0)
		{
			STransformable::Translate(x, y, z);
			RecalculateViewMatrix();
		}
	}

	// ********************************************************************************************

	S_API void SCamera::Rotate(const SVector3& rotation)	// abs
	{
		if (rotation != vRotation)
		{
			STransformable::Rotate(rotation);
			m_bViewMatrixCalculated = false;
		}
	}

	// ********************************************************************************************

	S_API void SCamera::Rotate(float p, float y, float r)	// abs
	{
		if (p != vRotation.x || y != vRotation.y || r != vRotation.z)
		{
			STransformable::Rotate(p, y, r);
			m_bViewMatrixCalculated = false;
		}
	}

	// ********************************************************************************************

	S_API void SCamera::Turn(const SVector3& vec)	// rel
	{
		if (vec.x != 0 || vec.y != 0 || vec.z != 0)
		{
			STransformable::Turn(vec);
			m_bViewMatrixCalculated = false;
		}
	}

	// ********************************************************************************************

	S_API void SCamera::Turn(float p, float y, float r)	// rel
	{
		if (p != 0 || y != 0 || r != 0)
		{
			STransformable::Turn(p, y, r);
			m_bViewMatrixCalculated = false;
		}
	}

	// ********************************************************************************************

	S_API void SCamera::Size(const SVector3& size)
	{
		return; // u cannot scale the camera
	}

	// ********************************************************************************************

	S_API void SCamera::Size(float w, float h, float d)
	{
		return; // u cannot scale the camera
	}
	
	// ********************************************************************************************
	S_API void SCamera::Scale(const SVector3& vec)
	{
		return; // u cannot scale the camera
	}

	// ********************************************************************************************

	S_API void SCamera::Scale(float w, float h, float d)
	{
		return; // u cannot scale the camera
	}
}
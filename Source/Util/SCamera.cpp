// ********************************************************************************************

//	SpeedPoint Camera

// ********************************************************************************************

#include <Util\SCamera.h>

namespace SpeedPoint
{	
	// ********************************************************************************************

	S_API inline SMatrix4& SCamera::GetViewMatrix()
	{
		if (!m_bViewMatrixCalculated)
			RecalculateViewMatrix();

		return m_ViewMatrix;
	}

	// ********************************************************************************************

	S_API inline SMatrix4& SCamera::RecalculateViewMatrix()
	{
		//zaxis = normal(Eye - At)
		//	xaxis = normal(cross(Up, zaxis))
		//	yaxis = cross(zaxis, xaxis)
		//
		//	xaxis.x           yaxis.x           zaxis.x          0
		//	xaxis.y           yaxis.y           zaxis.y          0
		//	xaxis.z           yaxis.z           zaxis.z          0
		//	dot(xaxis, eye)   dot(yaxis, eye)   dot(zaxis, eye)  1

		//D3DXVECTOR3 vEyePt(camera->vPosition.x, camera->vPosition.y, camera->vPosition.z);		

		// !! We do not recalculate the look-at position in each frame, as trigonometrical functions have a high perf cost
		/*D3DXVECTOR3 vLookAt(
			camera->vPosition.x + sin(camera->vRotation.y)*cos(camera->vRotation.x),
			camera->vPosition.y + sin(camera->vRotation.x),
			camera->vPosition.z + cos(camera->vRotation.y) * cos(camera->vRotation.x)
			);*/

		// we assume that EyePos - At vector has length 1, as we use unit polar coords
		SVector3 zaxis = vPosition - m_vLookAt;
		SVector3 xaxis = SVector3Cross(SVector3(0.0f, 1.0f, 0.0), zaxis).Normalize();
		SVector3 yaxis = SVector3Cross(zaxis, xaxis);

		m_ViewMatrix = SMatrix4(
			SVector4(xaxis.x, yaxis.x, zaxis.x, 0),
			SVector4(xaxis.y, yaxis.y, zaxis.y, 0),
			SVector4(xaxis.z, yaxis.z, zaxis.z, 0),
			SVector4(SVector3Dot(xaxis, vPosition), SVector3Dot(yaxis, vPosition), SVector3Dot(zaxis, vPosition), 1.0f)
			);

		m_bViewMatrixCalculated = true;

		return m_ViewMatrix;
	}

	// ********************************************************************************************

	S_API void SCamera::Move(const SVector3& pos)	// abs
	{
		if (pos != vPosition)
		{
			STransformable::Move(pos);
			RecalculateViewMatrix();
		}
	}

	// ********************************************************************************************

	S_API void SCamera::Move(float x, float y, float z)	// abs
	{
		if (vPosition.x != x || vPosition.y != y || vPosition.z != z)
		{
			STransformable::Move(x, y, z);
			RecalculateViewMatrix();
		}
	}

	// ********************************************************************************************

	S_API void SCamera::Translate(const SVector3& vec)	// rel
	{
		if (vec.x != 0 || vec.y != 0 || vec.z != 0)
		{
			STransformable::Translate(vec);
			RecalculateViewMatrix();
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
			RecalculateViewMatrix();
		}
	}

	// ********************************************************************************************

	S_API void SCamera::Rotate(float p, float y, float r)	// abs
	{
		if (p != vRotation.x || y != vRotation.y || r != vRotation.z)
		{
			STransformable::Rotate(p, y, r);
			RecalculateViewMatrix();
		}
	}

	// ********************************************************************************************

	S_API void SCamera::Turn(const SVector3& vec)	// rel
	{
		if (vec.x != 0 || vec.y != 0 || vec.z != 0)
		{
			STransformable::Turn(vec);
			RecalculateViewMatrix();
		}
	}

	// ********************************************************************************************

	S_API void SCamera::Turn(float p, float y, float r)	// rel
	{
		if (p != 0 || y != 0 || r != 0)
		{
			STransformable::Turn(p, y, r);
			RecalculateViewMatrix();
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
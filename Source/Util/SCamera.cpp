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
}
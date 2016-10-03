

#include <Abstract\Camera.h>

SP_NMSPACE_BEG

S_API void SCamera::Turn(float yaw, float pitch)
{	
	Quat qYaw = Quat::FromRotationY(-yaw);
	Quat qPitch = Quat::FromAxisAngle(GetLeft(), pitch);
	Quat rotation = qYaw * qPitch;

	d_turnQuat = rotation;

	Vec3f left = (rotation * GetLeft()).Normalized();
	Vec3f up = (rotation * GetUp()).Normalized();
	Vec3f forward = (rotation * GetForward()).Normalized();

	viewMatrix = SMatrix(
		left.x, up.x, forward.x, 0,
		left.y, up.y, forward.y, 0,
		left.z, up.z, forward.z, 0,
		0, 0, 0, 1
		);

	RecalculateViewMatrix();
}

S_API Vec3f SCamera::GetForward() const
{
	return Vec3f(viewMatrix._13, viewMatrix._23, viewMatrix._33).Normalized();
}

S_API Vec3f SCamera::GetLeft() const
{
	return Vec3f(viewMatrix._11, viewMatrix._21, viewMatrix._31).Normalized();
}

S_API Vec3f SCamera::GetUp() const
{
	return Vec3f(viewMatrix._12, viewMatrix._22, viewMatrix._32).Normalized();
}

S_API SMatrix& SCamera::RecalculateViewMatrix(bool roll/*=false*/)
{	
	viewMatrix._41 = -Vec3Dot(position, GetLeft());
	viewMatrix._42 = -Vec3Dot(position, GetUp());
	viewMatrix._43 = -Vec3Dot(position, GetForward());	
	//SPMatrixLookAtRH(&viewMatrix, position, position + Vec3f(0, 1.0f, 1.0f).Normalized(), Vec3f(0, 1.0f, 0));
	return viewMatrix;
}


SP_NMSPACE_END
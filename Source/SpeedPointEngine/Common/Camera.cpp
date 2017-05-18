#include "Camera.h"
#include "Vector2.h"

SP_NMSPACE_BEG

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

S_API ViewFrustum::ViewFrustum(const Mat44& _mtxView, const Mat44& _mtxProj)
	: mtxView(_mtxView),
	mtxProj(_mtxProj),
	bViewDirCalculated(false),
	bEnclosingPlanesCalculated(false),
	bCornersCalculated(false)
{
}

S_API Vec3f ViewFrustum::GetViewDirection()
{
	if (!bViewDirCalculated)
	{
		Mat44 mtxViewInv = SMatrixInvert(mtxView);

		viewDir = (mtxViewInv * Vec4f(0, 0, 1.0f, 0.0f)).xyz().Normalized();

		bViewDirCalculated = true;
	}

	return viewDir;
}

S_API void ViewFrustum::GetEnclosingPlanes(SPlane _planes[4])
{
	if (!bEnclosingPlanesCalculated)
	{
		CalculateCorners();
		planes[0] = SPlane::FromPoints(corners[3], corners[0], corners[7]); // Left
		planes[1] = SPlane::FromPoints(corners[6], corners[5], corners[2]); // Right
		planes[2] = SPlane::FromPoints(corners[0], corners[1], corners[4]); // Top
		planes[3] = SPlane::FromPoints(corners[2], corners[3], corners[6]); // Bottom
		bEnclosingPlanesCalculated = true;
	}

	if (!_planes)
		return;

	for (int i = 0; i < 4; ++i)
		_planes[i] = planes[i];
}

S_API float ViewFrustum::GetNearZ()
{
	GetCorners(0);
	return corners[0].z;
}

S_API float ViewFrustum::GetFarZ()
{
	GetCorners(0);
	return corners[4].z;
}

S_API void ViewFrustum::CalculateCorners()
{
	if (!bCornersCalculated)
	{
		Mat44 mtxProjInv = mtxProj;
		SMatrixTranspose(&mtxProjInv);
		mtxProjInv = SMatrixInvert(mtxProjInv);

		// Starting at top left, then clockwise
		const Vec2f vcorners[] =
		{
			Vec2f(-1.0f, -1.0f),
			Vec2f(1.0f, -1.0f),
			Vec2f(1.0f, 1.0f),
			Vec2f(-1.0f, 1.0f),
		};

		Vec4f vnear, vfar;
		for (int i = 0; i < 4; ++i)
		{
			vnear = mtxProjInv * Vec4f(vcorners[i].x, vcorners[i].y, 0, 1.0f);
			vfar = mtxProjInv * Vec4f(vcorners[i].x, vcorners[i].y, 1.0f, 1.0f);

			vnear /= vnear.w;
			vfar /= vfar.w;

			corners[i] = vnear.xyz();
			corners[i + 4] = vfar.xyz();
		}

		bCornersCalculated = true;
	}
}

S_API void ViewFrustum::GetCorners(Vec3f _corners[8], bool viewSpace /*= false*/)
{
	CalculateCorners();

	if (!_corners)
		return;

	Mat44 mtxViewInv = Mat44::Identity;
	if (!viewSpace)
	{
		mtxViewInv = mtxView;
		SMatrixTranspose(&mtxViewInv);
		mtxViewInv = SMatrixInvert(mtxViewInv);
	}

	for (int i = 0; i < 8; ++i)
		_corners[i] = (mtxViewInv * Vec4f(corners[i].x, corners[i].y, corners[i].z, 1.0f)).xyz();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

S_API void SCamera::Turn(float yaw, float pitch)
{	
	Quat qYaw = Quat::FromRotationY(-yaw);
	Quat qPitch = Quat::FromAxisAngle(GetLeft(), pitch);
	Quat rotation = qYaw * qPitch;

	d_turnQuat = rotation;

	Vec3f left = (rotation * GetLeft()).Normalized();
	Vec3f up = (rotation * GetUp()).Normalized();
	Vec3f forward = (rotation * GetForward()).Normalized();

	viewMatrix = Mat44(
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

S_API Mat44& SCamera::RecalculateViewMatrix(bool roll/*=false*/)
{	
	viewMatrix._41 = -Vec3Dot(position, GetLeft());
	viewMatrix._42 = -Vec3Dot(position, GetUp());
	viewMatrix._43 = -Vec3Dot(position, GetForward());	
	//SPMatrixLookAtRH(&viewMatrix, position, position + Vec3f(0, 1.0f, 1.0f).Normalized(), Vec3f(0, 1.0f, 0));
	return viewMatrix;
}


SP_NMSPACE_END
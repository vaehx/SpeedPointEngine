//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2016 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "..\RenderLight.h"

SP_NMSPACE_BEG

S_API CRenderLight::CRenderLight()
{
	m_Params.radius = 1.0f;
	m_Params.type = eLIGHT_TYPE_OMNIDIRECTIONAL;
}

S_API AABB CRenderLight::GetAABB()
{
	// TODO
	return AABB(Vec3f(-FLT_MAX), Vec3f(FLT_MAX));
}

S_API void CRenderLight::SetCustomViewProjMatrix(const Mat44& viewMtx, const Mat44& projMtx)
{
}

SP_NMSPACE_END

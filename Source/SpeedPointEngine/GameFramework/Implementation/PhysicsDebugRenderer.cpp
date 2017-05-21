#include "PhysicsDebugRenderer.h"
#include <3DEngine\Implementation\C3DEngine.h>

SP_NMSPACE_BEG

S_API void PhysicsDebugRenderer::VisualizePoint(const Vec3f& p, const SColor& color, bool releaseAfterRender)
{
	CHelper* helper = C3DEngine::Get()->AddHelper<CPointHelper>(CPointHelper::Params(p), releaseAfterRender);
	helper->SetColor(color);
}

S_API void PhysicsDebugRenderer::VisualizeVector(const Vec3f& p, const Vec3f& v, const SColor& color, bool releaseAfterRender)
{
	CHelper* helper = C3DEngine::Get()->AddHelper<CVectorHelper>(CVectorHelper::Params(p, v.Normalized(), v.Length()), releaseAfterRender);
	helper->SetColor(color);
}

S_API void PhysicsDebugRenderer::VisualizePlane(const Vec3f& p, const Vec3f& n, const SColor& color, bool releaseAfterRender)
{
	CPlaneHelper::Params params;
	params.n = n;
	params.d = Vec3Dot(p, n);
	params.hsize = 3.0f;
	CHelper* helper = C3DEngine::Get()->AddHelper<CPlaneHelper>(params, releaseAfterRender);
	helper->SetColor(color);
}

S_API void PhysicsDebugRenderer::VisualizeAABB(const AABB& aabb, const SColor& color, bool releaseAfterRender)
{
	CBoxHelper::Params params;
	params.center = (aabb.vMin + aabb.vMax) * 0.5f;
	params.dimensions[0] = Vec3f((aabb.vMax.x - aabb.vMin.x) * 0.5f, 0, 0);
	params.dimensions[1] = Vec3f(0, (aabb.vMax.y - aabb.vMin.y) * 0.5f, 0);
	params.dimensions[2] = Vec3f(0, 0, (aabb.vMax.z - aabb.vMin.z) * 0.5f);
	C3DEngine::Get()->AddHelper<CBoxHelper>(params, SHelperRenderParams(color, true, true), releaseAfterRender);
}

S_API void PhysicsDebugRenderer::VisualizeBox(const OBB& obb, const SColor& color, bool releaseAfterRender)
{
	CBoxHelper::Params params;
	params.center = obb.center;
	for (int i = 0; i < 3; ++i)
		params.dimensions[i] = obb.directions[i] * obb.dimensions[i];

	C3DEngine::Get()->AddHelper<CBoxHelper>(params, SHelperRenderParams(color, true, true), releaseAfterRender);
}

SP_NMSPACE_END

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

SP_NMSPACE_END

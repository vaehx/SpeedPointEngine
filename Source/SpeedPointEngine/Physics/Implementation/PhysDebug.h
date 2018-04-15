#pragma once

#include "..\IPhysics.h"

SP_NMSPACE_BEG

class S_API PhysDebug
{
private:
	static IPhysicsDebugRenderer* spRenderer;

public:
	static void VisualizePoint(const Vec3f& p, const SColor& color = SColor::Turqouise(), bool releaseAfterRender = false)
	{
		if (spRenderer)
			spRenderer->VisualizePoint(p, color, releaseAfterRender);
	}

	static void VisualizeVector(const Vec3f& p, const Vec3f& v, const SColor& color = SColor::Turqouise(), bool releaseAfterRender = false)
	{
		if (spRenderer)
			spRenderer->VisualizeVector(p, v, color, releaseAfterRender);
	}

	static void VisualizeLine(const Vec3f& p1, const Vec3f& p2, const SColor& color = SColor::Turqouise(), bool releaseAfterRender = false)
	{
		if (spRenderer)
			spRenderer->VisualizeLine(p1, p2, color, releaseAfterRender);
	}

	static void VisualizeTriangleOutline(const Vec3f& p1, const Vec3f& p2, const Vec3f& p3, const SColor& color = SColor::Turqouise(), bool releaseAfterRender = false)
	{
		if (spRenderer)
		{
			spRenderer->VisualizeLine(p1, p2, color, releaseAfterRender);
			spRenderer->VisualizeLine(p2, p3, color, releaseAfterRender);
			spRenderer->VisualizeLine(p3, p1, color, releaseAfterRender);
		}
	}

	static void VisualizePlane(const Vec3f& p, const Vec3f& n, const SColor& color = SColor::Turqouise(), bool releaseAfterRender = false)
	{
		if (spRenderer)
			spRenderer->VisualizePlane(p, n, color, releaseAfterRender);
	}

	static void VisualizeAABB(const AABB& aabb, const SColor& color = SColor::Turqouise(), bool releaseAfterRender = false)
	{
		if (spRenderer)
			spRenderer->VisualizeAABB(aabb, color, releaseAfterRender);
	}

	static void VisualizeBox(const OBB& obb, const SColor& color = SColor::Turqouise(), bool releaseAfterRender = false)
	{
		if (spRenderer)
			spRenderer->VisualizeBox(obb, color, releaseAfterRender);
	}

	static IPhysDebugHelper* CreateHelper()
	{
		if (spRenderer)
			return spRenderer->CreateHelper();
		else
			return 0;
	}

	static void SetRenderer(IPhysicsDebugRenderer* pRenderer)
	{
		spRenderer = pRenderer;
	}
};

SP_NMSPACE_END

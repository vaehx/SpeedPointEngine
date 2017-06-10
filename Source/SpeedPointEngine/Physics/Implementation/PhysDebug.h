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

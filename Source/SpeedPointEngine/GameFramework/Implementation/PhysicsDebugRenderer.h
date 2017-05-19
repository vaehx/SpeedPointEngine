#pragma once

#include <Common\SPrerequisites.h>
#include <Physics\IPhysics.h>

SP_NMSPACE_BEG

class S_API PhysicsDebugRenderer : public IPhysicsDebugRenderer
{
public:
	virtual void VisualizePoint(const Vec3f& p, const SColor& color = SColor::Turqouise(), bool releaseAfterRender = false);
	virtual void VisualizeVector(const Vec3f& p, const Vec3f& v, const SColor& color = SColor::Turqouise(), bool releaseAfterRender = false);
	virtual void VisualizePlane(const Vec3f& p, const Vec3f& n, const SColor& color = SColor::Turqouise(), bool releaseAfterRender = false);
};

SP_NMSPACE_END

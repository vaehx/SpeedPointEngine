#pragma once

#include <Common\SPrerequisites.h>
#include <Common\geo.h>
#include <3DEngine\CHelper.h>
#include <Physics\IPhysics.h>

SP_NMSPACE_BEG

class S_API CPhysDebugHelper : public IPhysDebugHelper
{
private:
	CHelper* m_pHelper;
	geo::EShapeType m_ShapeType;

public:
	CPhysDebugHelper();
	virtual ~CPhysDebugHelper() { Clear(); }
	virtual void CreateFromShape(const geo::shape* pshape, const SColor& color = SColor::White());
	virtual void UpdateFromShape(const geo::shape* pshape, const AABB& bounds = AABB());
	virtual void Show(bool show = true);
	virtual bool IsShown() const;
	virtual void Clear();
};

class S_API PhysicsDebugRenderer : public IPhysicsDebugRenderer
{
public:
	virtual void VisualizePoint(const Vec3f& p, const SColor& color = SColor::Turqouise(), bool releaseAfterRender = false);
	virtual void VisualizeVector(const Vec3f& p, const Vec3f& v, const SColor& color = SColor::Turqouise(), bool releaseAfterRender = false);
	virtual void VisualizePlane(const Vec3f& p, const Vec3f& n, const SColor& color = SColor::Turqouise(), bool releaseAfterRender = false);
	virtual void VisualizeAABB(const AABB& aabb, const SColor& color = SColor::Turqouise(), bool releaseAfterRender = false);
	virtual void VisualizeBox(const OBB& obb, const SColor& color = SColor::Turqouise(), bool releaseAfterRender = false);

	virtual IPhysDebugHelper* CreateHelper() const { return new CPhysDebugHelper(); }
};

SP_NMSPACE_END

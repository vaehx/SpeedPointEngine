#pragma once

#include "Vector3.h"
#include "SColor.h"
#include "BoundBox.h"

#define GEO_NMSPACE_BEG namespace SpeedPoint { namespace geo {
#define GEO_NMSPACE_END }}

GEO_NMSPACE_BEG

// n/d
struct quotient
{
	float n; // numerator
	float d; // denominator
	quotient() {}
	quotient(float numerator, float denominator) : n(numerator), d(denominator) {}
	void set(float numerator, float denominator) { n = numerator; d = denominator; }
	float val() { return (d) ? n / d : n; }
};

// --------------------------------------------------------------------------------------------------------------------

enum EShapeType
{
	eSHAPE_UNKNOWN = 0,
	eSHAPE_RAY,
	eSHAPE_PLANE,
	eSHAPE_SPHERE,
	eSHAPE_CYLINDER,
	eSHAPE_CAPSULE,
	eSHAPE_BOX,
	eSHAPE_TRIANGLE,
	eSHAPE_CIRCLE, // 3d circle
	eSHAPE_MESH,

	NUM_SHAPE_TYPES
};

const char* GetShapeTypeName(EShapeType type);

struct shape
{
protected:
	EShapeType ty;
public:
	EShapeType GetType() const { return ty; };
	virtual AABB GetBoundBoxAxisAligned() const { return AABB(); }
	virtual OBB GetBoundBox() const { return OBB(); }
	virtual float GetVolume() const = 0;
	virtual float GetDistance(const Vec3f& p) const = 0;
	virtual shape* Clone() const { return 0; }
	// Does not copy if the other shape is not of the same type
	virtual void CopyTo(shape* pother) const {}
	virtual void Transform(const Mat44& mtx) {};
};

struct ray : shape
{
	Vec3f p;
	Vec3f v;
	ray() { ty = eSHAPE_RAY; }
	ray(const Vec3f& _p, const Vec3f& _v) : p(_p), v(_v) { ty = eSHAPE_RAY; }
	virtual float GetVolume() const;
	virtual float GetDistance(const Vec3f& p) const;
	virtual shape* Clone() const;
	virtual void CopyTo(shape* pother) const;
	virtual void Transform(const Mat44& mtx);
};

struct plane : shape
{
	Vec3f n;
	float d;
	plane() { ty = eSHAPE_PLANE; }
	plane(const Vec3f& normal, float _d) : n(normal), d(_d) { ty = eSHAPE_PLANE; }
	plane(const Vec3f& normal, const Vec3f& p)
	{
		ty = eSHAPE_PLANE;
		n = normal;
		d = Vec3Dot(p, n);
	}
	// Creates a plane from the three points: n = normalize((p2 - p1) x (p3 - p1)), d = dot(p1, n)
	plane(const Vec3f& p1, const Vec3f& p2, const Vec3f& p3)
	{
		ty = eSHAPE_PLANE;
		n = Vec3Normalize((p2 - p1) ^ (p3 - p1));
		d = Vec3Dot(p1, n);
	}
	virtual OBB GetBoundBox() const;
	virtual float GetVolume() const;
	// signed distance (if negative, point is "behind" plane)
	virtual float GetDistance(const Vec3f& p) const;
	virtual shape* Clone() const;
	virtual void CopyTo(shape* pother) const;
	virtual void Transform(const Mat44& mtx);
};

struct circle : shape
{
	Vec3f c;
	Vec3f n;
	float r;
	circle() { ty = eSHAPE_CIRCLE; }
	virtual float GetVolume() const;
};

struct sphere : shape
{
	Vec3f c;
	float r;
	sphere() { ty = eSHAPE_SPHERE; }
	sphere(const Vec3f& center, float radius) : c(center), r(radius) { ty = eSHAPE_SPHERE; }
	virtual AABB GetBoundBoxAxisAligned() const;
	virtual OBB GetBoundBox() const;
	virtual float GetVolume() const;
	// signed distance. If negative, the point lies inside the sphere
	virtual float GetDistance(const Vec3f& p) const;
	virtual shape* Clone() const;
	virtual void CopyTo(shape* pother) const;
	virtual void Transform(const Mat44& mtx);
};

struct cylinder : shape
{
	Vec3f p[2];
	float r;
	cylinder() { ty = eSHAPE_CYLINDER; }
	cylinder(const Vec3f& bottom, const Vec3f& top, float radius)
	{
		ty = eSHAPE_CYLINDER;
		p[0] = bottom;
		p[1] = top;
		r = radius;
	}
	cylinder(const Vec3f& center, const Vec3f& axis, float halfheight, float radius)
	{
		ty = eSHAPE_CYLINDER;
		p[0] = center - axis * halfheight;
		p[1] = center + axis * halfheight;
		r = radius;
	}
	virtual AABB GetBoundBoxAxisAligned() const;
	virtual OBB GetBoundBox() const;
	virtual float GetVolume() const;
	// signed distance. if negative, the point lies inside the cylinder
	virtual float GetDistance(const Vec3f& p) const;
	virtual shape* Clone() const;
	virtual void CopyTo(shape* pother) const;
	virtual void Transform(const Mat44& mtx);
};

struct capsule : shape
{
	Vec3f c;
	Vec3f axis; // normalized!
	float hh;
	float r;
	capsule() { ty = eSHAPE_CAPSULE; }
	capsule(const Vec3f& bottom, const Vec3f& top, float radius)
	{
		ty = eSHAPE_CAPSULE;
		c = (bottom + top) * 0.5f;
		hh = (top - bottom).Length();
		axis = (top - bottom) / hh;
		hh *= 0.5f;
		r = radius;
	}
	capsule(const Vec3f& center, const Vec3f& naxis, float halfheight, float radius)
		: c(center), hh(halfheight), r(radius), axis(naxis)
	{
		ty = eSHAPE_CAPSULE;
	}
	virtual AABB GetBoundBoxAxisAligned() const;
	virtual OBB GetBoundBox() const;
	virtual float GetVolume() const;
	virtual float GetDistance(const Vec3f& p) const;
	virtual shape* Clone() const;
	virtual void CopyTo(shape* pother) const;
	virtual void Transform(const Mat44& mtx);
};

struct triangle : shape
{
	Vec3f p[3];
	Vec3f n;

	triangle() { ty = eSHAPE_TRIANGLE; }
	triangle(const Vec3f& p1, const Vec3f& p2, const Vec3f& p3, const Vec3f& _n)
		: triangle()
	{
		p[0] = p1; p[1] = p2; p[2] = p3;
		n = _n;
	}
	triangle(const Vec3f& p1, const Vec3f& p2, const Vec3f& p3)
		: triangle(p1, p2, p3, Vec3f(0))
	{
		n = (p[1] - p[0]) ^ (p[2] - p[0]);
	}
	virtual AABB GetBoundBoxAxisAligned() const;
	virtual float GetVolume() const;
	virtual float GetDistance(const Vec3f& p) const;
	virtual void CopyTo(shape* pother) const;
};

struct box : shape
{
	Vec3f c;
	Vec3f axis[3];
	float dim[3]; // half-dimensions

	box() { ty = eSHAPE_BOX; }
	box(const OBB& obb)
	{
		ty = eSHAPE_BOX;
		c = obb.center;
		for (int i = 0; i < 3; ++i)
		{
			axis[i] = obb.directions[i];
			dim[i] = obb.dimensions[i];
		}
	}
	virtual AABB GetBoundBoxAxisAligned() const;
	virtual OBB GetBoundBox() const;
	virtual float GetVolume() const;
	virtual float GetDistance(const Vec3f& p) const;
	virtual shape* Clone() const;
	virtual void CopyTo(shape* pother) const;
	virtual void Transform(const Mat44& mtx);
};

struct mesh_tree_node
{
	AABB aabb;
	unsigned int* tris; // for each tri: start index in indices array of mesh shape
	unsigned int num_tris;
	mesh_tree_node* children; // if 0, this node is a leaf
	unsigned int num_children;
	SColor _color;

	mesh_tree_node() 
		: tris(0), num_tris(0), children(0), num_children(0) {}
};

struct mesh : shape
{
	Vec3f* points;
	unsigned int num_points;
	unsigned int* indices; // triangles!
	unsigned int num_indices; // must be a multiple of 3
	mesh_tree_node root; // contains the whole mesh
	Mat44 transform;

	mesh() : points(0), indices(0) { ty = eSHAPE_MESH; }
	~mesh()
	{
		if (points) delete[] points; points = 0;
		if (indices) delete[] indices; indices = 0;
	}

	virtual AABB GetBoundBoxAxisAligned() const;
	virtual OBB GetBoundBox() const;
	virtual float GetVolume() const;
	virtual float GetDistance(const Vec3f& p) const;
	void CreateTree(bool octree = true, unsigned int maxTrisPerLeaf = 8);
	void ClearTree();
};

// --------------------------------------------------------------------------------------------------------------------

enum EIntersectionFeature
{
	eINTERSECTION_FEATURE_BASE_SHAPE,
	eINTERSECTION_FEATURE_CAP
};

struct SIntersection
{
	Vec3f p;
	Vec3f n;
	float dist; // interpenetration distance. negative if interpenetrating
	EIntersectionFeature feature;
};

bool _RayPlane(const ray* pray, const plane* pplane, SIntersection* pinters);
bool _RaySphere(const ray* pray, const sphere* psphere, SIntersection* pinters);
bool _RayCylinder(const ray* pray, const cylinder* pcyl, SIntersection* pinters);
bool _RayCapsule(const ray* pray, const capsule* pcapsule, SIntersection* pinters);
bool _RayBox(const ray* pray, const box* pbox, SIntersection* pinters);
bool _RayTriangle(const ray* pray, const triangle* ptri, SIntersection* pinters);

bool _PlaneRay(const plane* pplane, const ray* pray, SIntersection* pinters);
bool _PlanePlane(const plane* pplane1, const plane* pplane2, SIntersection* pinters);
bool _PlaneSphere(const plane* pplane, const sphere* psphere, SIntersection* pinters);
bool _PlaneCylinder(const plane* pplane, const cylinder* pcyl, SIntersection* pinters);
bool _PlaneCapsule(const plane* pplane, const capsule* pcapsule, SIntersection* pinters);
bool _PlaneBox(const plane* pplane, const box* pbox, SIntersection* pinters);
bool _PlaneTriangle(const plane* pplane, const triangle* ptri, SIntersection* pinters);

bool _SphereRay(const sphere* psphere, const ray* pray, SIntersection* pinters);
bool _SpherePlane(const sphere* psphere, const plane* pplane, SIntersection* pinters);
bool _SphereSphere(const sphere* psphere1, const sphere* psphere2, SIntersection* pinters);
bool _SphereCylinder(const sphere* psphere, const cylinder* pcyl, SIntersection* pinters);
bool _SphereCapsule(const sphere* psphere, const capsule* pcapsule, SIntersection* pinters);
bool _SphereBox(const sphere* psphere, const box* pbox, SIntersection* pinters);
bool _SphereTriangle(const sphere* psphere, const triangle* ptri, SIntersection* pinters);

bool _CylinderRay(const cylinder* pcyl, const ray* pray, SIntersection* pinters);
bool _CylinderSphere(const cylinder* pcyl, const sphere* psphere, SIntersection* pinters);
bool _CylinderCylinder(const cylinder* pcyl1, const cylinder* pcyl2, SIntersection* pinters);

bool _CapsuleRay(const capsule* pcapsule, const ray* pray, SIntersection* pinters);
bool _CapsuleSphere(const capsule* pcapsule, const sphere* psphere, SIntersection* pinters);
bool _CapsulePlane(const capsule* pcapsule, const plane* pplane, SIntersection* pinters);
bool _CapsuleCapsule(const capsule* pcapsule1, const capsule* pcapsule2, SIntersection* pinters);
bool _CapsuleBox(const capsule* pcapsule, const box* pbox, SIntersection* pinters);
bool _CapsuleTriangle(const capsule* pcapsule, const triangle* ptri, SIntersection* pinters);

bool _TriangleRay(const triangle* ptri, const ray* pray, SIntersection* pinters);
bool _TrianglePlane(const triangle* ptri, const plane* pplane, SIntersection* pinters);
bool _TriangleSphere(const triangle* ptri, const sphere* psphere, SIntersection* pinters);
bool _TriangleCapsule(const triangle* ptri, const capsule* pcapsule, SIntersection* pinters);

bool _PointIsInsideTriangle(const triangle* ptri, const Vec3f& P);

bool _CircleCircle(const circle* pcircle1, const circle* pcircle2, SIntersection* pinters);

bool _BoxBox(const box* pbox1, const box* pbox2, SIntersection* pinters);
bool _BoxRay(const box* pbox, const ray* pray, SIntersection* pinters);
bool _BoxPlane(const box* pbox, const plane* pplane, SIntersection* pinters);
bool _BoxSphere(const box* pbox, const sphere* psphere, SIntersection* pinters);
bool _BoxCapsule(const box* pbox, const capsule* pcapsule, SIntersection* pinters);

bool _MeshShape(const mesh* pmesh, const shape* pshape, SIntersection* pinters);
bool _ShapeMesh(const shape* pshape, const mesh* pmesh, SIntersection* pinters);

typedef bool (*_IntersectionTestFnPtr)(const shape* pshape1, const shape* pshape2, SIntersection* pinters);
static _IntersectionTestFnPtr _intersectionTestTable[NUM_SHAPE_TYPES][NUM_SHAPE_TYPES];
void FillIntersectionTestTable();

bool _Intersection(const shape* pshape1, const shape* pshape2, SIntersection* pinters = 0);

GEO_NMSPACE_END

#include "geo.h"
#include "ChunkedObjectPool.h"
#include <Physics\Implementation\PhysDebug.h> // TODO: Get this out of here.
#include <cstdlib>
#include <time.h>
#include <stack>

GEO_NMSPACE_BEG

#ifdef _DEBUG
#define _d(msg, ...) CLog::Log(S_DEBUG, msg, __VA_ARGS__)
#else
#define _d(msg)
#endif

// Returns the sign bit of x (x < 0)
ILINE int isneg(f32 x)
{
	union
	{
		f32    f;
		unsigned int i;
	} u;
	u.f = x;
	return (int)(u.i >> 31);
}

// Returns (x < 0) ? -1 : 1
ILINE __int32 sgnnz(f32 x)
{
	union
	{
		f32   f;
		__int32 i;
	} u;
	u.f = x;
	return ((u.i >> 31) << 1) + 1;
}

#define ONE_THIRD 0.333333333333f

inline float sqr(float f) { return f * f; }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* GetShapeTypeName(EShapeType t)
{
	switch (t)
	{
	case eSHAPE_BOX: return "SHAPE_BOX";
	case eSHAPE_CAPSULE: return "SHAPE_CAPSULE";
	case eSHAPE_CIRCLE: return "SHAPE_CIRCLE";
	case eSHAPE_CYLINDER: return "SHAPE_CYLINDER";
	case eSHAPE_MESH: return "SHAPE_MESH";
	case eSHAPE_PLANE: return "SHAPE_PLANE";
	case eSHAPE_RAY: return "SHAPE_RAY";
	case eSHAPE_SPHERE: return "SHAPE_SPHERE";
	case eSHAPE_TRIANGLE: return "SHAPE_TRIANGLE";
	case eSHAPE_UNKNOWN: return "SHAPE_UNKNOWN";
	default:
		return "???";
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	General Intersection Test
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FillIntersectionTestTable()
{
	for (int i = 0; i < NUM_SHAPE_TYPES; ++i)
		for (int j = 0; j < NUM_SHAPE_TYPES; ++j)
			_intersectionTestTable[i][j] = 0;

	_intersectionTestTable[eSHAPE_RAY][eSHAPE_RAY] = 0;
	_intersectionTestTable[eSHAPE_RAY][eSHAPE_PLANE] = (_IntersectionTestFnPtr)&_RayPlane;
	_intersectionTestTable[eSHAPE_RAY][eSHAPE_SPHERE] = (_IntersectionTestFnPtr)&_RaySphere;
	_intersectionTestTable[eSHAPE_RAY][eSHAPE_CYLINDER] = (_IntersectionTestFnPtr)&_RayCylinder;
	_intersectionTestTable[eSHAPE_RAY][eSHAPE_CAPSULE] = (_IntersectionTestFnPtr)&_RayCapsule;
	_intersectionTestTable[eSHAPE_RAY][eSHAPE_TRIANGLE] = (_IntersectionTestFnPtr)&_RayTriangle;
	_intersectionTestTable[eSHAPE_RAY][eSHAPE_MESH] = (_IntersectionTestFnPtr)&_ShapeMesh;
	_intersectionTestTable[eSHAPE_RAY][eSHAPE_BOX] = (_IntersectionTestFnPtr)&_RayBox;

	_intersectionTestTable[eSHAPE_PLANE][eSHAPE_RAY] = (_IntersectionTestFnPtr)&_PlaneRay;
	_intersectionTestTable[eSHAPE_PLANE][eSHAPE_PLANE] = (_IntersectionTestFnPtr)&_PlanePlane;
	_intersectionTestTable[eSHAPE_PLANE][eSHAPE_SPHERE] = (_IntersectionTestFnPtr)&_PlaneSphere;
	_intersectionTestTable[eSHAPE_PLANE][eSHAPE_CYLINDER] = 0;
	_intersectionTestTable[eSHAPE_PLANE][eSHAPE_CAPSULE] = (_IntersectionTestFnPtr)&_PlaneCapsule;
	_intersectionTestTable[eSHAPE_PLANE][eSHAPE_TRIANGLE] = (_IntersectionTestFnPtr)&_PlaneTriangle;
	_intersectionTestTable[eSHAPE_PLANE][eSHAPE_MESH] = (_IntersectionTestFnPtr)&_ShapeMesh;
	_intersectionTestTable[eSHAPE_PLANE][eSHAPE_BOX] = (_IntersectionTestFnPtr)&_PlaneBox;

	_intersectionTestTable[eSHAPE_SPHERE][eSHAPE_RAY] = (_IntersectionTestFnPtr)&_SphereRay;
	_intersectionTestTable[eSHAPE_SPHERE][eSHAPE_PLANE] = (_IntersectionTestFnPtr)&_SpherePlane;
	_intersectionTestTable[eSHAPE_SPHERE][eSHAPE_SPHERE] = (_IntersectionTestFnPtr)&_SphereSphere;
	_intersectionTestTable[eSHAPE_SPHERE][eSHAPE_CYLINDER] = (_IntersectionTestFnPtr)&_SphereCylinder;
	_intersectionTestTable[eSHAPE_SPHERE][eSHAPE_CAPSULE] = (_IntersectionTestFnPtr)&_SphereCapsule;
	_intersectionTestTable[eSHAPE_SPHERE][eSHAPE_TRIANGLE] = (_IntersectionTestFnPtr)&_SphereTriangle;
	_intersectionTestTable[eSHAPE_SPHERE][eSHAPE_MESH] = (_IntersectionTestFnPtr)&_ShapeMesh;
	_intersectionTestTable[eSHAPE_SPHERE][eSHAPE_BOX] = (_IntersectionTestFnPtr)&_SphereBox;

	_intersectionTestTable[eSHAPE_CYLINDER][eSHAPE_RAY] = (_IntersectionTestFnPtr)&_CylinderRay;
	_intersectionTestTable[eSHAPE_CYLINDER][eSHAPE_PLANE] = 0;
	_intersectionTestTable[eSHAPE_CYLINDER][eSHAPE_SPHERE] = (_IntersectionTestFnPtr)&_CylinderSphere;
	_intersectionTestTable[eSHAPE_CYLINDER][eSHAPE_CYLINDER] = (_IntersectionTestFnPtr)&_CylinderCylinder;
	_intersectionTestTable[eSHAPE_CYLINDER][eSHAPE_CAPSULE] = 0;
	_intersectionTestTable[eSHAPE_CYLINDER][eSHAPE_TRIANGLE] = 0;
	_intersectionTestTable[eSHAPE_CYLINDER][eSHAPE_MESH] = (_IntersectionTestFnPtr)&_ShapeMesh;

	_intersectionTestTable[eSHAPE_CAPSULE][eSHAPE_RAY] = (_IntersectionTestFnPtr)&_CapsuleRay;
	_intersectionTestTable[eSHAPE_CAPSULE][eSHAPE_PLANE] = (_IntersectionTestFnPtr)&_CapsulePlane;
	_intersectionTestTable[eSHAPE_CAPSULE][eSHAPE_SPHERE] = (_IntersectionTestFnPtr)&_CapsuleSphere;
	_intersectionTestTable[eSHAPE_CAPSULE][eSHAPE_CYLINDER] = 0;
	_intersectionTestTable[eSHAPE_CAPSULE][eSHAPE_CAPSULE] = (_IntersectionTestFnPtr)&_CapsuleCapsule;
	_intersectionTestTable[eSHAPE_CAPSULE][eSHAPE_BOX] = (_IntersectionTestFnPtr)&_CapsuleBox;
	_intersectionTestTable[eSHAPE_CAPSULE][eSHAPE_TRIANGLE] = (_IntersectionTestFnPtr)&_CapsuleTriangle;
	_intersectionTestTable[eSHAPE_CAPSULE][eSHAPE_MESH] = (_IntersectionTestFnPtr)&_ShapeMesh;

	_intersectionTestTable[eSHAPE_BOX][eSHAPE_BOX] = (_IntersectionTestFnPtr)&_BoxBox;
	_intersectionTestTable[eSHAPE_BOX][eSHAPE_RAY] = (_IntersectionTestFnPtr)&_BoxRay;
	_intersectionTestTable[eSHAPE_BOX][eSHAPE_PLANE] = (_IntersectionTestFnPtr)&_BoxPlane;
	_intersectionTestTable[eSHAPE_BOX][eSHAPE_SPHERE] = (_IntersectionTestFnPtr)&_BoxSphere;
	_intersectionTestTable[eSHAPE_BOX][eSHAPE_CAPSULE] = (_IntersectionTestFnPtr)&_BoxCapsule;

	_intersectionTestTable[eSHAPE_TRIANGLE][eSHAPE_RAY] = (_IntersectionTestFnPtr)&_TriangleRay;
	_intersectionTestTable[eSHAPE_TRIANGLE][eSHAPE_PLANE] = (_IntersectionTestFnPtr)&_TrianglePlane;
	_intersectionTestTable[eSHAPE_TRIANGLE][eSHAPE_SPHERE] = (_IntersectionTestFnPtr)&_TriangleSphere;
	_intersectionTestTable[eSHAPE_TRIANGLE][eSHAPE_CYLINDER] = 0;
	_intersectionTestTable[eSHAPE_TRIANGLE][eSHAPE_CAPSULE] = (_IntersectionTestFnPtr)&_TriangleCapsule;
	_intersectionTestTable[eSHAPE_TRIANGLE][eSHAPE_TRIANGLE] = 0;

	_intersectionTestTable[eSHAPE_MESH][eSHAPE_RAY] = (_IntersectionTestFnPtr)&_MeshShape;
	_intersectionTestTable[eSHAPE_MESH][eSHAPE_PLANE] = (_IntersectionTestFnPtr)&_MeshShape;
	_intersectionTestTable[eSHAPE_MESH][eSHAPE_SPHERE] = (_IntersectionTestFnPtr)&_MeshShape;
	_intersectionTestTable[eSHAPE_MESH][eSHAPE_CYLINDER] = (_IntersectionTestFnPtr)&_MeshShape;
	_intersectionTestTable[eSHAPE_MESH][eSHAPE_CAPSULE] = (_IntersectionTestFnPtr)&_MeshShape;
	_intersectionTestTable[eSHAPE_MESH][eSHAPE_TRIANGLE] = 0;
	_intersectionTestTable[eSHAPE_MESH][eSHAPE_MESH] = 0;

	_intersectionTestTable[eSHAPE_CIRCLE][eSHAPE_CIRCLE] = (_IntersectionTestFnPtr)&_CircleCircle;
}

bool _Intersection(const shape* pshape1, const shape* pshape2, SIntersection* pinters)
{
	_IntersectionTestFnPtr fn = _intersectionTestTable[pshape1->GetType()][pshape2->GetType()];
	if (!fn)
		return false;

	return fn(pshape1, pshape2, pinters);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Ray
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float ray::GetVolume() const
{
	return 0.0f;
}

float ray::GetDistance(const Vec3f& q) const
{
	Vec3f vn = Vec3Normalize(v);
	return (q - (p + Vec3Dot(q - p, vn) * vn)).Length();
}

shape* ray::Clone() const
{
	ray* pray = new ray();
	pray->p = p;
	pray->v = v;
	return pray;
}

void ray::Transform(const Mat44& mtx)
{
	Vec3f p2 = p + v;
	p = (mtx * Vec4f(p, 1.0f)).xyz();
	v = (mtx * Vec4f(p2, 1.0f)).xyz() - p;
}

bool _RayPlane(const ray* pray, const plane* pplane, SIntersection* pinters)
{
	quotient t(Vec3Dot(pplane->n * pplane->d - pray->p, pplane->n), Vec3Dot(pray->v, pplane->n));

	if (fabsf(t.d) < FLT_EPSILON && fabsf(t.n) >= FLT_EPSILON)
		return false;

	pinters->p = pray->p + pray->v * t.val();
	pinters->n = pplane->n;
	pinters->dist = 0.0f;
	pinters->feature = eINTERSECTION_FEATURE_BASE_SHAPE;
	return true;
}

bool _RaySphere(const ray* pray, const sphere* psphere, SIntersection* pinters)
{
	Vec3f dp = pray->p - psphere->c;
	float A = pray->v.LengthSq();
	float B = 2.0f * (dp | pray->v);
	float C = dp.LengthSq() - psphere->r * psphere->r;

	float discr = B * B - 4.0f * A * C;
	if (discr < 0)
		return false;

	float s = sqrtf(discr);
	float invden = 1.0f / (2.0f * A);
	float mint = min((-B + s) * invden, (-B - s) * invden);

	pinters->p = pray->p + mint * pray->v;
	pinters->n = Vec3Normalize(pinters->p - psphere->c);
	pinters->dist = 0.0f;
	pinters->feature = eINTERSECTION_FEATURE_BASE_SHAPE;
	return true;
}

bool _RayCylinder(const ray* pray, const cylinder* pcyl, SIntersection* pinters)
{
	const Vec3f vr = pray->v.Normalized(); // normalized ray direction
	const Vec3f vc = (pcyl->p[1] - pcyl->p[0]).Normalized(); // normalized cylinder ray direction
	const float rsq = pcyl->r * pcyl->r; // r^2

	float t[] = { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX }; // normalized ray params
	bool b[] = { false, false, false, false }; // t[i] is a valid intersection
	int n = 0;

	// Mantle
	// Find solutions for At^2 + Bt + C = 0 with ...
	const Vec3f U = vr - Vec3Dot(vr, vc) * vc;
	const Vec3f V = (pray->p - pcyl->p[0]) - Vec3Dot(pray->p - pcyl->p[0], vc) * vc;
	const float A = U.LengthSq();
	const float B = 2.0f * Vec3Dot(U, V);
	const float C = V.LengthSq() - rsq;
	const float s = B * B - 4.0f * A * C;
	const float den = 2.0f * A;
	if (s >= 0 && fabsf(den) > 0.0001f)
	{
		const float sqrt_s = sqrtf(s);
		const float invden = 1.0f / den;
		t[0] = (-B + sqrt_s) * invden;
		t[1] = (-B - sqrt_s) * invden;

		// Check if intersections are between caps
		Vec3f q;
		for (int i = 0; i < 2; ++i)
		{
			q = pray->p + vr * t[i];
			b[i] = (Vec3Dot(vc, q - pcyl->p[0]) > 0 && Vec3Dot(vc, q - pcyl->p[1]) < 0);
		}

		n += 2;
	}

	// Caps
	if (!b[0] || !b[1])
	{
		float param, den;
		Vec3f capn;
		for (int i = 0; i < 2; ++i)
		{
			capn = vc * (2.0f * (float)i - 1.0f);
			den = pray->v | capn;
			if (fabsf(den) >= FLT_EPSILON)
			{
				param = ((pcyl->p[i] - pray->p) | capn) / den;
				if (((pray->p + pray->v * param) - pcyl->p[i]).LengthSq() <= rsq)
				{
					t[i + 2] = param;
					b[i + 2] = true;
				}
			}
		}

		n += 2;
	}

	// Determine minimum intersection
	int mint = n;
	for (int i = 0; i < n; ++i)
	{
		if (b[i])
		{
			if (mint == n || t[i] < t[mint])
				mint = i;
		}
	}

	if (mint == n)
		return false;

	pinters->p = pray->p + vr * t[mint];
	pinters->dist = 0.0f;
	if (mint < 2)
	{
		pinters->n = Vec3Normalize(pinters->p - (pcyl->p[0] + ((pinters->p - pcyl->p[0]) | vc) * vc));
		pinters->feature = eINTERSECTION_FEATURE_BASE_SHAPE;
	}
	else
	{
		pinters->n = vc * (2.0f * (float)(mint - 2) - 1.0f);
		pinters->feature = eINTERSECTION_FEATURE_CAP;
	}

	return true;
}

bool _RayCapsule(const ray* pray, const capsule* pcapsule, SIntersection* pinters)
{
	const Vec3f vr = pray->v.Normalized(); // normalized ray direction
	const Vec3f vc = pcapsule->axis; // normalized cylinder ray direction
	const float rsq = pcapsule->r * pcapsule->r; // r^2

	// Mantle
	float t[2];
	const Vec3f U = vr - Vec3Dot(vr, vc) * vc;
	const Vec3f V = (pray->p - pcapsule->c) - Vec3Dot(pray->p - pcapsule->c, vc) * vc;
	const float A = U.LengthSq();
	const float B = 2.0f * Vec3Dot(U, V);
	const float C = V.LengthSq() - rsq;
	const float s = B * B - 4.0f * A * C;
	const float den = 2.0f * A;
	if (s >= 0 && fabsf(den) > 0.0001f)
	{
		const float sqrt_s = sqrtf(s);
		const float invden = 1.0f / den;
		t[0] = (-B + sqrt_s) * invden;
		t[1] = (-B - sqrt_s) * invden;

		Vec3f q, n;
		float s;
		for (int i = 0; i < 2; ++i)
		{
			q = pray->p + vr * t[i];
			s = Vec3Dot(q - pcapsule->c, vc);
			n = q - (pcapsule->c + s * vc);
			if (fabsf(s) <= pcapsule->hh && Vec3Dot(n, vr) <= 0)
			{
				pinters->feature = eINTERSECTION_FEATURE_BASE_SHAPE;
				pinters->dist = 0.0f;
				pinters->p = q;
				pinters->n = n;
				return true;
			}
		}
	}

	// Caps
	Vec3f u, v, capn, capp;
	for (int i = 0; i < 2; ++i)
	{
		capn = vc * (2.0f * (float)i - 1.0f);
		capp = pcapsule->c + (-1.0f + 2.0f * i) * pcapsule->hh * pcapsule->axis;
		u = (pray->p + Vec3Dot(capp - pray->p, vr) * vr) - capp;
		float ulnsq = u.LengthSq();
		if (ulnsq <= rsq && Vec3Dot(capn, vr) <= 0)
		{
			v = -vr * sqrtf(rsq - ulnsq);
			pinters->feature = eINTERSECTION_FEATURE_CAP;
			pinters->dist = 0.0f;
			pinters->p = capp + u + v;
			pinters->n = (u + v).Normalized();
			return true;
		}
	}

	return false;
}

bool _RayBox(const ray* pray, const box* pbox, SIntersection* pinters)
{
	int inters = 0;
	Vec3f p, n;
	quotient t;
	for (int i = 0; i < 3; ++i)
	{
		n = pbox->axis[i] * sgnnz(-Vec3Dot(pray->v, pbox->axis[i]));
		p = pbox->c + n * pbox->dim[i];

		t.set(Vec3Dot(p - pray->p, n), Vec3Dot(pray->v, n));
		if (fabsf(t.d) >= FLT_EPSILON)
		{
			pinters->p = pray->p + t.val() * pray->v;
			pinters->n = n;
			inters |= (isneg(fabsf(Vec3Dot(pinters->p - p, pbox->axis[(i + 1) % 3])) - pbox->dim[(i + 1) % 3])
				& isneg(fabsf(Vec3Dot(pinters->p - p, pbox->axis[(i + 2) % 3])) - pbox->dim[(i + 2) % 3]));
		}
	}

	pinters->dist = 0.0f;
	pinters->feature = eINTERSECTION_FEATURE_BASE_SHAPE;
	return inters;
}

bool _RayTriangle(const ray* pray, const triangle* ptri, SIntersection* pinters)
{
	Vec3f v[3], n, q;
	v[0] = ptri->p[1] - ptri->p[0];
	v[1] = ptri->p[2] - ptri->p[0];

	n = v[0] ^ v[1];
	quotient t(Vec3Dot(ptri->p[0] - pray->p, n), Vec3Dot(pray->v, n));
	if (fabsf(t.d) < FLT_EPSILON && fabsf(t.n) >= FLT_EPSILON)
		return false;

	q = pray->p + t.val() * pray->v;
	v[2] = q - ptri->p[0];

	const float dot00 = Vec3Dot(v[0], v[0]),
		dot01 = Vec3Dot(v[0], v[1]),
		dot02 = Vec3Dot(v[0], v[2]),
		dot11 = Vec3Dot(v[1], v[1]),
		dot12 = Vec3Dot(v[1], v[2]);

	const float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
	const float U = (dot11 * dot02 - dot01 * dot12) * invDenom;
	const float V = (dot00 * dot12 - dot01 * dot02) * invDenom;
	if (U < 0 || V < 0 || U + V >= 1.0f)
		return false;

	pinters->p = q;
	pinters->n = n;
	pinters->feature = eINTERSECTION_FEATURE_BASE_SHAPE;
	pinters->dist = 0.0f;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Plane
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OBB plane::GetBoundBox() const
{
	OBB obb;
	obb.center = n * d;
	obb.directions[1] = n;
	obb.directions[0] = n.GetOrthogonal().Normalized();
	obb.directions[2] = obb.directions[1] ^ obb.directions[0];
	obb.dimensions[0] = obb.dimensions[2] = FLT_MAX;
	obb.dimensions[1] = 1.0f;
	return obb;
}

float plane::GetVolume() const
{
	return 0.0f;
}

float plane::GetDistance(const Vec3f& q) const
{
	return Vec3Dot(q - n * d, n);
}

shape* plane::Clone() const
{
	plane* pplane = new plane();
	pplane->d = d;
	pplane->n = n;
	return pplane;
}

void plane::Transform(const Mat44& mtx)
{
	Vec3f p[3];
	p[0] = n * d;
	p[1] = p[0] + n.GetOrthogonal();
	p[2] = p[0] + (n ^ (p[1] - p[0]));

	for (int i = 0; i < 3; ++i)
		p[i] = (mtx * Vec4f(p[i], 1.0f)).xyz();

	n = (p[1] - p[0] ^ p[2] - p[0]).Normalized();
	d = Vec3Dot(p[0], n);
}

bool _PlaneRay(const plane* pplane, const ray* pray, SIntersection* pinters)
{
	return _RayPlane(pray, pplane, pinters);
}

bool _PlanePlane(const plane* pplane1, const plane* pplane2, SIntersection* pinters)
{
	// d1 and/or d2 might be negative
	float d = Vec3Dot((pplane1->n * pplane1->d).Normalized(), (pplane2->n * pplane2->d).Normalized());
	if (fabsf(fabsf(d) - 1.0f) < FLT_EPSILON)
		return false;

	Vec3f P = pplane2->n * pplane2->d;
	pinters->p = P + ((pplane1->d - Vec3Dot(pplane1->n, P)) / pplane1->n.LengthSq()) * pplane1->n;
	pinters->n = pplane1->n ^ pplane2->n;
	pinters->feature = eINTERSECTION_FEATURE_BASE_SHAPE;
	pinters->dist = 0.0f;
	return true;
}

bool _PlaneSphere(const plane* pplane, const sphere* psphere, SIntersection* pinters)
{
	pinters->p = psphere->c - pplane->n * psphere->r;
	pinters->n = pplane->n;
	pinters->feature = eINTERSECTION_FEATURE_BASE_SHAPE;
	float d = Vec3Dot(pinters->p - pplane->n * pplane->d, pplane->n);
	pinters->dist = d;
	return (d <= 0);
}



int cylinder_plane_intersection(const cylinder* pcyl, const plane* pplane, SIntersection* pinters)
{
	Vec3f cyl_center = (pcyl->p[0] + pcyl->p[1]) * 0.5f;
	float cyl_hh = (pcyl->p[1] - pcyl->p[0]).Length() * 0.5f;
	Vec3f cyl_axis = (pcyl->p[1] - pcyl->p[0]).Normalized();

	Vec3f dirdown = cyl_axis * Vec3Dot(cyl_axis, pplane->n);
	if (dirdown.LengthSq() > sqr(pcyl->r * 1E-3f))
		dirdown = dirdown.Normalized() * pcyl->r;

	pinters->p = cyl_center - cyl_axis * cyl_hh * sgnnz(Vec3Dot(cyl_axis, pplane->n)) + dirdown;
	pinters->n = -pplane->n;
	return isneg(Vec3Dot(pinters->p - pplane->n * pplane->d, pplane->n));
}

bool _PlaneCylinder(const plane* pplane, const cylinder* pcyl, SIntersection* pinters)
{
	int v = cylinder_plane_intersection(pcyl, pplane, pinters);
	return v != 0;
}

bool _PlaneCapsule(const plane* pplane, const capsule* pcapsule, SIntersection* pinters)
{
	quotient t(Vec3Dot(pplane->n * pplane->d - pcapsule->c, pplane->n), Vec3Dot(pcapsule->axis, pplane->n));
	if (fabsf(t.d) < FLT_EPSILON)
	{
		pinters->feature = eINTERSECTION_FEATURE_BASE_SHAPE;
		pinters->n = pplane->n;
		//pinters->p = pcapsule->c + pinters->n * pcapsule->r;
		pinters->p = pcapsule->c + t.n * pplane->n;
		pinters->dist = fabsf(t.n) - pcapsule->r;
		return (pinters->dist <= 0);
	}

	float mint = t.val();
	mint = min(max(mint, -pcapsule->hh), pcapsule->hh);
	Vec3f q = pcapsule->c + mint * pcapsule->axis;
	pinters->feature = eINTERSECTION_FEATURE_CAP;
	pinters->n = pplane->n;
	pinters->p = q - pinters->n * pcapsule->r;
	pinters->dist = fabsf(Vec3Dot(q - pplane->n * pplane->d, pplane->n)) - pcapsule->r;
	return (pinters->dist <= 0);
}

bool _PlaneBox(const plane* pplane, const box* pbox, SIntersection* pinters)
{
	Vec3f ce[3], cp = pbox->c;
	for (int i = 0; i < 3; ++i)
		cp += (ce[i] = pbox->axis[i] * sgnnz(-Vec3Dot(pplane->n, pbox->axis[i]))) * pbox->dim[i];

	int n = 0;
	Vec3f ep, ev, q[3];
	quotient t;
	float tt;
	for (int i = 0; i < 3; ++i)
	{
		ep = cp - ce[i] * pbox->dim[i];
		ev = ce[i] * pbox->dim[i];

		t.set(Vec3Dot(pplane->n * pplane->d - ep, pplane->n), Vec3Dot(ev, pplane->n));
		if (fabsf(t.d) >= FLT_EPSILON || fabsf(t.n) < FLT_EPSILON)
		{
			if (fabsf(tt = t.val()) <= 1.0f)
				q[n++] = cp + tt * ev;
		}
	}

	if (n == 0)
		return false;

	pinters->p = q[0];
	for (int i = 1; i < n; ++i)
		pinters->p += q[i];

	pinters->p /= (float)n;
	pinters->n = pplane->n;
	pinters->dist = -fabsf(Vec3Dot(cp - pplane->n * pplane->d, pplane->n));
	pinters->feature = eINTERSECTION_FEATURE_BASE_SHAPE;
	return true;
}

bool _PlaneTriangle(const plane* pplane, const triangle* ptri, SIntersection* pinters)
{
	Vec3f q[3]; // intersection points
	quotient t;
	float tt;
	int n = 0;
	for (int i = 0; i < 3; ++i)
	{
		t.set(Vec3Dot(pplane->n * pplane->d - ptri->p[i], pplane->n), Vec3Dot(ptri->p[(i + 1) % 3] - ptri->p[i], pplane->n));
		if (fabsf(t.d) >= FLT_EPSILON || fabsf(t.n) < FLT_EPSILON)
		{
			tt = t.val();
			if (tt >= 0.0f && tt <= 1.0f)
				q[n++] = ptri->p[i] + tt * (ptri->p[(i + 1) % 3] - ptri->p[i]);
		}
	}

	if (n < 2)
		return false;

	if (n == 3)
		pinters->p = (q[0] + q[1] + q[2]) * ONE_THIRD;
	else
		pinters->p = (q[0] + q[1]) * 0.5f;
	pinters->n = pplane->n;
	
	float d;
	pinters->dist = 0.0f;
	for (int i = 0; i < 3; ++i)
	{
		if ((d = Vec3Dot(ptri->p[i] - pplane->n * pplane->d, pplane->n)) < pinters->dist)
			pinters->dist = d;
	}

	pinters->feature = eINTERSECTION_FEATURE_BASE_SHAPE;
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Circle
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float circle::GetVolume() const
{
	return 0.0f;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Sphere
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OBB sphere::GetBoundBox() const
{
	OBB obb;
	obb.center = c;
	obb.dimensions[0] = obb.dimensions[1] = obb.dimensions[2] = r;
	return obb;
}

float sphere::GetVolume() const
{
	return 4.0f * SP_ONE_THIRD * SP_PI * (r * r * r);
}

float sphere::GetDistance(const Vec3f& q) const
{
	return (q - c).Length() - r;
}

shape* sphere::Clone() const
{
	sphere* psphere = new sphere();
	psphere->c = c;
	psphere->r = r;
	return psphere;
}

void sphere::Transform(const Mat44& mtx)
{
	Vec3f p = (mtx * Vec4f(c + Vec3f(0, r, 0), 1.0f)).xyz();
	c = (mtx * Vec4f(c, 1.0f)).xyz();
	if (fabsf(r) >= FLT_EPSILON)
		r = (p - c).Length();
}

bool _SphereRay(const sphere* psphere, const ray* pray, SIntersection* pinters)
{
	return _RaySphere(pray, psphere, pinters);
}

bool _SpherePlane(const sphere* psphere, const plane* pplane, SIntersection* pinters)
{
	bool res = _PlaneSphere(pplane, psphere, pinters);
	pinters->n *= -1.0f;
	return res;
}

bool _SphereSphere(const sphere* psphere1, const sphere* psphere2, SIntersection* pinters)
{
	float dsq = (psphere2->c - psphere1->c).LengthSq();
	if (dsq > (psphere1->r + psphere2->r) * (psphere1->r + psphere2->r))
		return false;

	float d = sqrtf(dsq);
	pinters->n = (psphere2->c - psphere1->c) / d;
	pinters->p = psphere1->c + pinters->n * psphere1->r;
	pinters->dist = d - psphere1->r - psphere2->r;
	pinters->feature = eINTERSECTION_FEATURE_BASE_SHAPE;
	return true;
}

bool _SphereCylinder(const sphere* psphere, const cylinder* pcyl, SIntersection* pinters)
{
	Vec3f a = pcyl->p[1] - pcyl->p[0]; // cyl axis
	float a_ln = a.Length();
	a /= a_ln;
	float proj = Vec3Dot(psphere->c - pcyl->p[0], a);
	Vec3f d = psphere->c - (pcyl->p[0] + proj * a); // perp vec from cyl ray to sphere center

	float dd;
	if (proj > 0.0f && proj < a_ln)
	{
		dd = d.LengthSq() - sqr(pcyl->r + psphere->r);
		pinters->feature = eINTERSECTION_FEATURE_BASE_SHAPE;
		pinters->dist = d.Length() - pcyl->r;
		pinters->n = -d.Normalized();
	}
	else
	{
		int cap = (proj > 0);
		Vec3f dp = (psphere->c - Vec3Dot(psphere->c - pcyl->p[cap], a) * a) - pcyl->p[cap];
		if (dp.LengthSq() > pcyl->r * pcyl->r)
			dp = dp.Normalized() * pcyl->r;

		dd = (psphere->c - (pcyl->p[cap] + dp)).LengthSq() - psphere->r * psphere->r;
		pinters->feature = eINTERSECTION_FEATURE_CAP;
		pinters->dist = (psphere->c - (pcyl->p[cap] + dp)).Length() - psphere->r;
		pinters->n = -a * (-1.0f + 2.0f * cap);
	}

	pinters->p = psphere->c + pinters->n * psphere->r;
	return dd < -FLT_EPSILON;
}

bool _SphereCapsule(const sphere* psphere, const capsule* pcapsule, SIntersection* pinters)
{
	float proj = Vec3Dot(psphere->c - pcapsule->c, pcapsule->axis);
	Vec3f d = psphere->c - (pcapsule->c + proj * pcapsule->axis); // perp vec from cyl ray to sphere center

	float dd;
	if (fabsf(proj) <= pcapsule->hh)
	{
		dd = d.LengthSq() - sqr(pcapsule->r + psphere->r);
		float dln = d.Length();
		pinters->feature = eINTERSECTION_FEATURE_BASE_SHAPE;
		pinters->n = -d / dln;
		pinters->dist = dln - pcapsule->r - psphere->r;
	}
	else
	{
		Vec3f capp = pcapsule->c + min(max(proj, -pcapsule->hh), pcapsule->hh) * pcapsule->axis;
		dd = (psphere->c - capp).LengthSq() - sqr(pcapsule->r + psphere->r);
		d = psphere->c - capp;
		float dln = d.Length();
		pinters->feature = eINTERSECTION_FEATURE_CAP;
		pinters->n = -d / dln;
		pinters->dist = dln - pcapsule->r - psphere->r;
	}

	pinters->p = psphere->c + pinters->n * psphere->r;
	return dd < -FLT_EPSILON;
}

bool _SphereBox(const sphere* psphere, const box* pbox, SIntersection* pinters)
{
	Vec3f dc = psphere->c - pbox->c, q = pbox->c;
	float d;
	for (int i = 0; i < 3; ++i)
	{
		d = Vec3Dot(dc, pbox->axis[i]);
		q += min(max(d, -pbox->dim[i]), pbox->dim[i]) * pbox->axis[i];
	}

	pinters->dist = (d = (psphere->c - q).Length()) - psphere->r;
	pinters->n = -(psphere->c - q) / d;
	pinters->p = q;
	pinters->feature = eINTERSECTION_FEATURE_BASE_SHAPE;
	return pinters->dist < FLT_EPSILON;
}

bool _SphereTriangle(const sphere* psphere, const triangle* ptri, SIntersection* pinters)
{
	int inters = 0, i;
	float rsq = psphere->r * psphere->r;

	// Test against perpendicular distance to plane
	Vec3f fp = psphere->c - Vec3Dot(psphere->c - ptri->p[0], ptri->n) * ptri->n;
	if ((psphere->c - fp).LengthSq() > rsq)
		return false;

	// Check if sphere center inside triangle
	inters = 1;
	/*for (i = 0; i < 3; ++i)
		inters &= isneg(Vec3Dot((ptri->p[(i + 1) % 3] - ptri->p[i]) ^ psphere->c - ptri->p[i], ptri->n));*/
	inters = (int)_PointIsInsideTriangle(ptri, fp);

	if (inters)
	{
		pinters->feature = eINTERSECTION_FEATURE_BASE_SHAPE;
		pinters->dist = (psphere->c - fp).Length() - psphere->r;
		pinters->n = -ptri->n;
		pinters->p = psphere->c + pinters->n * psphere->r;
		return pinters->dist <= 0.0f;
	}

	// Intersect with triangle vertices/edges
	float d, dmin = FLT_MAX;
	Vec3f pmin; // closest point on triangle to sphere
	Vec3f ev, ep;
	float ev_ln, ed;
	for (i = 0; i < 3; ++i)
	{
		ev = ptri->p[(i + 1) % 3] - ptri->p[i];
		ev /= (ev_ln = ev.Length());

		ed = Vec3Dot(psphere->c - ptri->p[i], ev);
		ed = min(max(ed, 0), ev_ln);

		ep = ptri->p[i] + ed * ev;

		if ((d = (psphere->c - ep).LengthSq()) <= rsq && d < dmin)
		{
			inters = true;
			dmin = d;
			pmin = ep;
		}
	}

	if (inters)
	{
		pinters->feature = eINTERSECTION_FEATURE_CAP;
		pinters->n = -ptri->n;
		pinters->p = pmin;
		pinters->dist = sqrtf(dmin) - psphere->r;
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Cylinder
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OBB cylinder::GetBoundBox() const
{
	float axisln = (p[1] - p[0]).Length();
	Vec3f naxis = (p[1] - p[0]) / axisln;
	OBB bb;
	bb.center = (p[0] + p[1]) * 0.5f;
	bb.directions[1] = naxis;
	bb.directions[0] = naxis.GetOrthogonal().Normalized();
	bb.directions[2] = naxis ^ bb.directions[0];
	bb.dimensions[0] = bb.dimensions[2] = r;
	bb.dimensions[1] = axisln * 0.5f;
	return bb;
}

float cylinder::GetVolume() const
{
	return SP_PI * r * r * (p[1] - p[0]).Length();
}

float cylinder::GetDistance(const Vec3f& q) const
{
	Vec3f closest[3], vc, v = p[1] - p[0], vn;
	float d, vln = v.Length();
	bool inside = true;
	vn = v / vln;
	
	// Closest point on flat cap 1
	vc = (q + vn * (d = Vec3Dot(q - p[0], -vn))) - p[0];
	if (vc.LengthSq() > r * r)
		vc = r * Vec3Normalize(vc);
	closest[0] = p[0] + vc;
	if (d > 0)
		inside = false;
	
	// Closest point on flat cap 2
	vc = (q - vn * (d = Vec3Dot(q - p[1], vn))) - p[1];
	if (vc.LengthSq() > r * r)
		vc = r * Vec3Normalize(vc);
	closest[1] = p[1] + vc;
	if (d > 0)
		inside = false;

	// Closest point on mantle
	float t = Vec3Dot(q - p[0], vn);
	if (t < 0) t = 0;
	if (t > vln) t = vln;
	Vec3f pp = p[0] + vn * t;
	closest[2] = pp + Vec3Normalize(q - pp) * r;
	if ((q - pp).LengthSq() - r * r > 0)
		inside = false;

	float distMin = FLT_MAX, dist;
	for (int i = 0; i < 3; ++i)
		if ((dist = (q - closest[i]).LengthSq()) < distMin)
			distMin = dist;

	distMin = sqrtf(distMin);
	if (inside)
		distMin *= -1.0f;

	return distMin;
}

shape* cylinder::Clone() const
{
	cylinder* pcyl = new cylinder();
	pcyl->p[0] = p[0];
	pcyl->p[1] = p[1];
	pcyl->r = r;
	return pcyl;
}

void cylinder::Transform(const Mat44& mtx)
{
	Vec3f q = p[0] + (p[1] - p[0]).Normalized().GetOrthogonal() * r;
	q = (mtx * Vec4f(q, 1.0f)).xyz();
	p[0] = (mtx * Vec4f(p[0], 1.0f)).xyz();
	p[1] = (mtx * Vec4f(p[1], 1.0f)).xyz();
	if (fabsf(r) >= FLT_EPSILON)
		r = (q - p[0]).Length();
}

bool _CylinderRay(const cylinder* pcylinder, const ray* pray, SIntersection* pinters)
{
	return _RayCylinder(pray, pcylinder, pinters);
}

bool _CylinderSphere(const cylinder* pcyl, const sphere* psphere, SIntersection* pinters)
{
	return _SphereCylinder(psphere, pcyl, pinters);
}

bool _CylinderCylinder(const cylinder* pcyl1, const cylinder* pcyl2, SIntersection* pinters)
{
	const float lnsq[2] = { (pcyl1->p[1] - pcyl1->p[0]).LengthSq(), (pcyl2->p[1] - pcyl2->p[0]).LengthSq() };
	const Vec3f axis[2] = { (pcyl1->p[1] - pcyl1->p[0]).Normalized(), (pcyl2->p[1] - pcyl2->p[0]).Normalized() };

	// Test infinite cylinders
	Vec3f closest[2];
	closest[0] = pcyl1->p[0] + Vec3Dot(pcyl2->p[0] - pcyl1->p[0], axis[0]) * axis[0];
	closest[1] = pcyl2->p[0] + Vec3Dot(closest[0] - pcyl2->p[0], axis[1]) * axis[1];

	if ((closest[1] - closest[0]).LengthSq() > sqr(pcyl1->r + pcyl2->r))
		return false;

	// Test mantle-mantle
	const float t[2] = { Vec3Dot(closest[0] - pcyl1->p[0], axis[0]), Vec3Dot(closest[1] - pcyl2->p[0], axis[1]) };
	if (t[0] > 0.0f && t[0] * t[0] < lnsq[0] && t[1] > 0.0f && t[1] * t[1] < lnsq[1])
	{
		pinters->n = (closest[1] - closest[0]).Normalized();
		pinters->p = closest[0] + pinters->n * pcyl1->r;
		pinters->feature = eINTERSECTION_FEATURE_BASE_SHAPE;
		pinters->dist = (closest[1] - closest[0]).Length() - pcyl1->r - pcyl2->r;
		return true;
	}

	// Test cap-mantle
	const cylinder* cylinders[] = { pcyl1, pcyl2 };
	int cyl2;
	Vec3f cc, cp, cpp;
	float t_cc, t_cpp;
	for (int cyl = 0; cyl < 2; ++cyl)
		for (int cap = 0; cap < 2; ++cap)
		{
			cyl2 = (cyl + 1) % 2;
			cc = cylinders[cyl2]->p[cap]; // cap center of cyl2

			t_cc = Vec3Dot(cc - cylinders[cyl]->p[0], axis[cyl]);
			if (t_cc < 0.0f || t_cc * t_cc > lnsq[cyl])
				continue;
			if (fabsf((t_cc * t_cc) - (cc - cylinders[cyl]->p[0]).LengthSq()) < FLT_EPSILON)
			{
				pinters->feature = eINTERSECTION_FEATURE_CAP;
				pinters->p = cc;
				pinters->n = axis[cyl2];
				pinters->dist = -cylinders[cyl]->r;
				return true;
			}

			Vec3f cv = (((cc - cylinders[cyl]->p[0]) ^ axis[cyl]) ^ axis[cyl2]).Normalized() * cylinders[cyl2]->r;
			for (int i = 0; i < 2; ++i)
			{
				// TODO: Optimize this by determining the actually closest point
				cp = (i == 0 ? cc + cv : cc - cv); // closest point of cyl2 to cyl1
				t_cpp = Vec3Dot(cp - cylinders[cyl]->p[0], axis[cyl]);
				if (t_cpp >= 0.0f && t_cpp * t_cpp <= lnsq[cyl])
				{
					cpp = cylinders[cyl]->p[0] + t_cpp * axis[cyl];
					if ((cpp - cp).LengthSq() <= cylinders[cyl]->r)
					{
						pinters->feature = eINTERSECTION_FEATURE_CAP;
						pinters->p = cp;
						pinters->n = (cp - cpp).Normalized();
						pinters->dist = (cp - cpp).Length() - cylinders[cyl]->r;
						return true;
					}
				}
			}
		}

	// Test cap-cap
	/*circle caps[2];
	caps[0].r = pcyl1->r;
	caps[1].r = pcyl2->r;
	for (int cap1 = 0; cap1 < 2; ++cap1)
		for (int cap2 = 0; cap2 < 2; ++cap2)
		{
			caps[0].c = cylinders[0]->p[cap1];
			caps[0].n = axis[0] * (-1.0f + 2.0f * cap1);
			caps[1].c = cylinders[1]->p[cap2];
			caps[1].n = axis[1] * (-1.0f + 2.0f * cap2);
			if (_CircleCircle(&caps[0], &caps[1], pinters))
			{
				pinters->feature = eINTERSECTION_FEATURE_CAP;
				return true;
			}
		}*/

	return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Capsule
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OBB capsule::GetBoundBox() const
{
	OBB bb;
	bb.center = c;
	bb.directions[0] = axis.GetOrthogonal().Normalized();
	bb.directions[1] = axis;
	bb.directions[2] = axis ^ bb.directions[0];
	bb.dimensions[0] = bb.dimensions[2] = r;
	bb.dimensions[1] = hh + r;
	return bb;
}

float capsule::GetVolume() const
{
	return SP_PI * r*r * (4.0f * SP_ONE_THIRD * r + 2.0f * hh);
}

float capsule::GetDistance(const Vec3f& q) const
{
	float t = Vec3Dot(q - c, axis);
	if (t < -hh) t = -hh;
	if (t > hh) t = hh;
	return (q - c + t * axis).Length() - r;
}

shape* capsule::Clone() const
{
	capsule* pcapsule = new capsule();
	pcapsule->c = c;
	pcapsule->axis = axis;
	pcapsule->hh = hh;
	pcapsule->r = r;
	return pcapsule;
}

void capsule::Transform(const Mat44& mtx)
{
	Vec3f p[2] = { c - axis * hh, c + axis * hh };
	for (int i = 0; i < 2; ++i)
		p[i] = (mtx * Vec4f(p[i], 1.0f)).xyz();
	
	Vec3f q = (mtx * Vec4f(c + axis.GetOrthogonal() * r, 1.0f)).xyz();
	c = (p[0] + p[1]) * 0.5f;
	hh = 0.5f * (p[1] - p[0]).Length();
	r = (q - c).Length();
}

bool _CapsuleRay(const capsule* pcapsule, const ray* pray, SIntersection* pinters)
{
	return _RayCapsule(pray, pcapsule, pinters);
}

bool _CapsuleSphere(const capsule* pcapsule, const sphere* psphere, SIntersection* pinters)
{
	bool res = _SphereCapsule(psphere, pcapsule, pinters);
	pinters->n *= -1.0f;
	return res;
}

bool _CapsulePlane(const capsule* pcapsule, const plane* pplane, SIntersection* pinters)
{
	bool res = _PlaneCapsule(pplane, pcapsule, pinters);
	pinters->n *= -1.0f;
	return res;
}

bool _CapsuleCapsule(const capsule* pcapsule1, const capsule* pcapsule2, SIntersection* pinters)
{
	const capsule* capsules[] = { pcapsule1, pcapsule2 };
	int inters = 0;
	float s[2], distsq;

	// Cap-Capsule
	pinters->dist = FLT_MAX;
	pinters->feature = eINTERSECTION_FEATURE_CAP;
	Vec3f cp;
	for (int i = 0; i < 2; ++i)
		for (int c = -1; c < 2; c += 2)
		{
			cp = capsules[i]->c + (s[i] = c * capsules[i]->hh) * capsules[i]->axis;
			s[i ^ 1] = Vec3Dot(cp - capsules[i ^ 1]->c, capsules[i ^ 1]->axis);
			s[i ^ 1] = min(max(s[i ^ 1], -capsules[i ^ 1]->hh), capsules[i ^ 1]->hh);
			distsq = (cp - (capsules[i ^ 1]->c + s[i ^ 1] * capsules[i ^ 1]->axis)).LengthSq();
			if ((distsq <= sqr(pcapsule1->r + pcapsule2->r)) & (distsq < pinters->dist))
			{
				inters = 1;
				pinters->dist = distsq;
				pinters->p = capsules[0]->c + s[0] * capsules[0]->axis;
				pinters->n = (capsules[1]->c + s[1] * capsules[1]->axis) - pinters->p;
			}
		}

	// Mantle-Mantle
	Vec3f dc = capsules[1]->c - capsules[0]->c;
	Vec3f d = capsules[0]->axis ^ capsules[1]->axis;
	float dlnsq = d.LengthSq();
	if (isneg(fabsf(Vec3Dot(capsules[0]->axis, capsules[1]->axis)) - (1.0f - FLT_EPSILON))
		& isneg(sqr(Vec3Dot(dc, d)) - sqr(capsules[0]->r + capsules[1]->r) * dlnsq)
		& isneg(fabsf(s[0] = Vec3Dot(dc ^ capsules[1]->axis, d)) - capsules[0]->hh * dlnsq)
		& isneg(fabsf(Vec3Dot(dc ^ capsules[0]->axis, d)) - capsules[1]->hh * dlnsq))
	{
		inters = 1;
		pinters->feature = eINTERSECTION_FEATURE_BASE_SHAPE;
		pinters->p = capsules[0]->c + s[0] * capsules[0]->axis;
		pinters->n = d * sgnnz(Vec3Dot(dc, d));
		pinters->dist = sqr(Vec3Dot(dc, d)) / dlnsq;
	}

	pinters->dist = sqrtf(pinters->dist) - pcapsule1->r - pcapsule2->r;
	pinters->n = pinters->n.Normalized();
	pinters->p += pinters->n * capsules[0]->r;
	return inters;
}

bool _CapsuleBox(const capsule* pcapsule, const box* pbox, SIntersection* pinters)
{
	Vec3f ep, ev, d, dp, cp, pp, sc, n;
	float dlnsq, dln, t[2], tt;
	int i, i2, i3;
	quotient q;

	// Capsule Ray intersection - Box sides
	t[0] = FLT_MAX; t[1] = -FLT_MAX;
	for (i = 0; i < 3; ++i)
	{
		i2 = (i + 1) % 3; i3 = (i + 2) % 3;
		for (int sg = -1; sg <= 1; sg += 2)
		{
			sc = pbox->c + pbox->axis[i] * sg * pbox->dim[i];
			q.set(Vec3Dot(sc - pcapsule->c, pbox->axis[i] * sg), Vec3Dot(pcapsule->axis, pbox->axis[i] * sg));
			if (fabsf(q.d) >= FLT_EPSILON)
			{
				tt = q.val();
				pp = pcapsule->c + tt * pcapsule->axis;
				if (isneg(fabsf(Vec3Dot(pp - sc, pbox->axis[i2])) - pbox->dim[i2]) &
					isneg(fabsf(Vec3Dot(pp - sc, pbox->axis[i3])) - pbox->dim[i3]))
				{
					t[0] = min(t[0], tt);
					t[1] = max(t[1], tt);
					n = pbox->axis[i] * sg;
				}
			}
		}
	}

	if ((t[0] < t[1]) & isneg(t[0] - pcapsule->hh) & isneg(-pcapsule->hh - t[1]))
	{
		cp = pcapsule->c + pcapsule->axis * sgnnz(Vec3Dot(pcapsule->axis, -n)) * pcapsule->hh - n * pcapsule->r;
		pinters->dist = Vec3Dot(cp - pinters->p, n);
		pinters->p = pcapsule->c + t[0] * pcapsule->axis;
		pinters->n = -n;
		pinters->feature = eINTERSECTION_FEATURE_CAP;
		return true;
	}

	// Caps - Box sides
	pinters->feature = eINTERSECTION_FEATURE_CAP;
	for (int cap = -1; cap <= 1; cap += 2)
	{
		cp = pcapsule->c + pcapsule->axis * pcapsule->hh * cap;
		for (i = 0; i < 3; ++i)
		{
			i2 = (i + 1) % 3; i3 = (i + 2) % 3;
			n = pbox->axis[i] * sgnnz(Vec3Dot(cp - pbox->c, pbox->axis[i]));
			pp = cp - (dln = Vec3Dot(cp - (sc = pbox->c + n * pbox->dim[i]), n)) * n;
			if (isneg(pinters->dist = dln - pcapsule->r)
				& isneg(fabsf(Vec3Dot(pp - sc, pbox->axis[i2])) - pbox->dim[i2])
				& isneg(fabsf(Vec3Dot(pp - sc, pbox->axis[i3])) - pbox->dim[i3]))
			{
				pinters->n = -n;
				pinters->p = cp + pinters->n * pcapsule->r;
				return true;
			}
		}
	}

	// All edges - capsule	
	for (int a = 0; a < 3; ++a)
	{
		ev = pbox->axis[a];
		for (int s1 = -1; s1 <= 1; s1 += 2)
			for (int s2 = -1; s2 <= 1; s2 += 2)
			{
				ep = pbox->c - ev * pbox->dim[a]
					+ (float)s1 * pbox->dim[(a + 1) % 3] * pbox->axis[(a + 1) % 3]
					+ (float)s2 * pbox->dim[(a + 2) % 3] * pbox->axis[(a + 2) % 3];

				// edge cylinder
				dp = pcapsule->c - ep;
				dlnsq = (d = ev ^ pcapsule->axis).LengthSq();
				if (isneg(fabsf(Vec3Dot(ev, pcapsule->axis)) - (1.0f - FLT_EPSILON))
					& isneg(sqr(Vec3Dot(dp, d)) - pcapsule->r * pcapsule->r * dlnsq)
					& isneg(fabsf(t[0] = Vec3Dot(dp ^ ev, d)) - pcapsule->hh * dlnsq)
					& isneg(fabsf(Vec3Dot(dp ^ pcapsule->axis, d)) - pbox->dim[a] * 2 * dlnsq))
				{
					dln = sqrtf(dlnsq);
					pinters->p = pcapsule->c + t[0] * pcapsule->axis;
					pinters->n = (d / dln) * sgnnz(Vec3Dot(-dp, d));
					pinters->dist = fabsf(Vec3Dot(dp, d) / dln) - pcapsule->r;
					pinters->feature = eINTERSECTION_FEATURE_BASE_SHAPE;
					return true;
				}

				// edge cap
				for (i = -1; i <= 1; i += 2)
				{
					cp = pcapsule->c + pcapsule->axis * pcapsule->hh * i;
					pp = ep + min(max(Vec3Dot(cp - ep, ev), 0), pbox->dim[a] * 2.0f) * ev;
					dlnsq = (d = cp - pp).LengthSq();
					if (dlnsq <= pcapsule->r * pcapsule->r)
					{
						dln = sqrtf(dlnsq);
						pinters->n = -d / dln;
						pinters->p = cp + pinters->n * pcapsule->r;
						pinters->dist = dln - pcapsule->r;
						pinters->feature = eINTERSECTION_FEATURE_CAP;
						return true;
					}
				}
			}
	}

	return false;
}

bool _CapsuleTriangle(const capsule* pcapsule, const triangle* ptri, SIntersection* pinters)
{
	pinters->dist = FLT_MAX;

	// Clamped Capsule ray - Clamped Triangle edges
	int cap;
	Vec3f p1, p2, dp, d, ev, ep;
	float t1, t2, dlnsqinv, distsq;
	for (int i = 0; i < 3; ++i)
	{
		ev = ptri->p[(i + 1) % 3] - (ep = ptri->p[i]);
		d = ev ^ pcapsule->axis; // v1 x v2
		dlnsqinv = 1.0f / d.LengthSq();
		dp = pcapsule->c - ep;

		// Determine closest points on edge and capsule ray
		// t1: edge, t2: capsule ray
		t1 = Vec3Dot(dp ^ pcapsule->axis, d) * dlnsqinv;
		p1 = ep + min(max(t1, 0), 1.0f) * ev;

		t2 = Vec3Dot(dp ^ ev, d) * dlnsqinv;
		cap = 0;
		if (t2 < -pcapsule->hh) { t2 = -pcapsule->hh; cap = 1; }
		if (t2 > pcapsule->hh) { t2 = pcapsule->hh; cap = 1; }
		p2 = pcapsule->c + t2 * pcapsule->axis;

		if ((distsq = (p2 - p1).LengthSq()) < pinters->dist)
		{
			pinters->p = p1;
			pinters->n = -ptri->n;
			pinters->feature = (cap ? eINTERSECTION_FEATURE_CAP : eINTERSECTION_FEATURE_BASE_SHAPE);
			pinters->dist = distsq;
		}
	}

	// Clamped Capsule ray - Triangle face
	quotient t(Vec3Dot(ptri->p[0] - pcapsule->c, ptri->n), Vec3Dot(pcapsule->axis, ptri->n));
	float tt = t.val();
	cap = 0;
	if (tt < -pcapsule->hh) { tt = -pcapsule->hh; cap = 1; }
	if (tt > pcapsule->hh) { tt = pcapsule->hh; cap = 1; }
	Vec3f cp = pcapsule->c + tt * pcapsule->axis;
	Vec3f pp = cp - Vec3Dot(cp - ptri->p[0], ptri->n) * ptri->n;

	int inside = 1;
	/*for (int i = 0; i < 3; ++i)
		inside &= isneg(Vec3Dot(pp - ptri->p[i], (ptri->p[(i + 1) % 3] - ptri->p[0]) ^ ptri->n));*/
	inside = (int)_PointIsInsideTriangle(ptri, pp);

	if (inside & ((distsq = (pp - cp).LengthSq()) < pinters->dist))
	{
		pinters->dist = distsq;
		pinters->feature = (cap ? eINTERSECTION_FEATURE_CAP : eINTERSECTION_FEATURE_BASE_SHAPE);
		pinters->n = -ptri->n;
		if (cap)
			pinters->p = cp + pinters->n * pcapsule->r;
		else
			pinters->p = cp;
	}

	// TODO: Distance is not quite right if capsule ray intersected with triangle.
	pinters->dist = sqrtf(pinters->dist) - pcapsule->r;	
	return pinters->dist <= 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Triangle
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float triangle::GetVolume() const
{
	return 0.0f;
}

float triangle::GetDistance(const Vec3f& q) const
{
	Vec3f Q = q - Vec3Dot(q - p[0], n) * n; // q projected onto plane
	if (_PointIsInsideTriangle(this, Q))
		return (q - Q).Length();

	//	For each edge UV and remaining point W
	//		If Q and W lie on different sides of UV:
	//			Project Q onto UV and clamp into U-V segment. Add to list of possibly closest points
	//		Else
	//			Add W to the list of possibly closest points
	//	From the list of possibly closest points, select the one (M) with minimum distance to Q
	//	The Distance of q to the triangle then is the distance between M and q
	float distMin = FLT_MAX, dist;
	Vec3f closestMin, closest;
	for (int i = 0; i < 3; ++i)
	{
		const Vec3f &U = p[i], &V = p[(i + 1) % 3], &W = p[(i + 2) % 3];
		Vec3f uv = Vec3Normalize(V - U);
		float t = Vec3Dot(q - U, uv);
		if (Vec3Dot(q - (U + t * uv), W - U) < 0)
		{
			if (t < 0) t = 0;
			if (t > 1.0f) t = 1.0f;
			closest = U + t * uv;
		}
		else
		{
			closest = W;
		}

		if ((dist = (Q - closest).LengthSq()) < distMin)
		{
			distMin = dist;
			closestMin = closest;
		}
	}

	return (q - closestMin).Length();
}

bool _PointIsInsideTriangle(const triangle* ptri, const Vec3f& P)
{
	const Vec3f
		v0 = ptri->p[2] - ptri->p[0],
		v1 = ptri->p[1] - ptri->p[0],
		v2 = P - ptri->p[0];

	const float dot00 = Vec3Dot(v0, v0),
		dot01 = Vec3Dot(v0, v1),
		dot02 = Vec3Dot(v0, v2),
		dot11 = Vec3Dot(v1, v1),
		dot12 = Vec3Dot(v1, v2);

	const float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
	const float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	const float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	return (u >= 0) && (v >= 0) && (u + v < 1.0f);
}

bool _TriangleRay(const triangle* ptri, const ray* pray, SIntersection* pinters)
{
	return _RayTriangle(pray, ptri, pinters);
}

bool _TrianglePlane(const triangle* ptri, const plane* pplane, SIntersection* pinters)
{
	return _PlaneTriangle(pplane, ptri, pinters);
}

bool _TriangleSphere(const triangle* ptri, const sphere* psphere, SIntersection* pinters)
{
	bool res = _SphereTriangle(psphere, ptri, pinters);
	pinters->n *= -1.0f;
	return res;
}

bool _TriangleCapsule(const triangle* ptri, const capsule* pcapsule, SIntersection* pinters)
{
	bool res = _CapsuleTriangle(pcapsule, ptri, pinters);
	pinters->n *= -1.0f;
	return res;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Circle
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
The circles intersect if their ranges on the intersection line of both circle planes intersect.
*/
bool _CircleCircle(const circle* pcircle1, const circle* pcircle2, SIntersection* pinters)
{
	pinters->feature = eINTERSECTION_FEATURE_BASE_SHAPE;
	pinters->dist = 0.0f;
	pinters->n = pcircle1->n;

	Vec3f dc = pcircle2->c - pcircle1->c;
	if (dc.LengthSq() > sqr(pcircle1->r + pcircle2->r)) // test bounding spheres
		return false;

	Vec3f lnDir = pcircle1->n ^ pcircle2->n;
	Vec3f lnPoint = dc ^ lnDir;

	if (fabsf(Vec3Dot(pcircle1->n, pcircle2->n)) >= 1.0f - FLT_EPSILON) // parallel
	{
		if (fabsf(Vec3Dot(dc, pcircle2->n)) < FLT_EPSILON) // coincident
		{
			pinters->p = pcircle1->c + (dc.LengthSq() < FLT_EPSILON ? Vec3f(0) : dc.Normalized()) * pcircle1->r;
			return (dc.LengthSq() <= sqr(pcircle1->r + pcircle2->r));
		}
		return false;
	}

	const circle* circles[] = { pcircle1, pcircle2 };
	float t[2][2], A, B, C, discr, sqrt_discr, inv_den;
	int inters = 1;
	for (int i = 0; i < 2; ++i)
	{
		A = lnDir.LengthSq();
		B = 2.0f * Vec3Dot(lnPoint - circles[i]->c, lnDir);
		C = (lnPoint - circles[i]->c).LengthSq() - circles[i]->r * circles[i]->r;
		discr = B * B - 4.0f * A * C;
		if (inters &= (discr >= 0.0f))
		{
			sqrt_discr = sqrtf(discr);
			inv_den = 1.0f / (2.0f * A);
			t[i][0] = (-B - sqrt_discr) * inv_den;
			t[i][1] = (-B + sqrt_discr) * inv_den;
		}
	}

	// t[i][0] <= t[i][1]
	inters &= (t[0][0] <= t[1][1] && t[1][0] <= t[0][1]);
	float t_min = (t[0][0] > t[1][0] ? t[0][0] : t[1][0]);
	float t_max = (t[0][1] < t[1][1] ? t[0][1] : t[1][1]);
	pinters->p = lnPoint + (t_min + t_max) * 0.5f * lnDir;
	return inters;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Box
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OBB box::GetBoundBox() const
{
	OBB bb;
	bb.center = c;
	bb.dimensions[0] = dim[0];
	bb.dimensions[1] = dim[1];
	bb.dimensions[2] = dim[2];
	bb.directions[0] = axis[0];
	bb.directions[1] = axis[1];
	bb.directions[2] = axis[2];
	return bb;
}

float box::GetVolume() const
{
	return 8.0f * dim[0] * dim[1] * dim[2];
}

float box::GetDistance(const Vec3f& q) const
{
	Vec3f Q = Mat33::FromColumns(axis[0], axis[1], axis[2]).Transposed() * q;

	float distMin = FLT_MAX, dist;
	Vec3f closestMin; // in box-space
	for (int i = 0; i < 3; ++i)
	{
		Vec3f n = Vec3f(0);
		n[i] = Q[i] < 0 ? -dim[i] : dim[i];

		// Perpendicular foot of Q on plane
		Vec3f tp = Q - Vec3Dot(Q - n, n) * n;

		// Clamp tp into other dimensions
		for (int j = 0; j < 3; ++j)
			if (j != i)
				tp[j] = min(max(tp[j], -dim[j]), dim[j]);

		if ((dist = (Q - tp).LengthSq()) < distMin)
		{
			distMin = dist;
			closestMin = tp;
		}
	}

	//closestMin = Mat33::FromColumns(axis[0], axis[1], axis[2]) * closestMin;
	return sqrtf(distMin);
}

shape* box::Clone() const
{
	box* pbox = new box();
	pbox->c = c;
	for (int i = 0; i < 3; ++i)
	{
		pbox->axis[i] = axis[i];
		pbox->dim[i] = dim[i];
	}

	return pbox;
}

void box::Transform(const Mat44& mtx)
{
	c = (mtx * Vec4f(c, 1.0f)).xyz();
	for (int i = 0; i < 3; ++i)
	{
		axis[i] = (mtx * Vec4f(axis[i], 0)).xyz();
		axis[i] /= (dim[i] = axis[i].Length());
	}
}



bool _BoxBoxExists(const box* pbox1, const box* pbox2)
{
	float ra, rb;
	Mat33 R, AbsR;
	Vec3f dc;
	Vec3f n;
	float d, dmin = FLT_MAX;

	dc = pbox2->c - pbox1->c;
	dc = Vec3f(Vec3Dot(dc, pbox1->axis[0]), Vec3Dot(dc, pbox1->axis[1]), Vec3Dot(dc, pbox1->axis[2]));

	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			R.m[i][j] = Vec3Dot(pbox1->axis[i], pbox2->axis[j]);

	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			AbsR.m[i][j] = fabsf(R.m[i][j]) + FLT_EPSILON;

	for (int i = 0; i < 3; ++i)
	{
		ra = pbox1->dim[i];
		rb = pbox2->dim[0] * AbsR.m[i][0] + pbox2->dim[1] * AbsR.m[i][1] + pbox2->dim[2] * AbsR.m[i][2];
		if ((d = fabsf(dc[i]) - ra - rb) > 0) return false;
		if (fabsf(d) < fabsf(dmin)) { dmin = d; n = pbox1->axis[i]; }
	}

	for (int i = 0; i < 3; ++i)
	{
		ra = pbox1->dim[0] * AbsR.m[0][i] + pbox1->dim[1] * AbsR.m[1][i] + pbox1->dim[2] * AbsR.m[2][i];
		rb = pbox2->dim[i];
		if ((d = fabsf(dc[0] * R.m[0][i] + dc[1] * R.m[1][i] + dc[2] * R.m[2][i]) - ra - rb) > 0) return false;
		if (fabsf(d) < fabsf(dmin)) { dmin = d; n = pbox2->axis[i]; }
	}

	// Test axis L = A0 x B0
	ra = pbox1->dim[1] * AbsR.m[2][0] + pbox1->dim[2] * AbsR.m[1][0];
	rb = pbox2->dim[1] * AbsR.m[0][2] + pbox2->dim[2] * AbsR.m[0][1];
	if ((d = fabsf(dc[2] * R.m[1][0] - dc[1] * R.m[2][0]) - ra - rb) > 0) return 0;
	if (fabsf(d) < fabsf(dmin)) { dmin = d; n = pbox1->axis[0] ^ pbox2->axis[0]; }

	// Test axis L = A0 x B1
	ra = pbox1->dim[1] * AbsR.m[2][1] + pbox1->dim[2] * AbsR.m[1][1];
	rb = pbox2->dim[0] * AbsR.m[0][2] + pbox2->dim[2] * AbsR.m[0][0];
	if ((d = fabsf(dc[2] * R.m[1][1] - dc[1] * R.m[2][1]) - ra - rb) > 0) return 0;
	if (fabsf(d) < fabsf(dmin)) { dmin = d; n = pbox1->axis[0] ^ pbox2->axis[1]; }
	
	// Test axis L = A0 x B2
	ra = pbox1->dim[1] * AbsR.m[2][2] + pbox1->dim[2] * AbsR.m[1][2];
	rb = pbox2->dim[0] * AbsR.m[0][1] + pbox2->dim[1] * AbsR.m[0][0];
	if ((d = fabsf(dc[2] * R.m[1][2] - dc[1] * R.m[2][2]) - ra - rb) > 0) return 0;
	if (fabsf(d) < fabsf(dmin)) { dmin = d; n = pbox1->axis[0] ^ pbox2->axis[2]; }

	// Test axis L = A1 x B0
	ra = pbox1->dim[0] * AbsR.m[2][0] + pbox1->dim[2] * AbsR.m[0][0];
	rb = pbox2->dim[1] * AbsR.m[1][2] + pbox2->dim[2] * AbsR.m[1][1];
	if ((d = fabsf(dc[0] * R.m[2][0] - dc[2] * R.m[0][0]) - ra - rb) > 0) return 0;
	if (fabsf(d) < fabsf(dmin)) { dmin = d; n = pbox1->axis[1] ^ pbox2->axis[0]; }

	// Test axis L = A1 x B1
	ra = pbox1->dim[0] * AbsR.m[2][1] + pbox1->dim[2] * AbsR.m[0][1];
	rb = pbox2->dim[0] * AbsR.m[1][2] + pbox2->dim[2] * AbsR.m[1][0];
	if ((d = fabsf(dc[0] * R.m[2][1] - dc[2] * R.m[0][1]) - ra - rb) > 0) return 0;
	if (fabsf(d) < fabsf(dmin)) { dmin = d; n = pbox1->axis[1] ^ pbox2->axis[1]; }

	// Test axis L = A1 x B2
	ra = pbox1->dim[0] * AbsR.m[2][2] + pbox1->dim[2] * AbsR.m[0][2];
	rb = pbox2->dim[0] * AbsR.m[1][1] + pbox2->dim[1] * AbsR.m[1][0];
	if ((d = fabsf(dc[0] * R.m[2][2] - dc[2] * R.m[0][2]) - ra - rb) > 0) return 0;
	if (fabsf(d) < fabsf(dmin)) { dmin = d; n = pbox1->axis[1] ^ pbox2->axis[2]; }

	// Test axis L = A2 x B0
	ra = pbox1->dim[0] * AbsR.m[1][0] + pbox1->dim[1] * AbsR.m[0][0];
	rb = pbox2->dim[1] * AbsR.m[2][2] + pbox2->dim[2] * AbsR.m[2][1];
	if ((d = fabsf(dc[1] * R.m[0][0] - dc[0] * R.m[1][0]) - ra - rb) > 0) return 0;
	if (fabsf(d) < fabsf(dmin)) { dmin = d; n = pbox1->axis[2] ^ pbox2->axis[0]; }

	// Test axis L = A2 x B1
	ra = pbox1->dim[0] * AbsR.m[1][1] + pbox1->dim[1] * AbsR.m[0][1];
	rb = pbox2->dim[0] * AbsR.m[2][2] + pbox2->dim[2] * AbsR.m[2][0];
	if ((d = fabsf(dc[1] * R.m[0][1] - dc[0] * R.m[1][1]) - ra - rb) > 0) return 0;
	if (fabsf(d) < fabsf(dmin)) { dmin = d; n = pbox1->axis[2] ^ pbox2->axis[1]; }

	// Test axis L = A2 x B2
	ra = pbox1->dim[0] * AbsR.m[1][2] + pbox1->dim[1] * AbsR.m[0][2];
	rb = pbox2->dim[0] * AbsR.m[2][1] + pbox2->dim[1] * AbsR.m[2][0];
	if ((d = fabsf(dc[1] * R.m[0][2] - dc[0] * R.m[1][2]) - ra - rb) > 0) return 0;
	if (fabsf(d) < fabsf(dmin)) { dmin = d; n = pbox1->axis[2] ^ pbox2->axis[2]; }

	return true;
}

void SetBasisFromMtx(Vec3f basis[3], const Mat33& mtx)
{
	basis[0] = Vec3f(mtx.m[0][0], mtx.m[0][1], mtx.m[0][2]);
	basis[1] = Vec3f(mtx.m[1][0], mtx.m[1][1], mtx.m[1][2]);
	basis[2] = Vec3f(mtx.m[2][0], mtx.m[2][1], mtx.m[2][2]);
}

#define _SWAP(x,y) t=x;x=y;y=t;
void TransposeBasis(Vec3f basis[3])
{
	float t;
	_SWAP(basis[0].y, basis[1].x);
	_SWAP(basis[0].z, basis[2].x);
	_SWAP(basis[1].z, basis[2].y);
}

#define BOXBOX_UPDATE_BEST(axis, cp) \
	if (fabsf(d) < fabsf(dmin)) { \
		dmin = d; \
		L = axis; \
		P = cp; \
	}

#define ORANGE SColor(1.0f, 0.5f, 0.0f)
#define YELLOW SColor::Yellow()

bool _BoxBox(const box* pbox1, const box* pbox2, SIntersection* pinters)
{
	const box* boxes[2] = { pbox1, pbox2 }, *box1, *box2;
	
	float dmin = FLT_MAX, d;
	Vec3f L; // separation axis with d closest to 0
	Vec3f P; // contact point

	Vec3f axes[3];
	Mat33 basis[2];
	basis[0] = Mat33::FromColumns(pbox1->axis[0], pbox1->axis[1], pbox1->axis[2]);
	basis[1] = Mat33::FromColumns(pbox2->axis[0], pbox2->axis[1], pbox2->axis[2]);
	SetBasisFromMtx(axes, basis[0] * basis[1].Transposed());

	Vec3f dc[2], dcw[2];
	dc[0] = basis[0] * (dcw[0] = pbox2->c - pbox1->c);
	dc[1] = basis[1] * (dcw[1] = pbox1->c - pbox2->c);

	// Face-Vertex
	float sgcoord;
	Vec3f origin, vtx;
	int sg[3];
	for (int ibox = 0; ibox < 2; ++ibox) // ibox provides face, ibox^1 provides vertex
	{
		box1 = boxes[ibox]; box2 = boxes[ibox ^ 1];
		for (int icoord = 0; icoord < 3; ++icoord)
		{
			origin[icoord] = box1->dim[icoord] * (sgcoord = sgnnz(dc[ibox][icoord]));
			sg[0] = sgnnz(axes[0][icoord]); sg[1] = sgnnz(axes[1][icoord]); sg[2] = sgnnz(axes[2][icoord]);
			vtx = dc[ibox] + (axes[0] * (box2->dim[0] * sg[0]) + axes[1] * (box2->dim[1] * sg[1]) + axes[2] * (box2->dim[2] * sg[2])) * -sgcoord;

			float ra = fabsf(origin[icoord]);
			float rb = fabsf((vtx[icoord] - dc[ibox][icoord]));
			float ToL = fabsf(dc[ibox][icoord]);

			d = ToL - ra - rb;

			if (d > 0)
			{
				return false;
			}

			BOXBOX_UPDATE_BEST(box1->axis[icoord] * (((ibox ^ 1) << 1) - 1) * sgcoord, basis[ibox].Transposed() * vtx + box1->c);
			origin[icoord] = 0;
		}

		TransposeBasis(axes);
	}

	// Edge-Edge
	box1 = pbox1; box2 = pbox2;
	Vec3f n, p[2];
	int ie12, ie13, ie22, ie23;
	for (int ie1 = 0; ie1 < 3; ++ie1)
		for (int ie2 = 0; ie2 < 3; ++ie2)
		{
			ie12 = (ie1 + 1) % 3; ie13 = (ie1 + 2) % 3;
			ie22 = (ie2 + 1) % 3; ie23 = (ie2 + 2) % 3;

			Vec3f a;
			a[ie1] = 1.0f;
			n = a ^ axes[ie2];
			if (n.LengthSq() < FLT_EPSILON)
				continue; // axes are parallel

			n = n.Normalized();
			n *= sgnnz(Vec3Dot(n, dc[0]));

			p[0][ie1] = 0;
			p[0][ie12] = pbox1->dim[ie12] * sgnnz(n[ie12]);
			p[0][ie13] = pbox1->dim[ie13] * sgnnz(n[ie13]);

			Vec3f a11, a12;
			p[1] = dc[0]
				+ (a11 = axes[ie22] * pbox2->dim[ie22] * sgnnz(Vec3Dot(axes[ie22], -n)))
				+ (a12 = axes[ie23] * pbox2->dim[ie23] * sgnnz(Vec3Dot(axes[ie23], -n)));

			float ra = fabsf(Vec3Dot(p[0], n));
			float rb = fabsf(Vec3Dot(p[1] - dc[0], n));
			float ToL = fabsf(Vec3Dot(dc[0], n));

			d = ToL - ra - rb;

			if (d > 0)
				return false;

			Mat33 BT = basis[0].Transposed();
			BOXBOX_UPDATE_BEST(basis[0].Transposed() * n, ((BT * p[0] + box1->c) + (BT * p[1] + box1->c)) * 0.5f);
		}

	pinters->dist = dmin;
	pinters->n = L.Normalized();
	pinters->p = P;
	pinters->feature = eINTERSECTION_FEATURE_BASE_SHAPE;
	return true;
}

bool _BoxRay(const box* pbox, const ray* pray, SIntersection* pinters)
{
	return _RayBox(pray, pbox, pinters);
}

bool _BoxPlane(const box* pbox, const plane* pplane, SIntersection* pinters)
{
	bool res = _PlaneBox(pplane, pbox, pinters);
	pinters->n *= -1.0f;
	return res;
}

bool _BoxSphere(const box* pbox, const sphere* psphere, SIntersection* pinters)
{
	bool res = _SphereBox(psphere, pbox, pinters);
	pinters->n *= -1.0f;
	return res;
}

bool _BoxCapsule(const box* pbox, const capsule* pcapsule, SIntersection* pinters)
{
	bool res = _CapsuleBox(pcapsule, pbox, pinters);
	pinters->n *= -1.0f;
	return res;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Mesh
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline int _TestLineSegmentAABB(const Vec3f& p0, const Vec3f& p1, const AABB& aabb)
{
	Vec3f c = (aabb.vMin + aabb.vMax) * 0.5f;
	Vec3f e = aabb.vMax - c;
	Vec3f m = (p0 + p1) * 0.5f;
	Vec3f d = p1 - m;
	m = m - c;

	// Try world coordinate axes as separating axes
	float adx = fabsf(d.x);
	if (fabsf(m.x) > e.x + adx) return 0;
	float ady = fabsf(d.y);
	if (fabsf(m.y) > e.y + ady) return 0;
	float adz = fabsf(d.z);
	if (fabsf(m.z) > e.z + adz) return 0;

	adx += FLT_EPSILON; ady += FLT_EPSILON; adz += FLT_EPSILON;
	if (fabsf(m.y * d.z - m.z * d.y) > e.y * adz + e.z * ady) return 0;
	if (fabsf(m.z * d.x - m.x * d.z) > e.x * adz + e.z * adx) return 0;
	if (fabsf(m.x * d.y - m.y * d.x) > e.x * ady + e.y * adx) return 0;

	// No separating axis found
	return 1;
}


struct TriBuffer
{
	unsigned int* buffer;
	unsigned int capacity;
	unsigned int size;

	TriBuffer() : buffer(0), capacity(0), size(0) {}
};

void CreateMeshTreeNode(mesh* pmesh, mesh_tree_node* pnode, const TriBuffer& parentTris, bool octree, unsigned int maxDepth, Vec3f* triCache, unsigned int depth = 1)
{
	TriBuffer tris;
	unsigned int itri;
	for (unsigned int i = 0; i < parentTris.size; ++i)
	{
		itri = parentTris.buffer[i];

		static Vec3f p[3];
		p[0] = triCache[itri];
		p[1] = triCache[itri + 1];
		p[2] = triCache[itri + 2];

		if (/*pnode->aabb.ContainsPoint(p[0]) || pnode->aabb.ContainsPoint(p[1]) || pnode->aabb.ContainsPoint(p[2]) ||*/
			_TestLineSegmentAABB(p[0], p[1], pnode->aabb)
			|| _TestLineSegmentAABB(p[1], p[2], pnode->aabb)
			|| _TestLineSegmentAABB(p[2], p[0], pnode->aabb))
		{
			if (!tris.buffer || tris.size >= tris.capacity)
			{
				unsigned int newCapacity = tris.capacity + 20;
				unsigned int* newBuffer = new unsigned int[newCapacity];
				if (tris.buffer)
				{
					memcpy(newBuffer, tris.buffer, tris.size * sizeof(unsigned int));
					delete[] tris.buffer;
					tris.buffer = 0;
				}

				tris.buffer = newBuffer;
				tris.capacity = newCapacity;
			}

			tris.buffer[tris.size] = itri;
			tris.size++;
		}
	}

	if (depth < maxDepth && tris.size > 16) // min 16 triangles in one BV
	{
		pnode->tris = 0;
		pnode->num_tris = 0;
		pnode->num_children = (octree ? 8 : 4);
		pnode->children = new mesh_tree_node[pnode->num_children];

		Vec3f vMin = pnode->aabb.vMin, vMax = pnode->aabb.vMax;
		Vec3f vCenter = (vMin + vMax) * 0.5f;
		if (octree)
		{
			pnode->children[0].aabb = AABB(vMin, vCenter);
			pnode->children[1].aabb = AABB(Vec3f(vMin.x, vMin.y, vCenter.z), Vec3f(vCenter.x, vCenter.y, vMax.z));
			pnode->children[2].aabb = AABB(Vec3f(vCenter.x, vMin.y, vCenter.z), Vec3f(vMax.x, vCenter.y, vMax.z));
			pnode->children[3].aabb = AABB(Vec3f(vCenter.x, vMin.y, vMin.z), Vec3f(vMax.x, vCenter.y, vCenter.z));
			pnode->children[4].aabb = AABB(Vec3f(vMin.x, vCenter.y, vMin.z), Vec3f(vCenter.x, vMax.y, vCenter.z));
			pnode->children[5].aabb = AABB(Vec3f(vMin.x, vCenter.y, vCenter.z), Vec3f(vCenter.x, vMax.y, vMax.z));
			pnode->children[6].aabb = AABB(Vec3f(vCenter.x, vCenter.y, vCenter.z), Vec3f(vMax.x, vMax.y, vMax.z));
			pnode->children[7].aabb = AABB(Vec3f(vCenter.x, vCenter.y, vMin.z), Vec3f(vMax.x, vMax.y, vCenter.z));
		}
		else
		{
			pnode->children[0].aabb = AABB(vMin, Vec3f(vCenter.x, vMax.y, vCenter.z));
			pnode->children[1].aabb = AABB(Vec3f(vCenter.x, vMin.y, vMin.z), Vec3f(vMax.x, vMax.y, vCenter.z));
			pnode->children[2].aabb = AABB(Vec3f(vCenter.x, vMin.y, vCenter.z), Vec3f(vMax.x, vMax.y, vMax.z));
			pnode->children[3].aabb = AABB(Vec3f(vMin.x, vMin.y, vCenter.z), Vec3f(vCenter.x, vMax.y, vMax.z));
		}

		for (unsigned int i = 0; i < pnode->num_children; ++i)
			CreateMeshTreeNode(pmesh, &pnode->children[i], tris, octree, maxDepth, triCache, depth + 1);
	}
	else
	{
		pnode->num_children = 0;
		pnode->children = 0;
		pnode->num_tris = tris.size;
		if (pnode->num_tris > 0)
		{
			// !! We do not copy the buffer here! Instead we takeover control of it
			pnode->tris = tris.buffer;
			tris.buffer = 0;
		}

		static bool seeded = false;
		if (!seeded)
		{
			srand(static_cast<unsigned>(time(0)));
			seeded = true;
		}

		pnode->_color.r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		pnode->_color.g = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		pnode->_color.b = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		//PhysDebug::VisualizeAABB(pnode->aabb, pnode->_color);
	}

	if (tris.buffer)
	{
		delete[] tris.buffer;
		tris.buffer = 0;
	}
}

void mesh::CreateTree(bool octree, unsigned int maxDepth)
{
	ClearTree();
	if (!points || !indices || num_points == 0 || num_indices == 0)
		return;

	for (unsigned int i = 0; i < num_points; ++i)
		root.aabb.AddPoint(points[i]);

	TriBuffer rootTris;
	rootTris.capacity = (unsigned int)(num_indices / 3);
	rootTris.buffer = new unsigned int[rootTris.capacity];

	// Cache triangle points
	Vec3f* triCache = (Vec3f*)malloc(sizeof(Vec3f) * num_indices);

	for (unsigned int i = 0; i < num_indices; i += 3)
	{
		rootTris.buffer[rootTris.size++] = i;
		triCache[i] = points[indices[i]];
		triCache[i + 1] = points[indices[i + 1]];
		triCache[i + 2] = points[indices[i + 2]];
	}

	CreateMeshTreeNode(this, &root, rootTris, octree, maxDepth, triCache);

	if (rootTris.buffer)
	{
		delete[] rootTris.buffer;
		rootTris.buffer = 0;
	}

	free(triCache);
}


//inline void VisualizeMeshTreeNode(mesh_tree_node* pnode)
//{
//	static bool seeded = false;
//	if (!seeded)
//	{
//		srand(static_cast<unsigned>(time(0)));
//		seeded = true;
//	}
//
//	pnode->_color.r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
//	pnode->_color.g = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
//	pnode->_color.b = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
//
//	PhysDebug::VisualizeAABB(pnode->aabb, pnode->_color);
//}
//
//inline void VisualizeMeshTreeNodeTriAssignment(const mesh* pmesh, const mesh_tree_node* pnode, unsigned int itri)
//{
//	Vec3f p[3];
//	p[0] = pmesh->points[pmesh->indices[itri]];
//	p[1] = pmesh->points[pmesh->indices[itri + 1]];
//	p[2] = pmesh->points[pmesh->indices[itri + 2]];
//	Vec3f tricenter = (p[0] + p[1] + p[2]) / 3.0f;
//	Vec3f aabbcenter = (pnode->aabb.vMin + pnode->aabb.vMax) * 0.5f;
//
//	PhysDebug::VisualizeVector(tricenter, aabbcenter - tricenter, pnode->_color);
//}
//
//unsigned int DetermineMeshMaxTreeDepth(mesh_tree_node* pnode)
//{
//	if (!pnode)
//	{
//		return 0;
//	}
//	else if (!pnode->children)
//	{
//		return 1;
//	}
//	else
//	{
//		unsigned int depth, maxDepth = 0;
//		for (unsigned int i = 0; i < pnode->num_children; ++i)
//		{
//			depth = DetermineMeshMaxTreeDepth(&pnode->children[i]);
//			maxDepth = max(maxDepth, depth);
//		}
//
//		return maxDepth + 1; // +1 for current node
//	}
//}
//
//void DumpMeshTree(const mesh* pmesh, const mesh_tree_node* pnode, unsigned int curDepth = 0)
//{
//	if (!pnode)
//		return;
//
//	static string indent;
//	indent = "  ";
//	for (unsigned int i = 0; i < curDepth; ++i)
//		indent += "  ";
//
//	static string tris;
//	tris = "";
//	if (pnode->tris)
//	{
//		tris += "{ ";
//		for (unsigned int i = 0; i < pnode->num_tris; ++i)
//		{
//			if (i > 0)
//				tris += ", ";
//
//			tris += std::to_string(pnode->tris[i]);
//		}
//		tris += " }";
//	}
//
//	_d("%s[num_children=%d, num_tris=%d] %s", indent.c_str(), pnode->num_children, pnode->num_tris, tris.c_str());
//
//	if (pnode->children)
//	{
//		for (unsigned int i = 0; i < pnode->num_children; ++i)
//			DumpMeshTree(pmesh, &pnode->children[i], curDepth + 1);
//	}
//}
//
//inline bool MeshTreeNodeContainsTri(const mesh* pmesh, const mesh_tree_node* pnode, unsigned int itri)
//{
//	static Vec3f p[3];
//	p[0] = pmesh->points[pmesh->indices[itri]];
//	p[1] = pmesh->points[pmesh->indices[itri + 1]];
//	p[2] = pmesh->points[pmesh->indices[itri + 2]];
//
//	/*return _TestLineSegmentAABB(p[0], p[1], pnode->aabb)
//		| _TestLineSegmentAABB(p[1], p[2], pnode->aabb)
//		| _TestLineSegmentAABB(p[2], p[0], pnode->aabb);*/
//
//	return pnode->aabb.ContainsPoint(p[0])
//		|| pnode->aabb.ContainsPoint(p[1])
//		|| pnode->aabb.ContainsPoint(p[2])
//		|| pnode->aabb.HitsLineSegment(p[0], p[1])
//		|| pnode->aabb.HitsLineSegment(p[1], p[2])
//		|| pnode->aabb.HitsLineSegment(p[2], p[0]);
//}
//
//inline void SplitMeshTreeNode(mesh* pmesh, mesh_tree_node* pnode, bool octree)
//{
//	// Create children
//	pnode->num_children = (octree ? 8 : 4);
//	pnode->children = new mesh_tree_node[pnode->num_children];
//
//	Vec3f vMin = pnode->aabb.vMin, vMax = pnode->aabb.vMax;
//	Vec3f vCenter = (vMin + vMax) * 0.5f;
//	if (octree)
//	{
//		pnode->children[0].aabb = AABB(vMin, vCenter);
//		pnode->children[1].aabb = AABB(Vec3f(vMin.x, vMin.y, vCenter.z), Vec3f(vCenter.x, vCenter.y, vMax.z));
//		pnode->children[2].aabb = AABB(Vec3f(vCenter.x, vMin.y, vCenter.z), Vec3f(vMax.x, vCenter.y, vMax.z));
//		pnode->children[3].aabb = AABB(Vec3f(vCenter.x, vMin.y, vMin.z), Vec3f(vMax.x, vCenter.y, vCenter.z));
//		pnode->children[4].aabb = AABB(Vec3f(vMin.x, vCenter.y, vMin.z), Vec3f(vCenter.x, vMax.y, vCenter.z));
//		pnode->children[5].aabb = AABB(Vec3f(vMin.x, vCenter.y, vCenter.z), Vec3f(vCenter.x, vMax.y, vMax.z));
//		pnode->children[6].aabb = AABB(Vec3f(vCenter.x, vCenter.y, vCenter.z), Vec3f(vMax.x, vMax.y, vMax.z));
//		pnode->children[7].aabb = AABB(Vec3f(vCenter.x, vCenter.y, vMin.z), Vec3f(vMax.x, vMax.y, vCenter.z));
//	}
//	else
//	{
//		pnode->children[0].aabb = AABB(vMin, Vec3f(vCenter.x, vMax.y, vCenter.z));
//		pnode->children[1].aabb = AABB(Vec3f(vCenter.x, vMin.y, vMin.z), Vec3f(vMax.x, vMax.y, vCenter.z));
//		pnode->children[2].aabb = AABB(Vec3f(vCenter.x, vMin.y, vCenter.z), Vec3f(vMax.x, vMax.y, vMax.z));
//		pnode->children[3].aabb = AABB(Vec3f(vMin.x, vMin.y, vCenter.z), Vec3f(vCenter.x, vMax.y, vMax.z));
//	}
//
//	// Insert existing tris into children
//	if (pnode->tris)
//	{
//		mesh_tree_node* pchild = 0;
//		for (unsigned int i = 0; i < pnode->num_tris; ++i)
//		{
//			for (unsigned int ichild = 0; ichild < pnode->num_children; ++ichild)
//			{
//				unsigned int itri = pnode->tris[i];
//
//				pchild = &pnode->children[ichild];
//				if (!MeshTreeNodeContainsTri(pmesh, pchild, itri))
//					continue;
//
//				if (!pchild->tris)
//				{
//					pchild->tris = new unsigned int[pnode->num_tris];
//					pchild->num_tris = 0;
//
//					//VisualizeMeshTreeNode(pchild);
//				}
//
//				pchild->tris[pchild->num_tris] = itri;
//				pchild->num_tris++;
//			}
//		}
//
//		delete[] pnode->tris;
//		pnode->tris = 0;
//		pnode->num_tris = 0;
//	}
//}
//
//// maxTrisPerLeaf - algorithm will split leaf node when (maxTrisPerLeaf + 1)th tri is added to it
//void InsertMeshTreeTriangle(mesh* pmesh, unsigned int itri, unsigned int maxTrisPerLeaf = 4, bool octree = true)
//{
//	struct open_node
//	{
//		mesh_tree_node* pnode;
//		unsigned int depth;
//
//		open_node() : pnode(0), depth(0) {}
//		open_node(mesh_tree_node* _pnode) : pnode(_pnode), depth(0) {}
//		open_node(mesh_tree_node* _pnode, unsigned int _depth) : pnode(_pnode), depth(_depth) {}
//	};
//
//	static std::stack<open_node> openNodes;
//	openNodes.emplace(&pmesh->root);
//
//	bool propagate;
//	mesh_tree_node* pnode = 0;
//	while (openNodes.size() > 0)
//	{
//		open_node node = openNodes.top();
//		openNodes.pop();
//
//		pnode = node.pnode;
//		propagate = (bool)(pnode->children);
//
//		if (!propagate && pnode->num_tris >= maxTrisPerLeaf)
//		{
//			SplitMeshTreeNode(pmesh, pnode, octree);
//			propagate = true;
//		}
//
//		if (propagate)
//		{
//			for (unsigned int ichild = 0; ichild < pnode->num_children; ++ichild)
//			{
//				if (!MeshTreeNodeContainsTri(pmesh, pnode, itri))
//					continue;
//
//				openNodes.emplace(&pnode->children[ichild], node.depth + 1);
//			}
//		}
//		else
//		{
//			if (!pnode->tris)
//			{
//				pnode->tris = new unsigned int[maxTrisPerLeaf];
//				pnode->num_tris = 0;
//
//				//VisualizeMeshTreeNode(pnode);
//			}
//
//			pnode->tris[pnode->num_tris] = itri;
//			pnode->num_tris++;
//		}
//	}
//}
//
//void mesh::CreateTree(bool octree, unsigned int maxTrisPerLeaf /*= 8*/)
//{
//	_d("===== mesh::CreateTree(octree=%s, maxTrisPerLeaf=%d) =====", (octree ? "true" : "false"), maxTrisPerLeaf);
//
//	_d("  Clearing tree...");
//	ClearTree();
//
//	if (!points || !indices || num_points == 0 || num_indices == 0)
//		return;
//
//	_d("  Adding points...");
//	for (unsigned int i = 0; i < num_points; ++i)
//		root.aabb.AddPoint(points[i]);
//
//	float minAABBAxisDiff = 0.1f;
//	for (unsigned int i = 0; i < 3; ++i)
//	{
//		float diff;
//		if ((diff = root.aabb.vMax[i] - root.aabb.vMin[i]) < minAABBAxisDiff)
//		{
//			diff = (minAABBAxisDiff - diff) * 0.5f;
//			root.aabb.vMin[i] -= diff;
//			root.aabb.vMax[i] += diff;
//		}
//	}
//
//	// Slightly un-align root aabb so we can avoid perfectly matching node aabbs and triangle edges
//	// for grid patterns.
//	root.aabb.vMax += Vec3f(0.003f, 0.003f, 0.003f);
//	root.aabb.vMin -= Vec3f(0.001f, 0.001f, 0.001f);
//
//	_d("  Inserting %d triangles...", (unsigned int)(num_indices / 3));
//	for (unsigned int itri = 0; itri < num_indices; itri += 3)
//		InsertMeshTreeTriangle(this, itri, maxTrisPerLeaf, octree);
//	_d("  Done.");
//	_d("");
//
//	//DumpMeshTree(this, &root);
//	//_d("");
//
//	unsigned int maxTreeDepth = DetermineMeshMaxTreeDepth(&root);
//	_d("  ===> Max Tree Depth: %d", maxTreeDepth);
//	_d("");
//}


void ClearMeshTreeNode(mesh_tree_node* pnode)
{
	if (!pnode) return;
	if (pnode->children)
	{
		for (unsigned int i = 0; i < pnode->num_children; ++i)
			ClearMeshTreeNode(&pnode->children[i]);

		delete[] pnode->children;
	}

	if (pnode->tris)
		delete[] pnode->tris;

	pnode->aabb.Reset();
	pnode->children = 0;
	pnode->num_children = 0;
	pnode->tris = 0;
	pnode->num_tris = 0;
}

void mesh::ClearTree()
{
	ClearMeshTreeNode(&root);
}


OBB mesh::GetBoundBox() const
{
	OBB obb(root.aabb);
	for (int i = 0; i < 3; ++i)
		if (obb.dimensions[i] < FLT_EPSILON) obb.dimensions[i] = 0.01f;
	obb.Transform(transform);
	return obb;
}

float mesh::GetVolume() const
{
	if (num_points == 0 || num_indices == 0 || !points || !indices)
		return 0.0f;

	Mat33 A;
	Vec3f p[3];
	float V = 0;
	const float ONE_SIXTH = 1.0f / 6.0f;
	for (unsigned int tri = 0; tri < num_indices; tri += 3)
	{
		for (int i = 0; i < 3; ++i)
			p[i] = points[indices[tri + i]];

		A = Mat33::FromColumns(p[0], p[1], p[2]);
		float Vtet = ONE_SIXTH * A.Determinant();
		if (V + Vtet < FLT_EPSILON)
			continue;

		V += Vtet;
	}

	return V;
}

float mesh::GetDistance(const Vec3f & p) const
{


	// TODO



	return 0.0f;
}


bool _MeshNodeShape(const mesh* pmesh, const mesh_tree_node* pnode, box* pnodebox, const shape* pshape, const box* pshapebox, SIntersection* pinters)
{
	// Check against node bounding box
	static SIntersection bbinters;
	static SIntersection tmpinters;
	static triangle tri;
	
	OBB nodeBB(pnode->aabb);
	pnodebox->c = nodeBB.center;
	for (int i = 0; i < 3; ++i)
	{
		pnodebox->axis[i] = nodeBB.directions[i];
		pnodebox->dim[i] = nodeBB.dimensions[i];
	}

	if (pshape->GetType() == eSHAPE_RAY || pshape->GetType() == eSHAPE_PLANE)
	{
		// Intersect infinite shapes directly with bound box
		if (!_Intersection(pnodebox, pshape, &bbinters))
			return false;
	}
	else
	{
		//if (!_Intersection(pnodebox, pshapebox, &bbinters))
		if (!_BoxBoxExists(pnodebox, pshapebox))
			return false;
	}

	//PhysDebug::VisualizeBox(nodeBB, pnode->_color, true);

	bool inters = false;
	if (pnode->children)
	{
		for (unsigned int i = 0; i < pnode->num_children; ++i)
		{
			inters |= _MeshNodeShape(pmesh, &pnode->children[i], pnodebox, pshape, pshapebox, pinters);
		}
	}
	else if (pnode->num_tris > 0 && pnode->tris)
	{
		for (unsigned int i = 0; i < pnode->num_tris; ++i)
		{
			tri.p[0] = pmesh->points[pmesh->indices[pnode->tris[i] + 0]];
			tri.p[1] = pmesh->points[pmesh->indices[pnode->tris[i] + 1]];
			tri.p[2] = pmesh->points[pmesh->indices[pnode->tris[i] + 2]];
			tri.n = ((tri.p[1] - tri.p[0]) ^ (tri.p[2] - tri.p[0])).Normalized();

			//for (int i = 0; i < 3; ++i)
			//	PhysDebug::VisualizeVector(tri.p[i], tri.p[(i + 1) % 3] - tri.p[i], pnode->_color, true);

			//PhysDebug::VisualizeVector(tri.p[0], tri.p[1] - tri.p[0], pnode->_color, true);
			//PhysDebug::VisualizeVector(tri.p[1], tri.p[2] - tri.p[1], pnode->_color, true);
			//PhysDebug::VisualizeVector(tri.p[2], tri.p[0] - tri.p[2], pnode->_color, true);

			// TODO: Actually save all contacts instead finding the minimum here?
			if (!_Intersection(&tri, pshape, &tmpinters))
				continue;

			//PhysDebug::VisualizeVector(tri.p[0], tri.n, pnode->_color, true);

			if (tmpinters.dist < pinters->dist)
			{
				inters = true;
				*pinters = tmpinters;
			}
		}
	}

	return inters;
}

bool _MeshShape(const mesh* pmesh, const shape* pshape, SIntersection* pinters)
{
	box nodebox, shapebox;
	Mat44 invTransform = SMatrixInvert(pmesh->transform);

	shape* ptshape = pshape->Clone();
	if (!ptshape)
		return false;

	if (ptshape->GetType() == eSHAPE_PLANE)
		int a = 0;

	ptshape->Transform(invTransform);

	OBB shapebb = ptshape->GetBoundBox();
	shapebox.c = shapebb.center;
	for (int i = 0; i < 3; ++i)
	{
		shapebox.axis[i] = shapebb.directions[i];
		shapebox.dim[i] = shapebb.dimensions[i];
	}

	pinters->dist = FLT_MAX;

	bool res = _MeshNodeShape(pmesh, &pmesh->root, &nodebox, ptshape, &shapebox, pinters);
	delete ptshape;

	if (res)
	{
		pinters->p = (pmesh->transform * Vec4f(pinters->p, 1.0f)).xyz();
		pinters->n = (pmesh->transform * Vec4f(pinters->n, 0.0f)).xyz();
	}

	return res;
}

bool _ShapeMesh(const shape* pshape, const mesh* pmesh, SIntersection *pinters)
{
	bool res = _MeshShape(pmesh, pshape, pinters);
	pinters->n *= -1.0f;
	return res;
}

GEO_NMSPACE_END

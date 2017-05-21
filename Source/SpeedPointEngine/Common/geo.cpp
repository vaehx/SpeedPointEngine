#include "geo.h"
#include <Physics\Implementation\PhysDebug.h> // TODO: Get this out of here.

GEO_NMSPACE_BEG

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

	_intersectionTestTable[eSHAPE_PLANE][eSHAPE_RAY] = (_IntersectionTestFnPtr)&_PlaneRay;
	_intersectionTestTable[eSHAPE_PLANE][eSHAPE_PLANE] = (_IntersectionTestFnPtr)&_PlanePlane;
	_intersectionTestTable[eSHAPE_PLANE][eSHAPE_SPHERE] = (_IntersectionTestFnPtr)&_PlaneSphere;
	_intersectionTestTable[eSHAPE_PLANE][eSHAPE_CYLINDER] = 0;
	_intersectionTestTable[eSHAPE_PLANE][eSHAPE_CAPSULE] = (_IntersectionTestFnPtr)&_PlaneCapsule;
	_intersectionTestTable[eSHAPE_PLANE][eSHAPE_TRIANGLE] = 0;

	_intersectionTestTable[eSHAPE_SPHERE][eSHAPE_RAY] = (_IntersectionTestFnPtr)&_SphereRay;
	_intersectionTestTable[eSHAPE_SPHERE][eSHAPE_PLANE] = (_IntersectionTestFnPtr)&_SpherePlane;
	_intersectionTestTable[eSHAPE_SPHERE][eSHAPE_SPHERE] = (_IntersectionTestFnPtr)&_SphereSphere;
	_intersectionTestTable[eSHAPE_SPHERE][eSHAPE_CYLINDER] = (_IntersectionTestFnPtr)&_SphereCylinder;
	_intersectionTestTable[eSHAPE_SPHERE][eSHAPE_CAPSULE] = (_IntersectionTestFnPtr)&_SphereCapsule;
	_intersectionTestTable[eSHAPE_SPHERE][eSHAPE_TRIANGLE] = 0;

	_intersectionTestTable[eSHAPE_CYLINDER][eSHAPE_RAY] = (_IntersectionTestFnPtr)&_CylinderRay;
	_intersectionTestTable[eSHAPE_CYLINDER][eSHAPE_PLANE] = 0;
	_intersectionTestTable[eSHAPE_CYLINDER][eSHAPE_SPHERE] = (_IntersectionTestFnPtr)&_CylinderSphere;
	_intersectionTestTable[eSHAPE_CYLINDER][eSHAPE_CYLINDER] = (_IntersectionTestFnPtr)&_CylinderCylinder;
	_intersectionTestTable[eSHAPE_CYLINDER][eSHAPE_CAPSULE] = 0;
	_intersectionTestTable[eSHAPE_CYLINDER][eSHAPE_TRIANGLE] = 0;

	_intersectionTestTable[eSHAPE_CAPSULE][eSHAPE_RAY] = (_IntersectionTestFnPtr)&_CapsuleRay;
	_intersectionTestTable[eSHAPE_CAPSULE][eSHAPE_PLANE] = (_IntersectionTestFnPtr)&_CapsulePlane;
	_intersectionTestTable[eSHAPE_CAPSULE][eSHAPE_SPHERE] = (_IntersectionTestFnPtr)&_CapsuleSphere;
	_intersectionTestTable[eSHAPE_CAPSULE][eSHAPE_CYLINDER] = 0;
	_intersectionTestTable[eSHAPE_CAPSULE][eSHAPE_CAPSULE] = (_IntersectionTestFnPtr)&_CapsuleCapsule;
	_intersectionTestTable[eSHAPE_CAPSULE][eSHAPE_TRIANGLE] = 0;

	_intersectionTestTable[eSHAPE_TRIANGLE][eSHAPE_RAY] = (_IntersectionTestFnPtr)&_TriangleRay;
	_intersectionTestTable[eSHAPE_TRIANGLE][eSHAPE_PLANE] = 0;
	_intersectionTestTable[eSHAPE_TRIANGLE][eSHAPE_SPHERE] = 0;
	_intersectionTestTable[eSHAPE_TRIANGLE][eSHAPE_CYLINDER] = 0;
	_intersectionTestTable[eSHAPE_TRIANGLE][eSHAPE_CAPSULE] = 0;
	_intersectionTestTable[eSHAPE_TRIANGLE][eSHAPE_TRIANGLE] = 0;

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
	return OBB(AABB(Vec3f(-FLT_MAX), Vec3f(FLT_MAX)));
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

bool _PlaneTriangle(const plane* pplane, const triangle* ptri, SIntersection* pinters)
{
	Vec3f q[3]; // intersection points
	float t, den;
	int n = 0;
	for (int i = 0; i < 3; ++i)
	{
		den = Vec3Dot(ptri->p[(i + 1) % 3] - ptri->p[i], pplane->n);
		if (true)
		{
			t = (den) ? Vec3Dot(pplane->n * pplane->d - ptri->p[i], pplane->n) / den : 0.0f;
			if (t >= 0.0f && t <= 1.0f)
				q[n++] = ptri->p[i] + t * (ptri->p[(i + 1) % 3] - ptri->p[i]);
		}
	}

	if (n < 2)
		return false;

	if (n == 3)
		pinters->p = (q[0] + q[1] + q[2]) * ONE_THIRD;
	else
		pinters->p = (q[0] + q[1]) * 0.5f;
	pinters->n = pplane->n;
	pinters->dist = 0.0f;
	pinters->feature = eINTERSECTION_FEATURE_BASE_SHAPE;
	return true;
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
	obb.dimensions = Vec3f(r, r, r);
	return obb;
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
	for (i = 0; i < 3; ++i)
		inters &= (Vec3Dot((ptri->p[(i + 1) % 3] - ptri->p[i]) ^ ptri->n, psphere->c - ptri->p[i]) <= 0.0f);

	if (inters)
	{
		pinters->feature = eINTERSECTION_FEATURE_BASE_SHAPE;
		pinters->dist = (psphere->c - fp).Length() - psphere->r;
		pinters->p = psphere->c - ptri->n * psphere->r;
		pinters->n = ptri->n;
		return true;
	}

	// Intersect with triangle vertices
	float d, dmin = FLT_MAX;
	Vec3f pmin; // closest point on triangle to sphere
	for (i = 0; i < 3; ++i)
	{
		// Vertex
		if ((d = (psphere->c - ptri->p[i]).LengthSq()) <= rsq && d < dmin)
		{
			inters = true;
			dmin = d;
			pmin = ptri->p[i];
		}

		// Edge
		Vec3f ev = ptri->p[(i + 1) % 3] - ptri->p[i];
		float ev_ln = ev.Length();
		ev /= ev_ln;
		float ed = Vec3Dot(psphere->c - ptri->p[i], ev);
		Vec3f ep = ptri->p[i] + ed * ev;
		if (ed > 0.0f && ed < ev_ln && (d = (psphere->c - ep).LengthSq()) <= rsq && d < dmin)
		{
			inters = true;
			dmin = d;
			pmin = ep;
		}
	}

	if (inters)
	{
		pinters->feature = eINTERSECTION_FEATURE_CAP;
		pinters->dist = sqrtf(dmin) - psphere->r;
		pinters->n = ptri->n;
		pinters->p = psphere->c + (pmin - psphere->c).Normalized() * psphere->r;
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
	bb.dimensions = Vec3f(r, axisln * 0.5f, r);
	return bb;
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
	bb.dimensions = Vec3f(r, hh + r, r);
	return bb;
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
	float s[2];

	// Cap - Capsule
	pinters->dist = FLT_MAX;
	pinters->feature = eINTERSECTION_FEATURE_CAP;
	float distsq;
	Vec3f cp;
	for (int i = 0; i < 2; ++i)
		for (int c = 0; c < 2; ++c)
		{
			s[i ^ 1] = Vec3Dot((cp = (capsules[i]->c + (s[i] = (-1.0f + 2.0f * (float)c)) * capsules[i]->hh) * capsules[i]->axis) - capsules[i ^ 1]->c, capsules[i ^ 1]->axis);
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
		& isneg(Vec3Dot(dc, d) - capsules[0]->r - capsules[1]->r)
		& isneg(fabsf(s[0] = Vec3Dot(dc ^ capsules[1]->axis, d)) - capsules[0]->hh * dlnsq))
	{
		inters = 1;
		pinters->feature = eINTERSECTION_FEATURE_BASE_SHAPE;
		pinters->p = capsules[0]->c + s[0] * capsules[0]->axis;
		pinters->n = d; // wrong sign possible!!!!!!
		pinters->dist = sqr(Vec3Dot(dc, d)) / dlnsq;
	}

	pinters->dist = sqrtf(pinters->dist) - pcapsule1->r - pcapsule2->r;
	pinters->n = pinters->n.Normalized();
	pinters->p += pinters->n * capsules[0]->r;
	return inters;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Triangle
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
	return _SphereTriangle(psphere, ptri, pinters);
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
	bb.dimensions.x = dim[0];
	bb.dimensions.y = dim[1];
	bb.dimensions.z = dim[2];
	bb.directions[0] = axis[0];
	bb.directions[1] = axis[1];
	bb.directions[2] = axis[2];
	return bb;
}

bool _BoxBox(const box* pbox1, const box* pbox2, SIntersection* pinters)
{
	const box* boxes[] = { pbox1, pbox2 }, *box1, *box2;
	Vec3f n[3], ce[3], points[4], pp, pv;
	for (int box = 0; box < 2; ++box)
	{
		box1 = boxes[box]; box2 = boxes[box ^ 1];
		Vec3f dc = box2->c - box1->c; // from box1 to box2

		for (int i = 0; i < 3; ++i)
			n[i] = box1->axis[i] * sgnnz(Vec3Dot(box1->axis[i], dc));
		for (int i = 0; i < 3; ++i)
			ce[i] = box2->axis[i] * sgnnz(Vec3Dot(box2->axis[i], dc));

		Vec3f cp = box2->c - ce[0] * box2->dim[0] - ce[1] * box2->dim[1] - ce[2] * box2->dim[2];

		// Test intersection of the 3 edges vs the 3 closest planes.
		// This results in a maximum of 2 * 3 * 3 = 18 Tests
		quotient t;
		float tt;
		int npoints = 1, s0 = -1;
		points[0] = cp;
		for (int e = 0; e < 3; ++e) // edge
			for (int s = 0; s < 3; ++s) // side
			{
				pp = box1->c + n[s] * box1->dim[s];
				t.set(Vec3Dot(pp - cp, n[s]), Vec3Dot(ce[e], n[s]));
				tt = t.val();
				if (fabsf(t.d) >= FLT_EPSILON && tt >= 0.0f && tt <= box2->dim[e])
				{
					// Check if intersection point inside box side plane limits
					pv = (cp + ce[e] * tt) - pp;
					if ((fabsf(Vec3Dot(pv, n[(s + 1) % 3])) <= box1->dim[(s + 1) % 3]) &
						(fabsf(Vec3Dot(pv, n[(s + 2) % 3])) <= box1->dim[(s + 2) % 3]))
					{
						s0 = (s0 < 0 ? s : s0);
						points[npoints++] = cp + ce[e] * tt;
						break;
					}
				}
			}

		if (npoints > 1)
		{
			pinters->feature = eINTERSECTION_FEATURE_BASE_SHAPE;
			pinters->n = n[s0];
			pinters->dist = 0.0f;
			pinters->p = points[1];
			pp = box1->c + n[s0] * box1->dim[s0];
			float d;
			for (int i = 0; i < npoints; ++i)
			{
				if ((d = Vec3Dot(points[i] - pp, n[s0])) < pinters->dist)
					pinters->dist = d;
			}
			return true;
		}
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Mesh
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool _MeshNodeShape(const mesh* pmesh, const mesh_tree_node* pnode, const shape* pshape, SIntersection* pinters)
{
	if (pshape->GetType() == eSHAPE_RAY)
	{
		ray* pray = (ray*)pshape;
		if (!pnode->aabb.HitsRay(pray->v, pray->p))
			return false;
	}
	else
	{


		// TODO: Test shape Bound box vs node bound box


	}

	if (pnode->children)
	{
		for (int i = 0; i < pnode->num_children; ++i)
		{
			if (_MeshNodeShape(pmesh, &pnode->children[i], pshape, pinters))
				return true;
		}
	}
	else if (pnode->num_tris > 0 && pnode->tris)
	{
		triangle tri;
		for (int i = 0; i < pnode->num_tris; ++i)
		{
			tri.p[0] = pmesh->points[pmesh->indices[pnode->tris[i] + 0]];
			tri.p[1] = pmesh->points[pmesh->indices[pnode->tris[i] + 1]];
			tri.p[2] = pmesh->points[pmesh->indices[pnode->tris[i] + 2]];
			tri.n = (tri.p[1] - tri.p[0]) ^ (tri.p[2] - tri.p[0]);
			if (_Intersection(pshape, &tri, pinters))
				return true;
		}
	}

	return false;
}

bool _MeshShape(const mesh* pmesh, const shape* pshape, SIntersection* pinters)
{
	return false;
}

GEO_NMSPACE_END

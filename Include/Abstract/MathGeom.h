//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2016, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	MathGeom.h
// Created:	11/18/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <SPrerequisites.h>
#include "Vector3.h"
#include "Matrix.h"
#include "Quaternion.h"
#include "BoundBox.h"

#include "Math.h"
#define FLOAT_TOLERANCE FLT_EPSILON

#ifdef _DEBUG
#include <string>
#endif

using std::vector;

SP_NMSPACE_BEG

struct SMesh;



// hd = OBB half dim, c = OBB Center, p = ray point, v = ray direction

/*

	x: Written, but not implemented yet
	Y: Written and tested successfully

	|  PNT	|  RAY	| PLANE	|  SPH	|  CYL	|  CAP	|  BOX	|  TRI	|  Mesh
================================================================================
PNT	|!!!!!!!|  Y	|   Y	|   Y	|	|   Y	|   Y	|   x	|
--------------------------------------------------------------------------------
RAY	|#######|  Y	|   Y	|	|   Y	|   Y	|   Y	|	|	
--------------------------------------------------------------------------------
PLANE	|#######|#######|	|   x	|	|   x	|	|	|	
--------------------------------------------------------------------------------
SPH	|#######|#######|#######|   Y	|	|	|	|   x	|	
--------------------------------------------------------------------------------
CYL	|#######|#######|#######|########|	|	|	|	|	
--------------------------------------------------------------------------------
CAP	|#######|#######|#######|########|######|	|	|   x	|	
--------------------------------------------------------------------------------
Box	|#######|#######|#######|########|######|#######|	|	|	
--------------------------------------------------------------------------------
TRI	|#######|#######|#######|########|######|#######|#######|	|	
--------------------------------------------------------------------------------
Mesh	|#######|#######|#######|########|######|#######|#######|#######|	


(use 8-space tabs for table)


*/


enum EGeomShapeType
{
	// DO NOT CHANGE THE VALUES - THEY ARE SERIALIZED !!!

	eGEOMSHAPE_UNKNOWN = 0x00,
	eGEOMSHAPE_RAY = 0x01,
	eGEOMSHAPE_LINE_SEGMENT = 0x02,
	eGEOMSHAPE_CAPSULE = 0x03,
	eGEOMSHAPE_CYLINDER = 0x04,
	eGEOMSHAPE_SPHERE = 0x05,
	eGEOMSHAPE_MESH = 0x06,
	eGEOMSHAPE_PLANE = 0x07,
	eGEOMSHAPE_BOX = 0x08
};

inline const char* GetGeomShapeTypeName(EGeomShapeType type)
{
	switch (type)
	{	
	case eGEOMSHAPE_RAY: return "GEOMSHAPE_RAY";
	case eGEOMSHAPE_LINE_SEGMENT: return "GEOMSHAPE_LINE_SEGMENT";
	case eGEOMSHAPE_CAPSULE: return "GEOMSHAPE_CAPSULE";
	case eGEOMSHAPE_CYLINDER: return "GEOMSHAPE_CYLINDER";
	case eGEOMSHAPE_SPHERE: return "GEOMSHAPE_SPHERE";
	case eGEOMSHAPE_MESH: return "GEOMSHAPE_MESH";
	case eGEOMSHAPE_PLANE: return "GEOMSHAPE_PLANE";
	case eGEOMSHAPE_BOX: return "GEOMSHAPE_BOX";
	case eGEOMSHAPE_UNKNOWN:
	default:
		return "GEOMSHAPE_UNKNOWN";
	}
}

// Generalisation of a geometrical shape.
// This allows to pack all kinds of shapes into one structure.
// To access, you can use the conversion constructors of the actual shape classes.
//
// Ray:			(p = v[0], v = v[1])
// Line-Segment:	(v1 = v[0], v2 = v[1])
// Capsule:		(p1 = v[0], p2 = v[1], radius = f)
// Cylinder:		(p1 = v[0], p2 = v[1], r = f)
// Sphere:		(center = v[0], radius = f)
// Mesh:		*pMesh
// Plane:		(n = v[0], d = f)
// Box:			(center = v[0], hd = (v[1], v[2], v[3]))
struct SGeomShape
{
	union
	{
		struct
		{
			Vec3f v0, v1, v2;
			float f, g, h;	
		};

		Vec3f v[4];
	};

	const SMesh* pMesh; // only set if type == eGEOMSHAPE_MESH, otherwise 0

	EGeomShapeType type;

	SGeomShape()
		: // vectors zeroed by their constructor
		f(0), g(0), h(0),
		pMesh(0),
		type(eGEOMSHAPE_UNKNOWN)
	{
	}

	SGeomShape(const Vec3f& _v0, const Vec3f& _v1, const Vec3f& _v2, const Vec3f& _v3, EGeomShapeType t)
	{
		v[0] = _v0;
		v[1] = _v1;
		v[2] = _v2;
		v[3] = _v3;
		type = t;
	}

	SGeomShape(const Vec3f& _v0, const Vec3f& _v1, const Vec3f& _v2, float _f, float _g, float _h, EGeomShapeType t)
		: v0(_v0), v1(_v1), v2(_v2),
		f(_f), g(_g), h(_h),
		type(t)
	{
	}

	SGeomShape(const SGeomShape& shape)
	{
		for (int i = 0; i < 4; ++i)
			v[i] = shape.v[i];

		pMesh = shape.pMesh;
		type = shape.type;
	}

	inline SGeomShape& Translate(const Vec3f& v);
};




////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//					P o i n t
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef S_API Vec3f SPoint;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//					 R a y
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct S_API SRay
{
	Vec3f p; // start point
	Vec3f v; // direction

	SRay() : p(0, 0, 0), v(0, 0, 0) {}
	SRay(const SRay& rhs) : p(rhs.p), v(rhs.v) {}
	SRay(const Vec3f& pp, const Vec3f& vv) : p(pp), v(vv) {}

	// p = v[0], v = v[1]
	SRay(const SGeomShape& shape) : p(shape.v[0]), v(shape.v[1]) {}

	inline operator SGeomShape() const
	{
		return SGeomShape(p, v, 0, 0, eGEOMSHAPE_RAY);
	}

	// p = p1, v = p2 - p1
	ILINE static SRay FromPoints(const Vec3f& p1, const Vec3f& p2)
	{
		return SRay(p1, p2 - p1);
	}

	ILINE Vec3f GetPoint(float t) const
	{
		return p + t * v;
	}
};

ILINE float GeomDistance(const SRay& ray1, const SRay& ray2)
{
	Vec3f v1xv2 = Vec3Cross(ray1.v, ray2.v);
	float d = v1xv2.Length();
	if (float_equal(d, 0))
		return 0; // parallel

	return float_abs(Vec3Dot(ray1.p - ray2.p, v1xv2)) / d;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//				   L i n e     S e g m e n t
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


struct SLineSegment
{
	Vec3f v1, v2;

	SLineSegment() {}
	SLineSegment(const SLineSegment& o) : v1(o.v1), v2(o.v2) {}
	SLineSegment(const Vec3f& _v1, const Vec3f& _v2) : v1(_v1), v2(_v2) {}

	// v1 = v[0], v2 = v[1]
	SLineSegment(const SGeomShape& shape) : v1(shape.v[0]), v2(shape.v[1]) {}

	inline operator SGeomShape() const
	{
		return SGeomShape(v1, v2, 0, 0, eGEOMSHAPE_LINE_SEGMENT);
	}
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//					S p h e r e
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct S_API SSphere
{
	Vec3f center;
	float radius;

	SSphere()
		: radius(1.0)
	{
	}

	SSphere(const SSphere& s)
		: radius(s.radius),
		center(s.center)
	{
	}

	SSphere(const Vec3f& c, const float r)
		: center(c),
		radius(r)
	{
	}

	// center = v[0], radius = f
	SSphere(const SGeomShape& shape)
		: center(shape.v[0]),
		radius(shape.f)
	{
	}

	inline operator SGeomShape() const
	{
		return SGeomShape(center, 0, 0, radius, 0, 0, eGEOMSHAPE_SPHERE);
	}
};






////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//					P l a n e
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// SpeedPoint Plane
struct S_API SPlane
{
	Vec3f n;
	float d;
	
	SPlane() {}
	SPlane(const SPlane& p) : n(p.n), d(p.d) {}
	SPlane(const float a, const float b, const float c, const float dd) : n(a, b, c), d(dd) {}	
	SPlane(const Vec3f& nn, float dd) : n(nn), d(dd) {}

	// n = v[0], d = f
	SPlane(const SGeomShape& shape) : n(shape.v[0]), d(shape.f) {}

	inline operator SGeomShape() const
	{
		return SGeomShape(n, 0, 0, d, 0, 0, eGEOMSHAPE_PLANE);
	}
	
	ILINE SPlane& operator = (const SPlane& p)
	{
		n = p.n;
		d = p.d;
		return *this;
	}

	// normal = cross(b - a, c - a)
	ILINE static SPlane FromPoints(const Vec3f& a, const Vec3f& b, const Vec3f& c)
	{
		SPlane plane;
		plane.n = Vec3Cross(b - a, c - a).Normalized();
		plane.d = Vec3Dot(plane.n, a);
		return plane;
	}

	// Creates a horizontal plane with y=height
	ILINE static SPlane FromHeight(float h)
	{
		return SPlane(0, 1.0f, 0, h);
	}

	ILINE static SPlane FromNormalAndPoint(const Vec3f& normal, const Vec3f& point)
	{
		SPlane plane;
		plane.n = normal;
		plane.d = Vec3Dot(normal, point);
		return plane;
	}
};

ILINE SPlane PlaneNormalize(const SPlane& p)
{
	const float fLengthInv = 1.0f / p.n.Length();
	return SPlane(p.n * fLengthInv, p.d * fLengthInv);
}

ILINE SPlane PlaneFromPoints(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3)
{
	return SPlane::FromPoints(v1, v2, v3);
}


// Returns false if the plane and the ray are parallel
ILINE bool GeomIntersects(const SPlane& plane, const SRay& ray, Vec3f* ipoint)
{
	float denominator = Vec3Dot(plane.n, ray.v);

	// check if plane normal and ray perpendicular
	if (float_equal(denominator, 0))
		return false;
	
	if (ipoint)
	{
		float s = (plane.d - Vec3Dot(plane.n, ray.p)) / denominator;
		*ipoint = ray.p + s * ray.v;
	}

	return true;
}






////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//					V i e w   F r u s t u m
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum EFrustumPlane
{
	eFRUSTUMPLANE_LEFT = 0,
	eFRUSTUMPLANE_RIGHT,
	eFRUSTUMPLANE_BOTTOM,
	eFRUSTUMPLANE_TOP,
	eFRUSTUMPLANE_NEAR,
	eFRUSTUMPLANE_FAR
};

struct S_API SViewFrustum
{
	SPlane planes[6];

	SViewFrustum() {};
	SViewFrustum(const SViewFrustum& frustum)
	{
		for (unsigned int i = 0; i < 6; ++i)
			planes[i] = frustum.planes[i];
	}

	// If vp equals the combined view and projection matrices, the frustum planes will be the clipping planes in world space.	
	ILINE SViewFrustum& BuildFromViewProjMatrix(const SMatrix4& vp)
	{
		planes[eFRUSTUMPLANE_LEFT  ] = SPlane(vp._14 + vp._11,	vp._24 + vp._21,	vp._34 + vp._31,	vp._44 + vp._41);
		planes[eFRUSTUMPLANE_RIGHT ] = SPlane(vp._14 - vp._11,	vp._24 - vp._21,	vp._34 - vp._31,	vp._44 - vp._41);
		planes[eFRUSTUMPLANE_BOTTOM] = SPlane(vp._14 + vp._12,	vp._24 + vp._22,	vp._34 + vp._32,	vp._44 + vp._42);
		planes[eFRUSTUMPLANE_TOP   ] = SPlane(vp._14 - vp._12,	vp._24 - vp._22,	vp._34 - vp._32,	vp._44 - vp._42);
		planes[eFRUSTUMPLANE_NEAR  ] = SPlane(vp._13,		vp._23,			vp._33,			vp._43);
		planes[eFRUSTUMPLANE_FAR   ] = SPlane(vp._14 - vp._13,	vp._24 - vp._23,	vp._34 - vp._33,	vp._44 - vp._43);
		
		return *this;
	}
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//					O r i e n t e d    B o u n d b o x
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Oriented Bound Box (OBB)
struct S_API SBox
{
	Vec3f c; // Center
	Vec3f hd[3]; // Half-Dimension vectors, also specifying the orientation of the box
	SPlane planes[6]; // is ordered: 0-1: x-Planes; 2-3: y-Planes; 4-5: zPlanes

	SBox() {}
	SBox(const Vec3f& center, const Vec3f& halfDimXAxis, const Vec3f& halfDimYAxis, const Vec3f& halfDimZAxis)
		: c(center)
	{
		hd[0] = halfDimXAxis;
		hd[1] = halfDimYAxis;
		hd[2] = halfDimZAxis;
		CalculatePlanePairs();
	}

	// c = v[0], hd = { v[1], v[2], v[3] }
	SBox(const SGeomShape& shape)
		: c(shape.v[0])
	{
		hd[0] = shape.v[1];
		hd[1] = shape.v[2];
		hd[2] = shape.v[3];

		// CalculatePlanePairs() ???
	}

	inline operator SGeomShape() const
	{
		return SGeomShape(c, hd[0], hd[1], hd[2], eGEOMSHAPE_BOX);
	}

	// points 0-4 will be (in clockwise order) the vertices of the local +x side, starting at local (+x,+y,+z)
	inline void ComputeVertices(SPoint points[8]) const
	{
		points[0] = c + hd[0] + hd[1] + hd[2];
		points[1] = c + hd[0] + hd[1] - hd[2];
		points[2] = c + hd[0] - hd[1] - hd[2];
		points[3] = c + hd[0] - hd[1] + hd[2];

		const Vec3f doubleXDim = 2.0f * hd[0];
		for (unsigned char i = 4; i < 8; ++i)
			points[i] = points[i - 4] - doubleXDim;
	}

	inline void CalculatePlanePairs()
	{
		SPoint points[8];
		ComputeVertices(points);

		// x-Planes
		planes[0] = SPlane::FromPoints(points[0], points[3], points[1]);
		planes[1] = SPlane::FromPoints(points[7], points[4], points[6]);

		// y-Planes
		planes[2] = SPlane::FromPoints(points[0], points[1], points[4]);
		planes[3] = SPlane::FromPoints(points[6], points[2], points[7]);

		// z-Planes
		planes[4] = SPlane::FromPoints(points[7], points[3], points[4]);
		planes[5] = SPlane::FromPoints(points[1], points[2], points[5]);
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////

struct SMeshVertex
{
	float x, y, z;
	float nx, ny, nz;

	SMeshVertex()
	{
	}

	SMeshVertex(const SMeshVertex& v)
		: x(v.x), y(v.y), z(v.z),
		nx(v.nx), ny(v.ny), nz(v.nz)
	{
	}

	inline SMeshVertex& operator =(const SMeshVertex& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		nx = v.nx;
		ny = v.ny;
		nz = v.nz;
		return *this;
	}
};

#define MESHVERTEX_TO_VEC3F(vtx) Vec3f(vtx.x, vtx.y, vtx.z)

struct SMeshEdge
{
	unsigned long vtx1, vtx2;
};

// Each MeshFace has an id that we can use to quickly compare two faces for equivalence in single mesh
struct SMeshFace
{
	SMeshVertex vtx[3];
	u16 id;	// max 65,536 faces

	SMeshFace() {}
	SMeshFace(const SMeshFace& face) : id(face.id)
	{
		for (int i = 0; i < 3; ++i)
			vtx[i] = face.vtx[i];
	}

	SMeshFace(const SMeshVertex& vtx1, const SMeshVertex& vtx2, const SMeshVertex& vtx3)
	{
		vtx[0] = vtx1;
		vtx[1] = vtx2;
		vtx[2] = vtx3;
	}
};



enum EMeshKTreeType
{
	eMESH_KTREE_QUADTREE,
	eMESH_KTREE_OCTREE	
};

#define MESH_KTREE_CHILDS_COUNT(type) (type == eMESH_KTREE_QUADTREE ? 4 : 8)

// Maximum number of childs that are possible by the specification of EMeshKTreeType
#define MESH_KTREE_MAX_CHILDS 8

// Intersection testing acceleration structure for a collection of SMeshFace's
struct SMeshKTree
{
	vector<SMeshFace> faces;	// Assigned faces to this k-tree

	EMeshKTreeType type;
	unsigned int nChilds;
	SMeshKTree* pChilds;

	AABB aabb;

	SMeshKTree()
		: pChilds(0),
		nChilds(0)
	{
	}

	~SMeshKTree()
	{
		Clear();
	}

	void Clear()
	{
		if (IS_VALID_PTR(pChilds))
		{
			for (unsigned int i = 0; i < nChilds; ++i)
				pChilds[i].Clear();

			delete[] pChilds;
		}

		pChilds = 0;
		nChilds = 0;
	}

	// Fills the kTree with Faces.
	// Will clear previous data
	// maxDepth - 0 means, this kTree is a leaf, i.e. no childs are added. 	
	// pFaceIndices - List of indices of faces in the insertFaces array that intersect with aabb
	inline void Init(const vector<SMeshFace>& insertFaces, const AABB& _aabb, const vector<u16>& faceIndices, EMeshKTreeType _type = eMESH_KTREE_OCTREE, unsigned int maxDepth = 0)
	{
		Clear();
		faces.clear();
		aabb = _aabb;

		type = _type;				

		if (maxDepth > 0)
		{
			Vec3f vMin = aabb.vMin;
			Vec3f vMax = aabb.vMax;
			Vec3f vCenter = vMin + (vMax - vMin) * 0.5f;			

			unsigned int k = MESH_KTREE_CHILDS_COUNT(type);

			//
			//
			// TODO: Check if stack overflow is possible !!!!! - Stack is much faster though!
			//
			//
			AABB *pChildAABBs = new AABB[MESH_KTREE_MAX_CHILDS];

			switch (type)
			{
			case eMESH_KTREE_QUADTREE:
				pChildAABBs[0] = AABB(vMin, Vec3f(vCenter.x, vMax.y, vCenter.z));
				pChildAABBs[1] = AABB(Vec3f(vCenter.x, vMin.y, vMin.z), Vec3f(vMax.x, vMax.y, vCenter.z));
				pChildAABBs[2] = AABB(Vec3f(vCenter.x, vMin.y, vCenter.z), Vec3f(vMax.x, vMax.y, vMax.z));
				pChildAABBs[3] = AABB(Vec3f(vMin.x, vMin.y, vCenter.z), Vec3f(vCenter.x, vMax.y, vMax.z));
				break;

			case eMESH_KTREE_OCTREE:
				pChildAABBs[0] = AABB(vMin, vCenter);
				pChildAABBs[1] = AABB(Vec3f(vMin.x, vMin.y, vCenter.z), Vec3f(vCenter.x, vCenter.y, vMax.z));
				pChildAABBs[2] = AABB(Vec3f(vCenter.x, vMin.y, vCenter.z), Vec3f(vMax.x, vCenter.y, vMax.z));
				pChildAABBs[3] = AABB(Vec3f(vCenter.x, vMin.y, vMin.z), Vec3f(vMax.x, vCenter.y, vCenter.z));
				pChildAABBs[4] = AABB(Vec3f(vMin.x, vCenter.y, vMin.z), Vec3f(vCenter.x, vMax.y, vCenter.z));
				pChildAABBs[5] = AABB(Vec3f(vMin.x, vCenter.y, vCenter.z), Vec3f(vCenter.x, vMax.y, vMax.z));
				pChildAABBs[6] = AABB(Vec3f(vCenter.x, vCenter.y, vCenter.z), Vec3f(vMax.x, vMax.y, vMax.z));
				pChildAABBs[7] = AABB(Vec3f(vCenter.x, vCenter.y, vMin.z), Vec3f(vMax.x, vMax.y, vCenter.z));
				break;
			}			

			// Determine faces per child first. This way, we only create those childs with any face in it.

			//
			//
			// TODO: Check if stack overflow is possible !!!! - Stack is much faster though!
			//
			//
			vector<u16> pChildFaces[MESH_KTREE_MAX_CHILDS];

			unsigned long numIntersections = 0;

			int numFilledChilds = 0;
			for (auto itFaceIndex = faceIndices.begin(); itFaceIndex != faceIndices.end(); ++itFaceIndex)
			{
				const SMeshFace& face = insertFaces[*itFaceIndex];

				// For each child
				for (unsigned int i = 0; i < k; ++i)
				{
					Vec3f v0 = MESHVERTEX_TO_VEC3F(face.vtx[0]);
					Vec3f v1 = MESHVERTEX_TO_VEC3F(face.vtx[1]);
					Vec3f v2 = MESHVERTEX_TO_VEC3F(face.vtx[2]);

					if (pChildAABBs[i].HitsLineSegment(v0, v1) || pChildAABBs[i].HitsLineSegment(v1, v2) || pChildAABBs[i].HitsLineSegment(v2, v0))
					{
						numIntersections++;

						// This is the first face we found, that intersects with this child. So we have another child to create.
						if (pChildFaces[i].size() == 0)
							++numFilledChilds;

						pChildFaces[i].push_back(*itFaceIndex);
					}
				}
			}			

			if (numFilledChilds > 0)
			{
				pChilds = new SMeshKTree[numFilledChilds];
				nChilds = 0; // will be set to the actual number of childs

				for (unsigned int i = 0; i < k; ++i)
				{
					// Do not create the child if there are no faces in it
					if (pChildFaces[i].size() == 0)
						continue;

					pChilds[nChilds].Init(insertFaces, pChildAABBs[i], pChildFaces[i], type, maxDepth - 1);
					nChilds++;
				}
			}

			delete[] pChildAABBs;
			//delete[] pChildFaces;
		}
		else
		{
			faces.reserve(faceIndices.size());			

			// Insert faces
			for (auto itFaceIndex = faceIndices.begin(); itFaceIndex != faceIndices.end(); itFaceIndex++)
			{
				faces.push_back(insertFaces[*itFaceIndex]);
				faces.back().id = *itFaceIndex;
			}
		}
	}

	inline void GetIntersectingLeafs(const AABB& operand, vector<SMeshKTree*>& leafs)
	{
		// Cancel search early, as this is definitely not a subtree in interest
		if (!aabb.Intersects(operand))
			return;

		if (IsLeaf())
		{
			leafs.push_back(this);
			return;
		}

		for (unsigned int i = 0; i < nChilds; ++i)
		{
			pChilds[i].GetIntersectingLeafs(operand, leafs);
		}
	}

	// Fills array with possibly intersecting faces.
	// This is an object-level broadphase algorithm to increase intersection performance between a mesh
	// and any other collision shape.
	//
	// Warning: This function assumes that the id's of the faces are set correctly and are unique!
	inline void GetIntersectingFaces(const AABB& operand, vector<const SMeshFace*>& intersecting, const SMatrix& transform = SMatrix(), vector<u16>& foundIntersecting = vector<u16>()) const
	{
		if (!aabb.Intersects(operand))
			return;

		if (IsLeaf())
		{
			// Assuming 1/4th is intersecting
			foundIntersecting.reserve(foundIntersecting.size() + (unsigned int)((float)faces.size() * 0.25f));

			// Add all new faces to the intersecting array
			vector<SMeshFace>::const_iterator faceEnd = faces.end();
			for (auto& itFace = faces.begin(); itFace != faceEnd; itFace++)
			{
				bool alreadyFound = false;
				vector<u16>::iterator foundEnd = foundIntersecting.end();
				for (auto& itFound = foundIntersecting.begin(); itFound != foundEnd; ++itFound)
				{
					if (*itFound == itFace->id)
					{
						alreadyFound = true;
						break;
					}
				}

				// Add the face if not yet in the intersecting array
				if (!alreadyFound)
				{


					// TODO:
					//
					//	Try if the following approach speeds up the search:
					//		Determine simple AABB from the face and check if it intersects the operand (this check should be fast enough)
					//		If they these AABBs do definitely not intersect, we can avoid copying the face!



					intersecting.push_back(&(*itFace));
					foundIntersecting.push_back(itFace->id);
				}
			}
		}
		else
		{
			for (unsigned int i = 0; i < nChilds; ++i)
			{
				pChilds[i].GetIntersectingFaces(operand, intersecting, transform, foundIntersecting);
			}
		}
	}

	inline bool IsLeaf() const
	{
		return nChilds == 0 || !IS_VALID_PTR(pChilds);
	}

	// Multiplies each vertex with this matrix
	inline void TransformVertices(const SMatrix& matrix)
	{
		if (IsLeaf())
		{
			for (auto itFace = faces.begin(); itFace != faces.end(); ++itFace)
			{
				for (int i = 0; i < 3; ++i)
				{
					Vec4f vec(itFace->vtx[i].x, itFace->vtx[i].y, itFace->vtx[i].z, 1.f);
					vec = matrix * vec;
					itFace->vtx[i].x = vec.x;
					itFace->vtx[i].y = vec.y;
					itFace->vtx[i].z = vec.z;
				}
			}
		}
		else
		{
			if (!IS_VALID_PTR(pChilds))
				return;

			for (unsigned int iChild = 0; iChild < nChilds; ++iChild)
			{
				pChilds[iChild].TransformVertices(matrix);
			}
		}
	}

	// Sets the given minY and maxY as the min/max values for ALL aabb's in the kTree
	inline void UpdateAABBHeights(float minY, float maxY)
	{
		aabb.vMin.y = minY;
		aabb.vMax.y = maxY;

		if (!IsLeaf())
		{
			for (unsigned int iChild = 0; iChild < nChilds; ++iChild)
			{
				pChilds[iChild].UpdateAABBHeights(minY, maxY);
			}
		}
	}
};

// Provides a structure to store a general-purpose mesh.
// Stores faces in a kTree.
struct SMesh
{
	SMeshKTree kTree;
	vector<SMeshFace> faces;

	// maxDepth - 0 means that the root kTree is a single leaf
	void Init(const vector<SMeshFace>& faces, const AABB& aabb, EMeshKTreeType kTreeType = eMESH_KTREE_OCTREE, unsigned int maxDepth = 3)
	{
		// Save all faces for later access
		// TODO: Remove this again - this was just for debugging purposes
		this->faces.insert(this->faces.end(), faces.begin(), faces.end());


		vector<u16> faceIndices;
		faceIndices.resize(faces.size());
		for (u16 i = 0; i < (u16)faces.size(); ++i)
			faceIndices[i] = i;

		CLog::Log(S_DEBUG, "Initializing kTree with %u faces,  maxDepth = %u", faces.size(), maxDepth);
		kTree.Init(faces, aabb, faceIndices, kTreeType, maxDepth);

		CLog::Log(S_DEBUG, "Done. kTree.nChilds = %u    kTree.faces.size() = %u", kTree.nChilds, kTree.faces.size());
	}

	// Fills the intersecting array with faces that intersect with the given AABB
	void GetIntersectingFaces(const AABB& operand, vector<const SMeshFace*>& intersecting, const SMatrix4& transform = SMatrix()) const
	{
		kTree.GetIntersectingFaces(operand, intersecting);
	}

	// relative
	inline SMesh& Translate(const Vec3f& v)
	{
		kTree.TransformVertices(SMatrix::MakeTranslationMatrix(v));
		return *this;
	}

	// relative
	inline SMesh& Rotate(const Quat& q)
	{
		kTree.TransformVertices(q.ToRotationMatrix());
		return *this;
	}

	// relative
	inline SMesh& Scale(const Vec3f& factor)
	{
		kTree.TransformVertices(SMatrix::MakeScaleMatrix(factor));
		return *this;
	}
};






////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//					C y l i n d e r
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct SCylinder
{
	Vec3f p1, p2; // Start and End-Points on the ray
	float r; // radius

	SCylinder() : r(1.0f) {}
	SCylinder(const SCylinder& cyl) : p1(cyl.p1), p2(cyl.p2), r(cyl.r) {}
	SCylinder(const Vec3f& bottom, const Vec3f& top, float radius) : p1(bottom), p2(top), r(radius) {}
	
	// p1  = v[0], p2 = v[1], r = f
	SCylinder(const SGeomShape& shape) : p1(shape.v[0]), p2(shape.v[1]), r(shape.f) {}

	inline operator SGeomShape() const
	{
		return SGeomShape(p1, p2, 0, r, 0, 0, eGEOMSHAPE_CYLINDER);
	}

	// Direction vector from p1 to p2, not normalized
	inline Vec3f GetDirection() const
	{
		return (p2 - p1);
	}

	// relative
	inline SCylinder& Translate(const Vec3f& v)
	{
		p1 += v;
		p2 += v;
		return *this;
	}

	// relative
	inline SCylinder& Rotate(const Quat& q)
	{
		p1 = q * p1;
		p2 = q * p2;
		return *this;
	}

	// relative
	inline SCylinder& Scale(const Vec3f& factor)
	{
		Vec3f mid = (p1 + p2) * 0.5f;
		p1 = (p1 - mid) * factor + mid;
		p2 = (p2 - mid) * factor + mid;
		return *this;
	}
};




////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//					C a p s u l e
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct SCapsule
{
	Vec3f p1, p2; // Start and End-Points on the ray, centers of the cap-spheres
	float r;

	SCapsule()
		: r(0.0f) {}

	SCapsule(const Vec3f& _p1, const Vec3f& _p2, float radius)
		: p1(_p1), p2(_p2), r(radius) {}

	SCapsule(const SCapsule& capsule)
		: p1(capsule.p1), p2(capsule.p2), r(capsule.r) {}

	SCapsule(const SGeomShape& shape)
		: p1(shape.v[0]), p2(shape.v[1]), r(shape.f) {}

	inline operator SGeomShape() const
	{
		return SGeomShape(p1, p2, 0, r, 0, 0, eGEOMSHAPE_CAPSULE);
	}
	
	// relative
	inline SCapsule& Translate(const Vec3f& v)
	{
		p1 += v;
		p2 += v;
		return *this;
	}

	// relative
	inline SCapsule& Rotate(const Quat& q)
	{
		p1 = q * p1;
		p2 = q * p2;
		return *this;
	}

	// relative
	inline SCapsule& Scale(const Vec3f& factor)
	{
		Vec3f mid = (p1 + p2) * 0.5f;
		p1 = (p1 - mid) * factor + mid;
		p2 = (p2 - mid) * factor + mid;
		return *this;
	}
};






/////////////////////////////////////////////////////////////////////////////////////////////////





inline SGeomShape& SGeomShape::Translate(const Vec3f& v)
{
	switch (type)
	{
	case eGEOMSHAPE_CAPSULE:
		*this = (SGeomShape)SCapsule(*this).Translate(v);
		break;
	default:
		break;
	}

	return *this;
}













////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//					I N T E R S E C T I O N S
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


ILINE bool RayHitAABB(const SRay& ray, const AABB& aabb, float* pLength = NULL)
{
	return aabb.HitsRay(ray.v, ray.p, pLength);
}










inline bool IntersectPointOBB(const SBox& obb, const SPoint& point);
inline bool IntersectRayOBB(const SBox& obb, const SRay& ray, float* pMin = 0, float *pMax = 0, Vec3f *pMinNormal = 0, Vec3f *pMaxNormal = 0);
inline bool IntersectRayRay(const SRay& ray1, const SRay& ray2, float* pParam1 = 0, float* pParam2 = 0, float* pDist = 0);

// incidentNormal - Set to true to get the actual incident normal, instead of the normal of the plane. The incident normal
//		calculated by checking on which side the plane start point lies
inline bool IntersectRayPlane(const SRay& ray, const SPlane& plane, float *param = 0, Vec3f* pNormal = 0, bool incidentNormal = false);

// pOutside - Set to true, if the point lies on the side in which the normal points to.
inline bool IntersectPlanePoint(const SPlane& plane, const SPoint& point, bool *pOutside = 0, float* pDistance = 0);

inline bool IntersectPlaneSphere(const SPlane& plane, const SSphere& sphere);
inline bool IntersectPlaneCapsule(const SPlane& plane, const SCapsule& capsule);

inline bool IntersectRayPoint(const SRay& ray, const SPoint& point, float* pDist = 0, Vec3f* pFoot = 0);
inline bool IntersectRaySphere(const SRay& ray, const SSphere& sphere, float* pDist = 0);
inline bool IntersectRayCylinder(const SRay& ray, const SCylinder& cyl);
inline bool IntersectRayCapsule(const SRay& ray, const SCapsule& capsule, float* pRayParam = 0);

// Line: origin + lambda * direction    with  0 <= lambda <= maxLambda
inline bool IntersectLineSphere(const SRay& ray, const float maxLambda, const SSphere& sphere);

// pDist is set the to minimum distance of the spheres from surface to surface
// If pDist is negative, this indicates, that the spheres inter-penetrate.
inline bool IntersectSphereSphere(const SSphere& sphere1, const SSphere& sphere2, float* pDist = 0);

// pDist is set to the minimum distance of the point to the surface of the sphere
// If pDist is negativ, this indicates, that the point inter-penetrates the sphere.
inline bool IntersectSpherePoint(const SSphere& sphere, const SPoint& point, float *pDist = 0);

// pDistance is the distance of the point to the plane that corresponds to the triangle
inline bool IntersectTrianglePoint(const SMeshVertex& vtx1, const SMeshVertex& vtx2, const SMeshVertex& vtx3, const SPoint& point, bool* bOutside = 0, float* pDistance = 0);

inline bool IntersectTriangleSphere(const SMeshVertex& vtx1, const SMeshVertex& vtx2, const SMeshVertex& vtx3, const SSphere& sphere, bool* bOutside = 0);

inline bool IntersectLineTriangle(const SLineSegment& line, const SMeshVertex& vtx1, const SMeshVertex& vtx2, const SMeshVertex& vtx3, Vec3f* pIntersection = 0, Vec3f* pNormal = 0);














// -------------------------------------------------------------------------------------------
//
//
//
//			U T I L I T I E S
//
//
//
// -------------------------------------------------------------------------------------------

inline Vec3f GetFootOnPlane(const SPlane& plane, const SPoint& point)
{
	float nLength = plane.n.Length();
	if (fabsf(nLength) <= FLOAT_TOLERANCE)
		return Vec3f();

	const Vec3f P(point.x, point.y, point.z);
	return (Vec3Dot(plane.n, P) - plane.d) / nLength;
}

// pX1, pX2 -  closest points to each other on line1 / line2 respectively
inline float GetMinLineSegmentDistanceSq(const SLineSegment& l1, const SLineSegment& l2, Vec3f* pX1 = 0, Vec3f* pX2 = 0)
{
	// Calculate lengths of line segments
	float length1 = (l1.v2 - l1.v1).Length();
	float length2 = (l2.v2 - l2.v1).Length();

	// Define the rays
	SRay ray1, ray2;
	ray1.p = l1.v1;
	ray1.v = (l1.v2 - l1.v1) / length1;

	ray2.p = l2.v1;
	ray2.v = (l2.v2 - l2.v1) / length2;

	float t1, t2;	
	IntersectRayRay(ray1, ray2, &t1, &t2);

	Vec3f x1, x2;
	if (t1 < 0.0f)
		x1 = l1.v1;
	else if (t1 > length1)
		x1 = l1.v2;
	else
		x1 = ray1.GetPoint(t1);

	if (t2 < 0.0f)
		x2 = l2.v1;
	else if (t2 > length2)
		x2 = l2.v2;
	else
		x2 = ray2.GetPoint(t2);

	if (IS_VALID_PTR(pX1)) *pX1 = x1;
	if (IS_VALID_PTR(pX2)) *pX2 = x2;

	return (x2 - x1).LengthSq();
}

inline Vec3f GetRayPointDistanceVec(const Vec3f& origin, const Vec3f& dir, const SPoint& point)
{
	Vec3f origin2pnt = point - origin;
	return Vec3Dot(origin2pnt, dir) * dir - origin2pnt;
}

inline float GetRayPointDistanceSq(const Vec3f& origin, const Vec3f& dir, const SPoint& point)
{
	Vec3f origin2pnt = point - origin;
	Vec3f distVec = Vec3Dot(origin2pnt, dir) * dir - origin2pnt;
	return distVec.LengthSq();
}

// Returns the minimum distance of a point to a line segment
inline float GetRayPointDistance(const Vec3f& origin, const Vec3f& dir, const SPoint& point)
{
	// Vector from ray origin to the point
	Vec3f origin2pnt = point - origin;

	// Project origin2pnt onto line vector and determine vector between result and origin2pnt
	Vec3f distVec = Vec3Dot(origin2pnt, dir) * dir - origin2pnt;

	return distVec.Length();
}

inline float GetRayPointDistance(const SRay& ray, const SPoint& point)
{
	return GetRayPointDistance(ray.p, ray.v, point);
}

inline float GetRayPointDistance(const SLineSegment& lineSegment, const SPoint& point)
{
	return GetRayPointDistance(lineSegment.v1, lineSegment.v2 - lineSegment.v1, point);
}











// -------------------------------------------------------------------------------------------
//
//	Ray - OBB
//
// -------------------------------------------------------------------------------------------
inline bool IntersectRayOBB(const SBox& obb, const SRay& ray, float* pMin/*= 0*/, float *pMax/*= 0*/, Vec3f *pMinNormal /*=0*/, Vec3f *pMaxNormal /*=0*/)
{
	float t_min = -FLT_MAX, t_max = FLT_MAX;
	Vec3f minNormal, maxNormal;
	float t1, t2;
	Vec3f n1, n2;
	unsigned char num_intersections = 0;
	unsigned char last_intersection_i = 0;
	for (unsigned char i = 0; i < 6; i += 2)
	{
		if (!IntersectRayPlane(ray, obb.planes[i], &t1, &n1))
			continue;

		if (!IntersectRayPlane(ray, obb.planes[i + 1], &t2, &n2))
			continue;

		++num_intersections;
		last_intersection_i = i;

		// Swap t1 and t2 if necessary
		if (t1 > t2)
		{
			float temp = t1;
			t1 = t2;
			t2 = temp;

			Vec3f ntemp = n1;
			n1 = n2;
			n2 = ntemp;
		}

		// Determine new min/max
		if (t1 > t_min)
		{
			t_min = t1;
			minNormal = n1;
		}
		if (t2 < t_max)
		{
			t_max = t2;
			maxNormal = n2;
		}

		if (t_max < t_min)
			return false;
	}

	if (num_intersections == 0)
		return false;

	if (num_intersections == 1)
	{
		const Vec3f minPoint = ray.GetPoint(t_min);
		const Vec3f maxPoint = ray.GetPoint(t_max);

		// We have to few information here.
		// We'll take the found min/max intersection points and check that they are all at the inside of each other plane
		bool outside;
		for (unsigned char i = 0; i < 6; ++i)
		{
			if (i == last_intersection_i || i == (last_intersection_i + 1))
				continue;

			IntersectPlanePoint(obb.planes[i], minPoint, &outside);
			if (outside)
				return false;
		}
	}


	if (IS_VALID_PTR(pMin)) *pMin = t_min;
	if (IS_VALID_PTR(pMax)) *pMax = t_max;
	if (IS_VALID_PTR(pMinNormal)) *pMinNormal = minNormal;
	if (IS_VALID_PTR(pMaxNormal)) *pMaxNormal = maxNormal;

	return true;
}

// -------------------------------------------------------------------------------------------
//
//	Point - OBB
//
// -------------------------------------------------------------------------------------------
inline bool IntersectPointOBB(const SBox& obb, const SPoint& point)
{
	// Go through all planes and check if the point is on the inside
	bool outside;
	for (unsigned char i = 0; i < 6; ++i)
	{
		if (IntersectPlanePoint(obb.planes[i], point, &outside))
			return true;

		if (outside)
			return false;
	}

	return true;
}


// -------------------------------------------------------------------------------------------
//
//	Ray - Ray
//
// -------------------------------------------------------------------------------------------
inline bool IntersectRayRay(const SRay& ray1, const SRay& ray2, float* pParam1/* = 0*/, float* pParam2/* = 0*/, float* pDist/*=0*/)
{
	// p + t*v = q + s*u
	const Vec3f &p = ray1.p, &v = ray1.v, &q = ray2.p, &u = ray2.v;

	// Both start points are near enough together?
	if ((p - q).LengthSq() <= FLOAT_TOLERANCE)
	{
		if (IS_VALID_PTR(pParam1)) *pParam1 = 0.0f;
		if (IS_VALID_PTR(pParam2)) *pParam2 = 0.0f;
		if (IS_VALID_PTR(pDist)) *pDist = 0.0f;
		return true;
	}

	// dist = dot(p - q, normalized(cross(v, u)))
	// This is determined using the function for the distance between two lines and dot-multiplying both
	// equation sides with distDir, which is perpendicular to v and u.
	const Vec3f distDir = Vec3Cross(v, u).Normalized();
	const float dist = fabsf(Vec3Dot(q - p, distDir));

	Vec3f vxu = Vec3Cross(v, u);
	float den = vxu.LengthSq();
	if (den <= FLOAT_TOLERANCE)
	{
		if (IS_VALID_PTR(pParam1)) *pParam1 = 0.0f;
		if (IS_VALID_PTR(pParam2)) *pParam2 = 0.0f;
		if (IS_VALID_PTR(pDist))
		{
			// Determine distance by projecting p-q-vector onto ray1 and then subtracting this from the p-q-vector
			const Vec3f vNormalized = v.Normalized();
			const Vec3f pq = q - p;
			*pDist = (pq - Vec3Dot(vNormalized, pq) * vNormalized).Length();
		}

		return false; // lines are coincident or parallel
	}

	// When lines do not intersect, t and s are the positions of the nearest distance
	float t = Vec3Dot(Vec3Cross(q - p, u), vxu) / den;
	float s = Vec3Dot(Vec3Cross(q - p, v), vxu) / den;
	if (IS_VALID_PTR(pParam1)) *pParam1 = t;
	if (IS_VALID_PTR(pParam2)) *pParam2 = s;

	bool intersect = (dist <= FLOAT_TOLERANCE);
	if (IS_VALID_PTR(pDist)) *pDist = (intersect ? 0.0f : dist);

	return intersect;
}


// -------------------------------------------------------------------------------------------
//
//	Ray - Plane
//
//	incidentNormal - Set to true to negate pNormal, in case the origin of the ray lies on the other side than the plane normal points to
//
// -------------------------------------------------------------------------------------------
inline bool IntersectRayPlane(const SRay& ray, const SPlane& plane, float *param/* = 0*/, Vec3f* pNormal/* = 0*/, bool incidentNormal/*=false*/)
{
	// Ray: x = p + tv
	// Plane: dot(x, n) = d
	const Vec3f &p = ray.p, &v = ray.v, &n = plane.n;
	const float &d = plane.d;

	// dot(p + tv, n) = d  --> t = (d - dot(p, n)) / dot(v, n)
	float den = Vec3Dot(v, n);

	// Parallel?  dot(v,n) == 0
	if (fabs(den) <= FLOAT_TOLERANCE)
	{
		if (IntersectPlanePoint(plane, p))
		{
			if (IS_VALID_PTR(param)) *param = 0.0f;

			// Simply use planes normal here
			if (IS_VALID_PTR(pNormal)) *pNormal = n;

			return true;
		}
		else
		{
			return false;
		}
	}

	if (IS_VALID_PTR(param))
	{
		*param = (d - Vec3Dot(p, n)) / den;
	}

	if (IS_VALID_PTR(pNormal))
	{
		if (incidentNormal)
		{
			// If the start point of the line lies on the opossite side of the plane,
			// than its normal points to, we simply flip the normal.
			if ((Vec3Dot(p, n) - d) < 0)
				*pNormal = -n;
			else
				*pNormal = n;
		}
		else
		{
			*pNormal = n;
		}
	}

	return true;
}

// -------------------------------------------------------------------------------------------
//
//	Plane - Point
//
//	pOutside - Set to true, if the point lies on the side in which the normal points to.
//
// -------------------------------------------------------------------------------------------
inline bool IntersectPlanePoint(const SPlane& plane, const SPoint& point, bool *pOutside /*= 0*/, float* pDistance /*= 0*/)
{
	// Plane: dot(x, n) = d
	const Vec3f &n = plane.n;
	const float &d = plane.d;

	// Calculate distance here and check against 0

	// D = (dot(n, x) - d) / n.Length()
	float den = n.Length();

	if (fabs(den) <= FLOAT_TOLERANCE)
		return false;
	
	float D = (Vec3Dot(n, point) - d) / den;
	float absD = fabsf(D);

	if (IS_VALID_PTR(pDistance))
		*pDistance = absD;

	if (absD > FLOAT_TOLERANCE)
	{
		if (IS_VALID_PTR(pOutside))
			*pOutside = (D > 0.0f);

		return false;
	}
	else
	{
		return true;
	}
}

// -------------------------------------------------------------------------------------------
//
//	Ray - Point
//
// -------------------------------------------------------------------------------------------
inline bool IntersectRayPoint(const SRay& ray, const SPoint& point, float* pDist/* = 0*/, Vec3f* pFoot /*=0*/)
{
	// Ray: x = p + s * v
	const Vec3f &p = ray.p, &v = ray.v, &x = point;

	// 1. Project (x - p) onto v, normalize with length of v
	Vec3f projected = v * Vec3Dot(v, x - p) / v.Length();

	// 2. Calculate distance
	Vec3f foot = p + projected;
	
	if (IS_VALID_PTR(pFoot))
		*pFoot = foot;

	float dist = Vec3Length(x - foot);

	if (IS_VALID_PTR(pDist))
		*pDist = dist;

	return (dist <= FLOAT_TOLERANCE);
}

// -------------------------------------------------------------------------------------------
//
//	Ray - Sphere
//
// -------------------------------------------------------------------------------------------
inline bool IntersectRaySphere(const SRay& ray, const SSphere& sphere, float* pDist/* = 0*/)
{
	float dist;
	if (IntersectRayPoint(ray, sphere.center, &dist))
	{
		if (IS_VALID_PTR(pDist)) *pDist = -sphere.radius;
		return true;
	}

	if (IS_VALID_PTR(pDist)) *pDist = dist - sphere.radius;

	return (dist <= sphere.radius);
}

// -------------------------------------------------------------------------------------------
//
//	Ray - Cylinder
//
// -------------------------------------------------------------------------------------------
inline bool IntersectRayCylinder(const SRay& ray, const SCylinder& cyl)
{
	// Ray: x = p + t * v
	// Cylinder: p1, p2, radius, u = direction (normalized)
	const Vec3f &p = ray.p, &v = ray.v;
	const Vec3f &p1 = cyl.p1, &p2 = cyl.p2;
	const float &r = cyl.r;
	Vec3f u = p2 - p1;
	const float ulength = u.Length();
	u /= ulength; // normalize u

	// Parallel?
	if (fabs(fabs(Vec3Dot(v.Normalized(), u)) - 1.0f) <= FLOAT_TOLERANCE)
	{
		// Determine distance by projecting p-p1-vector onto ray and then subtracting this from the p-p1-vector
		const Vec3f vNormalized = v.Normalized();
		const Vec3f pq = p1 - p;
		float distSq = (pq - Vec3Dot(vNormalized, pq) * vNormalized).LengthSq();
		return (distSq <= (r * r));
	}

	// Determine perpendicular foot on cyl of the shortest distance-vector
	float param, dist;
	SRay cylRay;
	cylRay.p = p1;
	cylRay.v = u;
	bool intersect = IntersectRayRay(ray, cylRay, 0, &param, &dist);

	// Check if perpendicular foot is in the cyl-ray-segment
	if (param >= 0 && param <= ulength)
		return (dist <= r + FLOAT_TOLERANCE); // intersecting the cylinder

	// Check if the ray intersects with the cap-planes. IntersectRayPlane() must not modify the param, if there was no intersection
	float bottomParam = FLT_MAX, topParam = FLT_MAX;
	bool bottomIntersect = IntersectRayPlane(ray, SPlane::FromNormalAndPoint(-u, p1), &bottomParam);
	bool topIntersect = IntersectRayPlane(ray, SPlane::FromNormalAndPoint(u, p2), &topParam);

	if (!bottomIntersect && !topIntersect)
		return false;

	Vec3f checkPnt = p1;
	param = bottomParam;
	if (topParam < bottomParam)
	{
		param = topParam;
		checkPnt = p2;
	}

	// possibleIntersection and distSq should lie on the same plane (bottom-cap-plane)
	const Vec3f possibleIntersection = ray.GetPoint(param);
	const float distSq = (possibleIntersection - checkPnt).LengthSq();

	return (distSq <= (r * r));
}



// -------------------------------------------------------------------------------------------
//
//	Ray - Capsule
//
// -------------------------------------------------------------------------------------------
inline bool IntersectRayCapsule(const SRay& ray, const SCapsule& capsule, float* pRayParam /*=0*/)
{
	// Ray: x = p + t * v
	// Cylinder: p1, p2, radius, u = direction (normalized)
	const Vec3f &p = ray.p, &v = ray.v;
	const Vec3f &p1 = capsule.p1, &p2 = capsule.p2;
	const float &r = capsule.r;
	Vec3f u = p2 - p1;
	const float ulength = u.Length();
	u /= ulength; // normalize u

		      // Parallel?
	if (fabs(fabs(Vec3Dot(v.Normalized(), u)) - 1.0f) <= FLOAT_TOLERANCE)
	{
		// Determine distance by projecting p-p1-vector onto ray and then subtracting this from the p-p1-vector
		const Vec3f vNormalized = v.Normalized();
		const Vec3f pq = p1 - p;
		float distSq = (pq - Vec3Dot(vNormalized, pq) * vNormalized).LengthSq();
		return (distSq <= (r * r));
	}

	// Determine perpendicular foot on cyl of the shortest distance-vector
	float cylParam, rayParam, dist;
	SRay cylRay;
	cylRay.p = p1;
	cylRay.v = u;
	IntersectRayRay(ray, cylRay, &rayParam, &cylParam, &dist);

	if (IS_VALID_PTR(pRayParam))
		*pRayParam = rayParam;

	// Check if perpendicular foot is in the cyl-ray-segment
	if (cylParam >= 0 && cylParam <= ulength)
		return (dist <= r + FLOAT_TOLERANCE); // intersecting the cylinder

	// Check if the ray intersects with the cap-sphere, that is nearest to the point on the ray that is nearest to the cyl-ray	
	Vec3f checkSphereCenter = p1;
	Vec3f rayPoint = ray.GetPoint(rayParam);
	if ((p2 - rayPoint).LengthSq() < (p1 - rayPoint).LengthSq())
		checkSphereCenter = p2;

	bool intersect = IntersectRaySphere(ray, SSphere(checkSphereCenter, r));
	return intersect;
}



// -------------------------------------------------------------------------------------------
//
//	Point - Sphere
//
//-------------------------------------------------------------------------------------------
inline bool IntersectSpherePoint(const SSphere& sphere, const SPoint& point, float *pDist /*= 0*/)
{
	// If the actual distance is not required, do not use a square-root
	if (!IS_VALID_PTR(pDist))
	{
		return ((point - sphere.center).LengthSq() <= sphere.radius * sphere.radius);
	}
	else
	{
		*pDist = (point - sphere.center).Length() - sphere.radius;
		return (*pDist <= FLOAT_TOLERANCE);
	}
}

// -------------------------------------------------------------------------------------------
//
//	Sphere - Sphere
// 
// 	pDist might be negative, meaning that the spheres inter-penetrate
//
// -------------------------------------------------------------------------------------------
inline bool IntersectSphereSphere(const SSphere& sphere1, const SSphere& sphere2, float* pDist/* = 0*/)
{
	if (IS_VALID_PTR(pDist))
	{
		*pDist = Vec3Length(sphere2.center - sphere1.center) - sphere1.radius - sphere2.radius;

		// dist can be negative
		return (*pDist <= FLOAT_TOLERANCE);
	}
	else
	{
		// Do not use a square root when the actual distance is not important.

		float radSum = sphere1.radius + sphere2.radius;
		if ((sphere2.center - sphere1.center).LengthSq() <= radSum * radSum)
			return true;
		else
			return false;
	}
}


// -------------------------------------------------------------------------------------------
//
//	Point - Capsule
//
// -------------------------------------------------------------------------------------------
inline bool IntersectCapsulePoint(const SCapsule& capsule, const SPoint& point, float* pDist/*=0*/)
{
	// Capsule: p1, p2, r
	const Vec3f &p1 = capsule.p1, &p2 = capsule.p2;
	const Vec3f v = p2 - p1;
	const float vlength = v.Length();
	const float &r = capsule.r;

	// Calculate position of the perpendicular foot of that point on the line by projecting (point - p1) onto v	
	const Vec3f vnorm = v / vlength;
	const float projectedLength = Vec3Dot(point - p1, vnorm);
	const Vec3f perpFoot = p1 + projectedLength * vnorm;

	// If the distance is greater than the radius, the point definetly lies not inside the capsule
	const float distSq = (perpFoot - point).LengthSq();
	if (distSq > (r * r))
	{
		if (IS_VALID_PTR(pDist))
		{
			float cylDist = sqrtf(distSq) - r;
			float dist1 = FLT_MAX, dist2 = FLT_MAX;
			SSphere testSphere;
			testSphere.radius = r;
			testSphere.center = p1;
			IntersectSpherePoint(testSphere, point, &dist1);

			testSphere.center = p2;
			IntersectSpherePoint(testSphere, point, &dist2);

			*pDist = min(cylDist, min(dist1, dist2));
		}
		return false;
	}

	// Check if the perpFoot is in the line segment of the inner cylinder
	if (projectedLength >= 0.0f && projectedLength <= vlength)
	{
		if (IS_VALID_PTR(pDist)) *pDist = sqrtf(distSq) - r;
		return true;
	}

	// If the projected is out of (-radius;length+radius) range, there is definetly not a collision.	
	bool inPossibleRange = (projectedLength >= -r && projectedLength <= (vlength + r));
	if (inPossibleRange || IS_VALID_PTR(pDist))
	{
		// Test with both cap-spheres to determine minimum distance to the capsule
		float dist1 = FLT_MAX, dist2 = FLT_MAX;
		bool test1, test2;
		SSphere testSphere;
		testSphere.radius = r;
		testSphere.center = p1;
		test1 = IntersectSpherePoint(testSphere, point, &dist1);

		testSphere.center = p2;
		test2 = IntersectSpherePoint(testSphere, point, &dist2);

		// minimal distance is ALWAYS the minimum of dist1 and dist2
		if (IS_VALID_PTR(pDist)) *pDist = min(dist1, dist2);

		if (inPossibleRange)
		{
			return (test1 || test2);
		}
	}

	// pDist is already set above
	return false;
}


// -------------------------------------------------------------------------------------------
//
//	Plane - Sphere
//
// -------------------------------------------------------------------------------------------
inline bool IntersectPlaneSphere(const SPlane& plane, const SSphere& sphere)
{
	float distance;
	IntersectPlanePoint(plane, sphere.center, 0, &distance);

	return distance <= sphere.radius;
}

// -------------------------------------------------------------------------------------------
//
//	Plane - Capsule
//
// -------------------------------------------------------------------------------------------
inline bool IntersectPlaneCapsule(const SPlane& plane, const SCapsule& capsule)
{
	// If centers of the caps lie on different sides or at least one center-of-cap lies on the plane
	Vec3f capsuleDir = Vec3Normalize(capsule.p2 - capsule.p1);
	bool outside1, outside2;
	float distance1, distance2;
	bool tipInt1 = IntersectPlanePoint(plane, capsule.p1 - capsuleDir * capsule.r, &outside1, &distance1);
	bool tipInt2 = IntersectPlanePoint(plane, capsule.p2 + capsuleDir * capsule.r, &outside2, &distance2);

	if (tipInt1 || tipInt2 || (outside1 != outside2))
	{
		return true;
	}

	// If at least one of the spheres intersects with the plane
	if (distance1 <= capsule.r || distance2 <= capsule.r)
	{
		return true;
	}

	return false;
}






// -------------------------------------------------------------------------------------------
//
//	Triangle - Point
//
// -------------------------------------------------------------------------------------------
inline bool IntersectTrianglePoint(const SMeshVertex& vtx1, const SMeshVertex& vtx2, const SMeshVertex& vtx3, const SPoint& point, bool* bOutside /*=0*/, float* pDistance /*=0*/)
{
	const Vec3f A(vtx1.x, vtx1.y, vtx1.z), B(vtx2.x, vtx2.y, vtx2.z), C(vtx3.x, vtx3.y, vtx3.z);

	// Make sure the point is on the plane
	if (!IntersectPlanePoint(SPlane::FromPoints(A, B, C), point, bOutside, pDistance))
	{
		return false;
	}

	const Vec3f P(point.x, point.y, point.z);
	const Vec3f v0 = C - A,
		v1 = B - A,
		v2 = P - A;

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

// -------------------------------------------------------------------------------------------
//
//	Triangle - Sphere
//
// -------------------------------------------------------------------------------------------
inline bool IntersectTriangleSphere(const SMeshVertex& vtx1, const SMeshVertex& vtx2, const SMeshVertex& vtx3, const SSphere& sphere, bool* bOutside /*=0*/)
{
	Vec3f v1(vtx1.x, vtx1.y, vtx1.z), v2(vtx2.x, vtx2.y, vtx2.z), v3(vtx3.x, vtx3.y, vtx3.z);
	Vec3f foot = GetFootOnPlane(SPlane::FromPoints(v1, v2, v3), sphere.center);

	float distance = Vec3Length(sphere.center - foot);
	if (distance > sphere.radius)
		return false;

	if (IntersectTrianglePoint(vtx1, vtx2, vtx3, foot, bOutside))
	{
		return true;
	}
	else
	{
		// Test against all edges
		if (IntersectLineSphere(SRay::FromPoints(v1, v2), 1.0f, sphere))
			return true;

		if (IntersectLineSphere(SRay::FromPoints(v2, v3), 1.0f, sphere))
			return true;

		if (IntersectLineSphere(SRay::FromPoints(v3, v1), 1.0f, sphere))
			return true;

		return false;
	}
}

// -------------------------------------------------------------------------------------------
//
//	Triangle - Capsule
//
// -------------------------------------------------------------------------------------------
inline bool IntersectTriangleCapsule(const SMeshVertex& vtx1, const SMeshVertex& vtx2, const SMeshVertex& vtx3, const SCapsule& capsule)
{
	const Vec3f A(vtx1.x, vtx1.y, vtx1.z), B(vtx2.x, vtx2.y, vtx2.z), C(vtx3.x, vtx3.y, vtx3.z);

	SPlane plane = SPlane::FromPoints(A, B, C);

	// First, try to intersect with triangle's plane
	if (!IntersectPlaneCapsule(plane, capsule))
	{
		return false;
	}

	// Check if the capsule hits the "inside" of the triangle
	// TODO: Optimize this by taking the intersection result from IntersectPlaneCapsule above	
	SRay ray = SRay::FromPoints(capsule.p1, capsule.p2);
	float t;
	if (IntersectRayPlane(ray, plane, &t))
	{
		if (t >= 0.0f && t <= 1.0f)
			return true;

		Vec3f x = ray.GetPoint(t);
		Vec3f testCap = (t < 0.0f ? capsule.p1 : capsule.p2);
		if ((x - testCap).LengthSq() <= capsule.r * capsule.r)
			return true;
	}

	// Now test distances between line segments
	SLineSegment capsuleLine(capsule.p1, capsule.p2);

	float dSq;
	float radSq = capsule.r * capsule.r;
	dSq = GetMinLineSegmentDistanceSq(capsuleLine, SLineSegment(A, B));
	if (dSq <= radSq)
		return true;

	dSq = GetMinLineSegmentDistanceSq(capsuleLine, SLineSegment(B, C));
	if (dSq <= radSq)
		return true;

	dSq = GetMinLineSegmentDistanceSq(capsuleLine, SLineSegment(C, A));
	if (dSq <= radSq)
		return true;

	return false;
}


// -------------------------------------------------------------------------------------------
//
//	Line Segment - Triangle
//
//	pIntersection - If a valid pointer, will be set to the point of intersection of the ray and the triangle plane, even if this point does not lie in the triangle
//
// -------------------------------------------------------------------------------------------
inline bool IntersectLineTriangle(const SLineSegment& line, const SMeshVertex& vtx1, const SMeshVertex& vtx2, const SMeshVertex& vtx3, Vec3f* pIntersection /*=0*/, Vec3f* pNormal /*=0*/)
{
	SRay ray = SRay::FromPoints(line.v1, line.v2);

	float param;
	bool planeInt = IntersectRayPlane(ray, SPlane::FromPoints(Vec3f(vtx1.x, vtx1.y, vtx1.z), Vec3f(vtx2.x, vtx2.y, vtx2.z), Vec3f(vtx3.x, vtx3.y, vtx3.z)), &param, pNormal);
	if (!planeInt)
		return false;
	
	// Out of line segment
	if (param < 0 || param * param > ray.p.LengthSq())
		return false;

	// Check if in triangle using barycentric coordinates
	Vec3f intersection = ray.GetPoint(param);
	if (IS_VALID_PTR(pIntersection))
		*pIntersection = intersection;

	bool triangleInt = IntersectTrianglePoint(vtx1, vtx2, vtx3, SPoint(intersection));
	return triangleInt;
}


struct SContactInfo
{
	bool intersection;
	bool vertexFace; // true if contact is vertex/face, otherwise edge/edge
	bool interpenetration;

	Vec3f contactPoint;	// point of contact
	Vec3f contactNormal;	// normal of the surface of the vertex/face contact

	Vec3f edge1;	// normalized direction of edge 1	
	Vec3f edge2;	// normalized direction of edge 2


	SLineSegment closestEdge; //? DEBUGGING
	std::string desc; //? DEBUGGING: Description text
};


// Case #1: Line segment between centers of caps intersects triangle
// 	- Always inter - penetration(further state of case #2)
//	- Contact point = intersection point between line segment and triangle
//	- Contact normal = 
// Case #2: Cap collides with triangle(determine foot on plane, then check using barycentrics)
// 	- Interpenetration : distance between cap - center and plane is < radius - epsilon
// 	- = Vertex / face
// 	- Contact point : foot point of cap center on plane
// 	- contact normal : triangle normal
// Case #3: Capsule collides with(at least one) edge(using Line - segment <->Line - Segment distance)
// 	- Interpenetration : distance < radius - epsilon
// 	- = Edge / Edge
// 	- Edge A : triangle edge
// 	- Edge B : capsule ray
//
//
//	tolerance - Interpenetration tolerance
//
inline bool IntersectTriangleCapsuleEx(const SMeshVertex& vtx1, const SMeshVertex& vtx2, const SMeshVertex& vtx3, const SCapsule& capsule, SContactInfo& contact, float tolerance)
{
	const Vec3f v1(vtx1.x, vtx1.y, vtx1.z), v2(vtx2.x, vtx2.y, vtx2.z), v3(vtx3.x, vtx3.y, vtx3.z);

	// Case 1: Intersection of capsule line segment and triangle
	const SLineSegment line(capsule.p1, capsule.p2);
	Vec3f planeIntersection;
	if (IntersectLineTriangle(line, vtx1, vtx2, vtx3, &planeIntersection, &contact.contactNormal))
	{
		contact.contactPoint = planeIntersection;
		contact.contactNormal = Vec3Cross(v2 - v1, v3 - v1).Normalized();
		contact.intersection = true;
		contact.interpenetration = true;
		contact.vertexFace = true;
		contact.desc = "lineSeg-Tri";
		return true;
	}	

	// Case 2: Intersection of (at least one of the) capsule caps and triangle
	float minRadius = capsule.r - tolerance;
	float maxRadius = capsule.r + tolerance;
	
	float minRadiusSq = minRadius * minRadius;
	float maxRadiusSq = maxRadius * maxRadius;

	SPlane plane = SPlane::FromPoints(v1, v2, v3);	
	
	Vec3f foot1 = GetFootOnPlane(plane, SPoint(capsule.p1));
	bool capInt1 = IntersectTrianglePoint(vtx1, vtx2, vtx3, foot1);	
	float distSq1 = (foot1 - capsule.p1).LengthSq();
	capInt1 = capInt1 && (distSq1 < maxRadiusSq);
	if (capInt1)
	{
		contact.intersection = true;

		contact.interpenetration = (distSq1 < minRadiusSq);

		contact.vertexFace = true;
		contact.contactPoint = foot1;
		contact.contactNormal = plane.n.Normalized();

		contact.desc = "cap1-tri";

		// Here, it might well be that the first cap does contact without interpenetration, but the second does
		if (contact.interpenetration)
			return true;
	}	

	Vec3f foot2 = GetFootOnPlane(plane, SPoint(capsule.p2));
	bool capInt2 = IntersectTrianglePoint(vtx1, vtx2, vtx3, foot2);
	float distSq2 = (foot2 - capsule.p2).LengthSq();
	capInt2 = (distSq2 < maxRadiusSq);
	if (capInt2 && !(capInt1 && distSq2 < distSq1))
	{
		contact.intersection = true;

		contact.interpenetration = (distSq2 < minRadiusSq);

		contact.vertexFace = true;
		contact.contactPoint = foot2;
		contact.contactNormal = plane.n.Normalized();
		
		contact.desc = "cap2-tri";
	}

	if (capInt1 || capInt2)	
		return true;


	// Case 3: Capsule collides with at least one triangle edge (using dist. between line segments)
	const SLineSegment edges[] = {
		SLineSegment(v1, v2), SLineSegment(v2, v3), SLineSegment(v3, v1)
	};

	// Find the closest capsule-linesegment <-> edge pair
	int minEdge = -1;
	float minDistSq = 0.f;
	Vec3f p1, p2; // closest points on the line segments
	for (int i = 0; i < 3; ++i)
	{
		Vec3f pp1, pp2;
		float distSq = GetMinLineSegmentDistanceSq(edges[i], line, &pp1, &pp2);
		if (i == 0 || distSq < minDistSq)
		{
			minEdge = i;
			minDistSq = distSq;
			p1 = pp1;
			p2 = pp2;
		}
	}

	if (minDistSq < maxRadiusSq)
	{
		contact.intersection = true;

		contact.interpenetration = (minDistSq < minRadiusSq);

		contact.vertexFace = false;
		contact.edge1 = (edges[minEdge].v2 - edges[minEdge].v1).Normalized();
		contact.edge2 = (line.v2 - line.v1).Normalized();		
		
		// We can actually set the contact point and normal, although this is an edge/edge contact
		contact.contactPoint = p1;
		//contact.contactNormal = Vec3Cross(contact.edge1, contact.edge2).Normalized();
		contact.contactNormal = plane.n.Normalized();

		contact.desc = "seg-seg";

		contact.closestEdge = edges[minEdge]; //? DEBUG


		return true;
	}

	return false;
}




// -------------------------------------------------------------------------------------------
//
//	Line - Sphere
//
// -------------------------------------------------------------------------------------------
inline bool IntersectLineSphere(const SRay& ray, const float maxLambda, const SSphere& sphere)
{
	const Vec3f &p = ray.p, &v = ray.v;
	const Vec3f& c = sphere.center;
	const float r = sphere.radius;

	float vLength = Vec3Length(v);

	// As we normalize v, we also have to scale maxLambda accordingly
	float scaledMaxLambda = maxLambda * vLength;

	Vec3f cp = p - c;
	float vDotCP = Vec3Dot(v / vLength, cp);
	float cpLength = Vec3Length(cp);	

	// d = -vDotCP +- sqrt(vDotCp^2 - length(cp)^2 + r^2)

	float D = vDotCP * vDotCP - cpLength * cpLength + r * r;
	
	if (D < -FLOAT_TOLERANCE)
		return false;

	float sqrtD = sqrtf(D);	

	// Check first solution
	float t = -vDotCP + sqrtD;
	if (t <= maxLambda)
		return true;

	if (D > FLOAT_TOLERANCE)
	{
		// Check the other solution
		t = -vDotCP - sqrtD;
		if (t <= maxLambda)
			return true;
	}

	return false;
}










// -------------------------------------------------------------------------------------------
//
//	Mesh - Capsule
//
// -------------------------------------------------------------------------------------------
inline bool IntersectMeshCapsule(const SMesh& mesh, const SCapsule& capsule, vector<SContactInfo>& contacts, float interpenetrationTolerance = 0.001f, bool* pInterpenetration = 0, vector<SMeshFace>* pPossibleFaces = 0)
{
	// First, determine AABB of the capsule
	AABB capsuleAABB;
	capsuleAABB.Reset();
	capsuleAABB.AddPoint(capsule.p1);
	capsuleAABB.AddPoint(capsule.p2);
	capsuleAABB.Outset(capsule.r);

	// Determine all possible faces that intersect with the capsule AABB
	vector<const SMeshFace*> possibleFaces;
	mesh.GetIntersectingFaces(capsuleAABB, possibleFaces);

	if (possibleFaces.size() == 0)
		return false;

	if (IS_VALID_PTR(pPossibleFaces))
		pPossibleFaces->reserve(possibleFaces.size());

	// We have to go through all possibly intersecting faces to find all contact points
	bool intersection = false;
	bool interpenetration = false;
	for (auto itFace = possibleFaces.begin(); itFace != possibleFaces.end(); ++itFace)
	{
		const SMeshFace* pMeshFace = *itFace;

		if (IS_VALID_PTR(pPossibleFaces))
			pPossibleFaces->push_back(**itFace);

		SContactInfo tmpContact;
		if (IntersectTriangleCapsuleEx(pMeshFace->vtx[0], pMeshFace->vtx[1], pMeshFace->vtx[2], capsule, tmpContact, interpenetrationTolerance))
		{
			contacts.push_back(tmpContact);
			intersection = true;
			
			if (tmpContact.interpenetration)
				interpenetration = true;
		}
	}

	if (IS_VALID_PTR(pInterpenetration))
		*pInterpenetration = interpenetration;

	return intersection;
}


















// ----------------------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------------
//
//
//	Shape - Shape
//
//	- transform1/2 :  Matrix describing the transformation to transform the shape1/2 before testing intersections
//
//
// ----------------------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------------------
inline bool IntersectShapes(const SGeomShape& shape1, const SGeomShape& shape2, vector<SContactInfo>& contacts, float interpenetrationTolerance = 0.00001f)
{
	// Capsule - Mesh
	if (shape1.type == eGEOMSHAPE_CAPSULE && shape2.type == eGEOMSHAPE_MESH)
		return IntersectMeshCapsule(*shape2.pMesh, SCapsule(shape1), contacts, interpenetrationTolerance);
	else if (shape1.type == eGEOMSHAPE_MESH && shape2.type == eGEOMSHAPE_CAPSULE)
		return IntersectMeshCapsule(*shape1.pMesh, SCapsule(shape2), contacts, interpenetrationTolerance);

	// Capsule - Capsule

	// Capsule - Sphere

	// Capsule - Box

	// ...



	CLog::Log(S_WARN, "Unhandled intersection test: Unknown shape combination: %s <-> %s!",
		GetGeomShapeTypeName(shape1.type), GetGeomShapeTypeName(shape2.type));

	return false;
}


SP_NMSPACE_END
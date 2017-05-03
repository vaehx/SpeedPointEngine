#include "MathGeom.h"

SP_NMSPACE_BEG


///////////////////////////////////////////////////////////////////////////////////////////////

S_API void SMeshKTree::Clear()
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

S_API void SMeshKTree::Init(const vector<SMeshFace>& insertFaces, const AABB& _aabb, const vector<u32>& faceIndices, EMeshKTreeType _type /*= eMESH_KTREE_OCTREE*/, unsigned int maxDepth /*= 0*/)
{
	//CLog::Log(S_DEBUG, "SMeshKTree::Init(insertFaces=[%d], faceIndices=[%d], depth=%d)", insertFaces.size(), faceIndices.size(), maxDepth);

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

		vector<u32> pChildFaces[MESH_KTREE_MAX_CHILDS];

		unsigned long numIntersections = 0;

		int numFilledChilds = 0;
		Vec3f v0, v1, v2;
		int __iii = 0; // DEBUG
		for (auto itFaceIndex = faceIndices.begin(); itFaceIndex != faceIndices.end(); ++itFaceIndex, ++__iii)
		{
			const SMeshFace& face = insertFaces[*itFaceIndex];

			// For each child
			for (unsigned int i = 0; i < k; ++i)
			{
				v0 = MESHVERTEX_TO_VEC3F(face.vtx[0]);
				v1 = MESHVERTEX_TO_VEC3F(face.vtx[1]);
				v2 = MESHVERTEX_TO_VEC3F(face.vtx[2]);

				if (pChildAABBs[i].HitsLineSegment(v0, v1) || pChildAABBs[i].HitsLineSegment(v1, v2) || pChildAABBs[i].HitsLineSegment(v2, v0))
				{
					++numIntersections;

					// This is the first face we found, that intersects with this child. So we have another child to create.
					if (pChildFaces[i].size() == 0)
						++numFilledChilds;

					// Simulate chunked behavior
					if (pChildFaces[i].size() % 100 == 0)
						pChildFaces[i].reserve(pChildFaces[i].size() + 100);

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

		//CLog::Log(S_DEBUG, "kTree (depth=%d): %d faces, %d children", maxDepth, faceIndices.size(), numFilledChilds);

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

		//CLog::Log(S_DEBUG, "kTree (LEAF): %d faces", faces.size());
	}
}

S_API void SMeshKTree::GetIntersectingLeafs(const AABB& operand, vector<SMeshKTree*>& leafs)
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
S_API void SMeshKTree::GetIntersectingFaces(const AABB& operand, vector<const SMeshFace*>& intersecting, const SMatrix& transform /*= SMatrix()*/, vector<u16>& foundIntersecting /*= vector<u16>()*/) const
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

S_API bool SMeshKTree::IsLeaf() const
{
	return nChilds == 0 || !IS_VALID_PTR(pChilds);
}

// Multiplies each vertex with this matrix
S_API void SMeshKTree::TransformVertices(const SMatrix& matrix)
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
S_API void SMeshKTree::UpdateAABBHeights(float minY, float maxY)
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


///////////////////////////////////////////////////////////////////////////////////////////////


bool IntersectRayCylinder(const SRay& ray, const SCylinder& cyl, SIntersection* inters)
{
	const Vec3f vr = ray.v.Normalized(); // normalized ray direction
	const Vec3f pc[] = { cyl.p1, cyl.p2 }; // cylinder bottom/top cap center
	const Vec3f vc = (pc[1] - pc[0]).Normalized(); // normalized cylinder ray direction
	const float rsq = cyl.r * cyl.r; // r^2

	// Test against mantle
	// Find solutions for At^2 + Bt + C = 0 with ...
	const Vec3f U = vr - Vec3Dot(vr, vc) * vc;
	const Vec3f V = (ray.p - pc[0]) - Vec3Dot(ray.p - pc[0], vc) * vc;
	const float A = U.LengthSq();
	const float B = 2.0f * Vec3Dot(U, V);
	const float C = V.LengthSq() - rsq;

	float t[] = { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX }; // normalized ray params
	bool b[] = { false, false, false, false }; // t[i] is a valid intersection
	int n = 0;
	const float s = B * B - 4.0f * A * C;
	const float den = 2.0f * A;
	if (s >= 0 && fabsf(den) > 0.0001f)
	{
		// Intersections with cylinder mantle
		const float sqrt_s = sqrtf(s);
		const float invden = 1.0f / den;
		t[0] = (-B + sqrt_s) * invden;
		t[1] = (-B - sqrt_s) * invden;

		// Check if intersections are between caps
		Vec3f q;
		for (int i = 0; i < 2; ++i)
		{
			q = ray.p + vr * t[i];
			b[i] = (Vec3Dot(vc, q - pc[0]) > 0 && Vec3Dot(vc, q - pc[1]) < 0);
		}

		n += 2;
	}

	if (!b[0] || !b[1])
	{
		// Test against caps
		float param;
		SPlane plane;
		SRay nray(ray.p, vr); // normalized ray
		for (int i = 0; i < 2; ++i)
		{
			plane = SPlane::FromNormalAndPoint(vc, pc[i]);
			if (IntersectRayPlane(nray, plane, &param))
			{
				if ((nray.GetPoint(param) - pc[i]).LengthSq() <= rsq)
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
	bool intersect = false;
	for (int i = 0; i < n; ++i)
	{
		if (b[i])
		{
			intersect = true;
			if (mint == n || t[i] < t[mint])
				mint = i;
		}
	}

	if (intersect)
	{
		inters->p = ray.p + vr * t[mint];
		if (mint < 2)
		{
			inters->n = Vec3Normalize(inters->p - pc[0] + ((inters->p - pc[0]) | vc) * vc);
			inters->feature = eINTERSECTION_FEATURE_BASE_SHAPE;
		}
		else
		{
			inters->n = Vec3Normalize(inters->p - pc[mint - 2]);
			inters->feature = eINTERSECTION_FEATURE_CAP;
		}
	}
	
	return intersect;
}



SP_NMSPACE_END

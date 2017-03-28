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

SP_NMSPACE_END

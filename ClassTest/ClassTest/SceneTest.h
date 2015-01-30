#pragma once


struct SVec3 { float x, y, z; };
struct SAABB { SVec3 min, max; };

struct IObject
{

};





class CSceneObject
{
private:
	// store a ptr to the actual object, but only access it if necessary.
	// Also, try to only include IObject definition if required!
	IObject* pObject;



	// Store all information necessary for scene calculations here.
	// Thus, acessing this data can happen in coherent memory (the CSceneObject-Buffer)

	SAABB aabb;


public:

};




// Summary:
//	Scene Octree, implementing fast intersection testing using sweep sort
class CSceneOctree
{
private:
	SAABB aabb;

	CSceneOctree* subTrees; // dont use fixed size array here - endless destruction otherwise	
	CSceneOctree* pParent;	

	CSceneObject* pObjects;	
	unsigned int nObjects;

	enum { DEF_MAX_DEPTH = 5 };

public:
	CSceneOctree()
		: subTrees(0), pParent(0), pObjects(0), nObjects(0) {}

	virtual ~CSceneOctree()
	{
		Clear();
	}

	// returns true if inserted
	bool Insert(T* obj, const unsigned int maxDepth = DEF_MAX_DEPTH, const unsigned int curDepth = 0)
	{
		if (!IS_VALID_PTR(obj))
			return;

		if (!aabb.Intersects(obj->GetAlixAlignedBoundBox()))
			return false;

		if (curDepth == maxDepth)
		{
			isLeave = true;
			objects.Add(obj);
			return true;
		}

		if (isLeave)
		{
			// create suboctrees
			subTrees = new Octree[8];
			for (unsigned int i = 0; i < 8; ++i)
				subTrees[i].pParent = this;

			SVector3 vMaxH = aabb.vMax * 0.5f;
			subTrees[0].aabb = AABB(aabb.vMin, vMaxH);  // left bottom front
			subTrees[1].aabb = AABB(SVector3(vMaxH.x, aabb.vMin.y, aabb.vMin.z), SVector3(aabb.vMax.x, vMaxH.y, vMaxH.z);   // right bottom front
			subTrees[2].aabb = AABB(SVector3(aabb.vMin.x, aabb.vMin.y, vMaxH.z), SVector3(vMaxH.x, vMaxH.y, aabb.vMax.z));  // left bottom back
			subTrees[3].aabb = AABB(SVector3(vMaxH.x, aabb.vMin.y, vMaxH.z), SVector3(aabb.vMax.x, vMaxH.y, aabb.vMax.z)); // right bottom back
			subTrees[4].aabb = AABB(SVector3(aabb.vMin.x, vMaxH.y, aabb.vMin.z), SVector3(vMaxH.x, aabb.vMax.y, vMaxH.z)); // left top front
			subTrees[5].aabb = AABB(SVector3(vMaxH.x, vMaxH.y, aabb.vMin.z), SVector3(aabb.vMax.x, aabb.vMax.y, vMaxH.z)); // right top front
			subTrees[6].aabb = AABB(SVector3(aabb.vMin.x, vMaxH.y, vMaxH.z), SVector3(vMaxH.x, aabb.vMax.y, aabb.vMax.z)); // left top back
			subTrees[7].aabb = AABB(vMaxH, aabb.vMax); // right top back

			isLeave = false;
		}

		for (unsigned int i = 0; i < 8; ++i)
		{
			if (subTrees[i].Insert(obj, maxDepth, curDepth + 1))
				return true;
		}

		return false;
	}

	// This function uses recursive DFS algorithm.
	// Note: if an object intersects with multiple Octrees, all of them are added to
	// the output.
	void GetIntersectingOctrees(std::vector<IOctree*>& out, const AABB& aabbInt)
	{
		if (isLeave || !IS_VALID_PTR(subTrees))
		{
			if (aabb.Intersects(aabbInt))
				out.push_back(this);

			return;
		}

		for (unsigned int i = 0; i < 8; ++i)
		{
			subTrees[i].GetIntersectingOctrees(out, aabbInt);
		}
	}

	void Clear()
	{
		isLeave = false;
		objects.Clear();
		if (!isLeave && IS_VALID_PTR(subTrees))
			delete[] subTrees;

		pParent = 0;
	}
};

#pragma once

#include <Common\BoundBox.h>
#include <Common\SPrerequisites.h>
#include <string>

using std::string;

SP_NMSPACE_BEG

struct S_API SRenderDesc;
struct S_API IVertexBuffer;
struct S_API IIndexBuffer;

struct S_API IRenderObject
{
protected:
	typedef int IsDerivedFromIRenderObject;

	virtual void Clear() {}

public:
#ifdef _DEBUG
	string _name;
#endif
	
	virtual ~IRenderObject() { Clear(); }

	virtual void Release() = 0;
	virtual bool IsTrash() const = 0;

	virtual AABB GetAABB() = 0;
	virtual SRenderDesc* GetRenderDesc() = 0;

	// viewProj - The custom view-projection matrix to apply when rendering this renderobject.
	//			  If viewproj == 0, the custom viewproj matrix is unset and the renderer uses the viewproj of the viewport
	virtual void SetCustomViewProjMatrix(const Mat44& viewMtx, const Mat44& projMtx) = 0;

	// Events
public:
	virtual void OnRender() {}
};

SP_NMSPACE_END

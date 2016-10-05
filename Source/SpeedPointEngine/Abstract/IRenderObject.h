#pragma once

#include "SPrerequisites.h"
#include "BoundBox.h"
#include <string>

using std::string;

SP_NMSPACE_BEG

struct S_API SRenderDesc;
struct S_API IVertexBuffer;
struct S_API IIndexBuffer;

struct S_API IRenderObject
{
#ifdef _DEBUG
	string _name;
#endif

	virtual ~IRenderObject() {}

	virtual void Release() = 0;
	virtual bool IsTrash() const = 0;

	virtual AABB GetAABB() = 0;


	// Events
public:
	virtual void OnRender() {}



	virtual SRenderDesc* GetRenderDesc() = 0;

	// viewProj - The custom view-projection matrix to apply when rendering this renderobject.
	//			  If viewproj == 0, the custom viewproj matrix is unset and the renderer uses the viewproj of the viewport
	virtual void SetCustomViewProjMatrix(const SMatrix* viewProj) = 0;

	virtual IVertexBuffer* GetVertexBuffer() = 0;
	virtual IIndexBuffer* GetIndexBuffer(unsigned int subset = 0) = 0;
};


SP_NMSPACE_END

// ********************************************************************************************

//	SpeedPoint Solid

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include <Util\STransformable.h>
#include <Abstract\IIndexBuffer.h> // for SIndex


SP_NMSPACE_BEG

class S_API SpeedPointEngine;
struct S_API SVertex;
struct S_API SPrimitive;
struct S_API SMaterial;
class S_API SBoundBox;
class S_API IOctree;


// SpeedPoint Geometrical Oject Interface
struct S_API IVisibleObject : public STransformable
{	
	enum ERenderPipelineStrategy
	{
		eRENDER_FORWARD,
		eRENDER_DEFERRED_SHADING,
		eRENDER_DEFERRED_LIGHTING
	};


	// -- General --

	// Note:
	//	Does NOT destruct Textures!
	virtual SResult Clear(void) = 0;

	virtual SResult Initialize(SpeedPointEngine* pEngine, bool bDynamic) = 0;

	virtual SResult SetGeometryData(SVertex* pVertices, UINT nVertices, SIndex* pdwIndices, UINT nIndices) = 0;

	virtual SP_ID GetVertexBuffer(void) = 0;
	virtual SP_ID GetIndexBuffer(void) = 0;

	virtual SResult RenderSolid(SpeedPointEngine* pEngineReplacement = 0) = 0;



	// -- Primitive --

	virtual SP_ID AddPrimitive(const SPrimitive& primitive) = 0;

	virtual SPrimitive* GetPrimitive(SP_ID id) = 0;
	virtual SPrimitive* GetPrimitive(SP_UNIQUE index) = 0;

	virtual usint32 GetPrimitiveCount(void) = 0;



	// -- Material --

	// Summary:
	//	Set the material that is applied to the whole geometrical object
	virtual void SetMaterial(const SMaterial& mat) = 0;
	virtual SMaterial* GetMaterial(void) = 0;




	// -- BoundBox --

	virtual SBoundBox* RecalculateBoundBox(void) = 0;

	virtual SBoundBox* GetBoundBox(void) = 0;
	virtual SBoundBox* SetBoundBox(const SBoundBox& bbox) = 0;




	// -- Octree --

	virtual IOctree* GetOctree(void) = 0;



	// -- Animations --

	virtual void SetAnimationBundle(SP_ID iBundle) = 0;
	virtual SP_ID GetAnimationBundle(void) = 0;

	// Summary:
	//	Set the currently running animation by its specification
	virtual SResult RunAnimation(SString cName) = 0;
	virtual SResult TickAnimation(float fFrameDelay) = 0;

	virtual SResult GetCurrentAnimation(SString* cOutput) = 0;		
};



typedef struct S_API ISolid IGeometricalObject;

SP_NMSPACE_END

// ********************************************************************************************

//	SpeedPoint VertexBuffer Resource

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include "IResource.h"
#include <Common\SPrerequisites.h>

SP_NMSPACE_BEG



struct S_API SVertex;
struct S_API IRenderer;


enum S_API EVBLockType
{
	eVBLOCK_DISCARD,
	eVBLOCK_NOOVERWRITE,
	eVBLOCK_KEEP
};


// Usage type of a vertexbuffer to let the engine get the most optimization
enum S_API EVBUsage
{
	eVBUSAGE_STATIC,		// the buffer has initial data and never changes until destruction
	eVBUSAGE_DYNAMIC_RARE,		// the buffer content changes less than once per frame
	eVBUSAGE_DYNAMIC_FREQUENT	// the buffer content changes more than once per frame
};



// SpeedPoint vertex buffer
struct S_API IVertexBuffer : public IResource
{
public:
	// Summary:
	//	Initialize the vertex buffer, create the Hardware vertex buffer if initial data is given.
	virtual SResult Initialize(IRenderer* renderer, EVBUsage usage,
		const SVertex* pInitialData = nullptr, const unsigned long nInitialVertices = 0) = 0;	

	// Check if this Vertex Buffer is inited properly, this means if the HW Vertex Buffer has been created and filled at least once.
	virtual bool IsInited(void) = 0;

	// Summary:
	//	Fill the Hardware Vertex Buffer with an array of vertices.
	// Description:
	//	When the hardware vertex buffer has not been created yet, this function updates RAM buffer AND
	//	creates the HW vertex buffer and uploads the data immediately.
	//	Old vertices will be overwritten.
	virtual SResult Fill(const SVertex* pVertices, const unsigned long nVertices) = 0;

	// Summary:
	//	Updates the data to the dynamic HW Vertex Buffer by the data stored in the RAM copy.
	//	Fails if the vertex buffer created is not dynamic. This function does not create the vertex buffer.
	//	To update vertex data in a static Vertex Buffer, use Fill() instead.
	// Returns:
	//	NOTINITED: The HW Vertex buffer or the RAM copy have never been created OR the vertex buffer is not dynamic.
	//	INVALIDPARAM: iVtxStart and nVertices define an invalid range in the buffer
	virtual SResult UploadVertexData(unsigned long iVtxStart = 0, unsigned long nVertices = 0) = 0;

	// Get the RAM Copy of the hardware Vertex Buffer.
	// Changes will not be updated automatically in HW Vertex Buffer. To do so, call UploadVertexData()
	virtual SVertex* GetShadowBuffer(void) = 0;

	virtual SVertex* GetVertex(unsigned long iVertex) = 0;	
	virtual unsigned long GetVertexCount(void) const = 0;
};



SP_NMSPACE_END
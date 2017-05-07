// ********************************************************************************************

//	SpeedPoint Resource Pool

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include "ITexture.h"
#include "IInstanceBuffer.h"
#include <Common\SPrerequisites.h>

SP_NMSPACE_BEG

struct S_API IVertexBuffer;
struct S_API IIndexBuffer;
struct S_API IRenderer;
struct S_API SColor;
struct S_API SMaterial;

struct S_API IResourcePool
{
protected:
	virtual ITypelessInstanceBuffer* CreateTypelessInstanceBuffer() = 0;
	virtual void OnInstanceBufferResourceCreated(IInstanceBufferResource* instanceBuffer) = 0;

public:
	virtual SResult Initialize(IRenderer* pRenderer) = 0;
	virtual SResult	ClearAll(void) = 0;

	virtual const string& GetResourceRootPath() const = 0;
	virtual void SetResourceRootPath(const string& rootSystemPath) = 0;
	
	// Returns the system path for given absolute resource path
	virtual string GetResourceSystemPath(const string& file) const = 0;

	// Summary:
	//		Returns the absolute resource path to the resource referenced by relPath
	// Arguments:
	//		relPath - The relative resource path, relative to absReference
	//		absReference - Absolute resource path to directory, relPath is relative to
	virtual string GetAbsoluteResourcePath(const string& relPath, const string& absReferenceDir) const = 0;


	// ------------------------------------------------------------------
	// Vertex Buffers:
public:
	virtual SResult	AddVertexBuffer(IVertexBuffer** pVBuffer) = 0;

	// Remove VB by given ptr. After deletion the ptr is set to 0
	virtual SResult RemoveVertexBuffer(IVertexBuffer** pVB) = 0;


	// ------------------------------------------------------------------
	// Index Buffers:
public:
	virtual SResult	AddIndexBuffer(IIndexBuffer** pIBuffer) = 0;

	// Remove IB by given ptr. After deletion the ptr is set to 0
	virtual SResult RemoveIndexBuffer(IIndexBuffer** pIB) = 0;


	// ------------------------------------------------------------------
	// Instance Buffers:
public:
	template<typename InstanceT, unsigned int chunkSz = 10>
	SResult AddInstanceBuffer(IInstanceBuffer<InstanceT>** pInstanceBuffer)
	{
		if (!pInstanceBuffer)
			return S_INVALIDPARAM;

		*pInstanceBuffer = new InstanceBufferWrapper<InstanceT, chunkSz>(CreateTypelessInstanceBuffer());

		OnInstanceBufferResourceCreated(*pInstanceBuffer);
		return S_SUCCESS;
	}

	template<typename InstanceT>
	SResult RemoveInstanceBuffer(IInstanceBuffer<InstanceT>** pInstanceBuffer)
	{
		return RemoveInstanceBuffer(reinterpret_cast<IInstanceBufferResource**>(pInstanceBuffer));
	}

	virtual SResult RemoveInstanceBuffer(IInstanceBufferResource** instanceBuffer) = 0;


	// ------------------------------------------------------------------
	// Textures:
public:
	// Summary:
	//		Returns a pointer to the texture object with the given specification and loads the texture if possible.
	// Description:
	//		If the specification is known, the pointer to the existing texture is returned.
	//		Otherwise, if specification is an absolute resource path, the texture is attempted to be loaded from disk during this call:
	//			- The loaded texture will then be non-dynamic and not staged.
	//			- No scaling of the texture will happen.
	//		If the texture could not be loaded, a pointer to a cleared new texture object is returned.
	// Parameters:
	//		specification - either the specification/path of an existing texture or the absolute resource path to the texture
	virtual ITexture* GetTexture(const string& specification) = 0;

	// Summary:
	//		Returns a pointer to the cube texture with given file name and loads it if it wasn't loaded yet.
	// Parameters:
	//		basePath - absolute resource path to the cube texture base name:   "/textures/sky" -> "/textures/sky_(pos|neg)(x|y|z).bmp"
	virtual ITexture* GetCubeTexture(const string& basePath) = 0;

	// Warning: The parameter of for each Handle() points to a ptr of ITexture which is destroyed
	// after Handle() returns.
	virtual SResult ForEachTexture(IForEachHandler<ITexture*>* pForEachHandler) = 0;

	// Returns a list of short description for each texture.
	// E.g. "grassland_n (res\\textures\\grassland_n.png)"
	virtual void ListTextures(vector<string>& list) const = 0;
};


SP_NMSPACE_END
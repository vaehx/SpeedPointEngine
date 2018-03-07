//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IInstanceBuffer.h"
#include "IResource.h"
#include <Common\SPrerequisites.h>

SP_NMSPACE_BEG

struct S_API ITexture;
struct S_API IVertexBuffer;
struct S_API IIndexBuffer;
struct S_API IRenderer;
struct S_API SColor;
struct S_API SMaterial;

//////////////////////////////////////////////////////////////////////////////////////////////////

enum S_API EAbsentTextureStrategy
{
	// If the specification is not known, a new texture object is created and additionally,
	// if the specification is an absolute resource path, the texture is attempted to load from disk.
	eTEX_ABSENT_CREATE_AND_ATTEMPT_LOAD = 0,

	// If the specification is not known, a new texture object is created.
	eTEX_ABSENT_CREATE_ONLY,

	// If the specification is not known, GetTexture() will return an invalid pointer
	eTEX_ABSENT_NOOP,
};

//////////////////////////////////////////////////////////////////////////////////////////////////

struct S_API IResourcePool
{
protected:
	virtual ITypelessInstanceBuffer* CreateTypelessInstanceBuffer() = 0;
	virtual void OnInstanceBufferResourceCreated(IInstanceBufferResource* instanceBuffer) = 0;

public:
	virtual SResult Initialize(IRenderer* pRenderer) = 0;
	virtual SResult	ClearAll(void) = 0;
	virtual void RunGarbageCollection() = 0;

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

	// ------------------------------------------------------------------
	// Index Buffers:
public:
	virtual SResult	AddIndexBuffer(IIndexBuffer** pIBuffer) = 0;


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


	// ------------------------------------------------------------------
	// Textures:
public:
	// Summary:
	//		Returns a pointer to the texture object with the given specification and loads the texture if possible.
	// Description:
	//		If the specification is known, the pointer to the existing texture is returned.
	//		Otherwise, the behavior of this method depends on the absentTextureStrategy parameter.
	// Parameters:
	//		specification - either the specification/path of an existing texture or the absolute resource path to the texture
	virtual ITexture* GetTexture(const string& specification, EAbsentTextureStrategy absentTextureStrategy = eTEX_ABSENT_CREATE_AND_ATTEMPT_LOAD) = 0;

	virtual ITexture* FindTexture(const string& specification) = 0;

	// Summary:
	//		Returns a pointer to the cube texture with given file name and loads it if it wasn't loaded yet.
	// Parameters:
	//		basePath - absolute resource path to the cube texture base name:   "/textures/sky" -> "/textures/sky_(pos|neg)(x|y|z).bmp"
	virtual ITexture* GetCubeTexture(const string& basePath) = 0;

	virtual void ForEachTexture(const std::function<void(ITexture*)>& fn) = 0;

	// Returns a list of short description for each texture.
	// E.g. "grassland_n (res\\textures\\grassland_n.png)"
	virtual void ListTextures(vector<string>& list) const = 0;
};


SP_NMSPACE_END
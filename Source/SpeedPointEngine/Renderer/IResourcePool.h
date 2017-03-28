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
	virtual void SetResourceRootPath(const string& path) = 0;
	virtual string GetResourcePath(const string& file) const = 0;

	virtual SResult	AddVertexBuffer(IVertexBuffer** pVBuffer) = 0;

	// Remove VB by given ptr. After deletion the ptr is set to 0
	virtual SResult RemoveVertexBuffer(IVertexBuffer** pVB) = 0;




	virtual SResult	AddIndexBuffer(IIndexBuffer** pIBuffer) = 0;

	// Remove IB by given ptr. After deletion the ptr is set to 0
	virtual SResult RemoveIndexBuffer(IIndexBuffer** pIB) = 0;


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



	// Summary:
	//		Adds an empty (dynamic) texture to the pool that can be modified later.
	// Parameters:
	//		specification - The texture specification. Should not be a filename.
	//		clearcolor - color to initialize the texture with
	virtual SResult AddTexture(const string& specification, ITexture** pTex, UINT w, UINT h, UINT miplevels = 1, const ETextureType& ty = eTEXTURE_R8G8B8A8_UNORM, const SColor& clearcolor = SColor()) = 0;

	// Summary:
	//		Returns a pointer to the texture object with the given specification.
	// Description:
	//		If the specification is known, the pointer to the existing texture is returned.
	//		Otherwise, the texture is attempted to be loaded from disk during this call.
	//			- The loaded texture will then be non-dynamic and not staged.
	//			- No scaling of the texture will happen.
	//		If the texture could not be loaded, a pointer to a texture object is returned,
	//		which is not loaded (cleared).
	// Parameters:
	//		specification - either the specification/filename of an existing texture or the filename to be loaded
	virtual ITexture* GetTexture(const string& specification) = 0;

	// Summary:
	//		Returns a pointer to the cube texture with given file name and loads
	//		it if it wasn't loaded yet.
	// Parameters:
	//		file - the name/path of the texture file relative to the set resource base path:   "assets\\sky" -> "assets\\sky_(pos|neg)(x|y|z).bmp"
	virtual ITexture* GetCubeTexture(const string& file) = 0;



	// Warning: The parameter of for each Handle() points to a ptr of ITexture which is destroyed
	// after Handle() returns.
	virtual SResult ForEachTexture(IForEachHandler<ITexture*>* pForEachHandler) = 0;

	// Returns a list of short description for each texture.
	// E.g. "grassland_n (res\\textures\\grassland_n.png)"
	virtual void ListTextures(vector<string>& list) const = 0;
};


SP_NMSPACE_END
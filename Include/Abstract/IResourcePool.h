// ********************************************************************************************

//	SpeedPoint Resource Pool

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include <string>

SP_NMSPACE_BEG

using std::string;

struct S_API IGameEngine;
struct S_API IVertexBuffer;
struct S_API IIndexBuffer;
struct S_API ITexture;
enum S_API ETextureType;
struct S_API IRenderer;
struct S_API SColor;
struct S_API SMaterial;

// SpeedPoint Resource Pool (abstract)
struct S_API IResourcePool
{
public:
	virtual SResult Initialize(IGameEngine* pEngine, IRenderer* pRenderer, const string& resourceBasePath = "") = 0;
	virtual SResult	ClearAll(void) = 0;



	virtual SResult	AddVertexBuffer(IVertexBuffer** pVBuffer) = 0;

	// Remove VB by given ptr. After deletion the ptr is set to 0
	virtual SResult RemoveVertexBuffer(IVertexBuffer** pVB) = 0;




	virtual SResult	AddIndexBuffer(IIndexBuffer** pIBuffer) = 0;

	// Remove IB by given ptr. After deletion the ptr is set to 0
	virtual SResult RemoveIndexBuffer(IIndexBuffer** pIB) = 0;




	// Summary:
	//		Makes sure the given texture file is loaded from disk
	//		If there is already a texture loaded with the same filename, the arguments are ignored and the existing pointer is returned.
	//		If the texture is streamed, the function will still return a pointer to an existing texture object.
	//		If the texture is loaded immediately and the texture could not be loaded, this function returns NULL!
	// Arguments:
	//		specification - the specification of the texture
	//		file - the path of the file relative to the resource base directory. If you don't need an extra specification, leave this empty, so the specification will be used as the file name
	//		w, h - desired pixel size. If w or h equals 0, the actual image size is used and no scaling is performed. If image size does not equal this size, a scale is performed.
	virtual SResult LoadTexture(const string& specification, ITexture** pTex, const string& file = "", UINT w = 0, UINT h = 0, bool bDynamic = false, bool bStaged = false) = 0;
	
	// Summary:
	//		Makes sre the given cube texture is loaded from disk
	//		If given, pTex will always be set to a valid pointer to an existing texture instance, although it may not be loaded yet.
	// Arguments:
	//		file - the name/path of the texture file relative to the set resource base path:   "assets\\sky" -> "assets\\sky_(pos|neg)(x|y|z).bmp"
	virtual SResult LoadCubeTexture(const string& specification, ITexture** pTex, const string& file = "", UINT w = 0, UINT h = 0) = 0;

	// Arguments:
	//	- clearcolor: Texture gets filled with this color. If type is depth map, clearcolor.r is used.
	virtual SResult AddTexture(const string& spec, ITexture** pTex, UINT w, UINT h, const ETextureType& ty, const SColor& clearcolor, bool bDynamic = false, bool bStaged = false) = 0;
	
	// Returns the texture object with the given specification. This will always be a valid pointer.
	virtual ITexture* GetTexture(const string& specification) = 0;

	// *pTex gets set to nullptr if removal succeeded
	virtual SResult RemoveTexture(ITexture** pTex) = 0;

	// Warning: The parameter of for each Handle() points to a ptr of ITexture which is destroyed
	// after Handle() returns.
	virtual SResult ForEachTexture(IForEachHandler<ITexture*>* pForEachHandler) = 0;
};


SP_NMSPACE_END
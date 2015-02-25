// ********************************************************************************************

//	SpeedPoint Resource Pool

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include <SPrerequisites.h>

SP_NMSPACE_BEG

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
	virtual SResult Initialize(IGameEngine* pEngine, IRenderer* pRenderer) = 0;
	virtual SResult	ClearAll(void) = 0;



	virtual SResult	AddVertexBuffer(IVertexBuffer** pVBuffer) = 0;

	// Remove VB by given ptr. After deletion the ptr is set to 0
	virtual SResult RemoveVertexBuffer(IVertexBuffer** pVB) = 0;




	virtual SResult	AddIndexBuffer(IIndexBuffer** pIBuffer) = 0;

	// Remove IB by given ptr. After deletion the ptr is set to 0
	virtual SResult RemoveIndexBuffer(IIndexBuffer** pIB) = 0;




	// w, h is the desired size. When source image does not equal this size, a scale is performed
	virtual SResult LoadTexture(const SString& file, UINT w, UINT h, const SString& spec, ITexture** pTex) = 0;

	// Arguments:
	//	- clearcolor: Texture gets filled with this color. If type is depth map, clearcolor.r is used.
	virtual SResult AddTexture(UINT w, UINT h, const SString& spec, const ETextureType& ty, const SColor& clearcolor, ITexture** pTex, bool bDynamic = false) = 0;

	// is case-sensitive
	virtual ITexture* GetTexture(const SString& spec) = 0;
	virtual SString GetTextureSpecification(const ITexture* t) const = 0;

	// *pTex gets set to nullptr if removal succeeded
	virtual SResult RemoveTexture(ITexture** pTex) = 0;

	// Warning: The parameter of for each Handle() points to a ptr of ITexture which is destroyed
	// after Handle() returns.
	virtual SResult ForEachTexture(IForEachHandler<ITexture*>* pForEachHandler) = 0;



	// The pointer, this function returns gets never invalidated until RemoveMaterial is called.
	// specifiction != name of material. The name of material is not used for indexing.
	// Specification is case insensitive.
	virtual SMaterial* AddNewMaterial(const SString& specification) = 0;

	// The pointer, this function returns gets never invalidated until RemoveMaterial is called.
	// specifiction != name of material. The name of material is not used for indexing.
	// Specification is case insensitive.
	virtual SMaterial* GetMaterial(const SString& specification) = 0;

	// *pMat is set to 0 if removal succeeded
	virtual void RemoveMaterial(SMaterial** pMat) = 0;
};


SP_NMSPACE_END
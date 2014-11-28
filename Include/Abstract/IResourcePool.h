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
struct S_API IRenderer;

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
	virtual SResult AddTexture(UINT w, UINT h, const SString& spec, S_TEXTURE_TYPE ty, ITexture** pTex) = 0;

	// is case-sensitive
	virtual ITexture* GetTexture(const SString& spec) = 0;
	virtual SString GetTextureSpecification(const ITexture* t) const = 0;

	virtual SResult RemoveTexture(ITexture** pTex) = 0;

	// Warning: The parameter of for each Handle() points to a ptr of ITexture which is destroyed
	// after Handle() returns.
	virtual SResult ForEachTexture(IForEachHandler<ITexture*>* pForEachHandler) = 0;

};


SP_NMSPACE_END
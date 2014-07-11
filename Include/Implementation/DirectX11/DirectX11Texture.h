//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	written by Pascal R. aka iSmokiieZz
//	(c) 2011-2014, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SPrerequisites.h>
#include "DirectX11.h"
#include <Abstract\ITexture.h>

SP_NMSPACE_BEG

class S_API SpeedPointEngine;
struct S_API IRenderer;
class S_API DirectX11Renderer;

// IWICImagineFactory must not be forward declared in SpeedPoint NameSpace!
SP_NMSPACE_END
struct IWICImagingFactory;
SP_NMSPACE_BEG


template<class T> class S_API ScopedTextureLoadingObject
{
public:
	explicit ScopedTextureLoadingObject(T *p = 0) : _pointer(p) {}
	~ScopedTextureLoadingObject()
	{
		if (_pointer)
		{
			_pointer->Release();
			_pointer = nullptr;
		}
	}

	bool IsNull() const { return (!_pointer); }

	T& operator*() { return *_pointer; }
	T* operator->() { return _pointer; }
	T** operator&() { return &_pointer; }

	void Reset(T *p = 0) { if (_pointer) { _pointer->Release(); } _pointer = p; }

	T* Get() const { return _pointer; }

private:
	ScopedTextureLoadingObject(const ScopedTextureLoadingObject&);
	ScopedTextureLoadingObject& operator=(const ScopedTextureLoadingObject&);

	T* _pointer;
};




class S_API DirectX11Texture : ITexture
{
private:
	SString	m_Specification;
	SpeedPointEngine* m_pEngine;
	S_TEXTURE_TYPE m_Type;
	bool m_bDynamic;
	ID3D11Texture2D* m_pDXTexture;	
	DirectX11Renderer* m_pDXRenderer;
	ID3D11ShaderResourceView* m_pDXSRV;

public:		
	DirectX11Texture();
	DirectX11Texture(const DirectX11Texture& o);

	~DirectX11Texture();
	

	virtual SResult Initialize(SpeedPointEngine* pEngine, const SString& spec);
	
	virtual SResult Initialize(SpeedPointEngine* pEngine, const SString& spec, bool bDynamic);
	
	virtual SResult LoadFromFile(int w, int h, int mipLevels, char* cFileName);	
	virtual SResult CreateEmpty(int w, int h, int mipLevels, S_TEXTURE_TYPE type, SColor clearcolor);	
	virtual SString GetSpecification(void);

	// ----
	
	virtual S_TEXTURE_TYPE GetType(void);	
	virtual SResult GetSize(int* pW, int* pH);	
	virtual SResult Clear(void);


	// DX
	ID3D11ShaderResourceView* D3D11_GetSRV()
	{
		return m_pDXSRV;
	}

private:
	static size_t BitsPerPixel(REFGUID targetGuid, IWICImagingFactory* pWIC);
};

SP_NMSPACE_END
//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2016 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\ITexture.h"
#include <Common\SPrerequisites.h>
#include "DX11.h"

SP_NMSPACE_BEG

class S_API DX11Renderer;

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



struct S_API SLoadedCubemapSide
{	
	unsigned char* pBuffer;
	size_t imageStride;
	size_t imageSize;

	SLoadedCubemapSide()
		: pBuffer(0), imageStride(0), imageSize(0)
	{
	}
};


class S_API DX11Texture : public ITexture
{
private:
	DX11Renderer* m_pDXRenderer;
	string	m_Specification;
	ETextureType m_Type;
	bool m_bDynamic;
	bool m_bStaged;
	ID3D11Texture2D* m_pDXTexture;
	ID3D11Texture2D* m_pDXStagingTexture; // for array
	D3D11_TEXTURE2D_DESC m_DXTextureDesc;
	ID3D11ShaderResourceView* m_pDXSRV;
	D3D11_SHADER_RESOURCE_VIEW_DESC m_DXSRVDesc;

	bool m_bIsCubemap;
	bool m_bArray;
	bool m_bLocked;
	void* m_pLockedData;
	unsigned int m_nLockedBytes;

	void* m_pStagedData;

	void Clear();
	bool CheckMipMapAutogenSupported(DXGI_FORMAT format);
	SResult CreateEmptyIntrnl(unsigned int arraySize, unsigned int w, unsigned int h, unsigned int mipLevels, ETextureType type, SColor clearcolor);

public:
	DX11Texture(DX11Renderer* pDXRenderer);
	DX11Texture() : DX11Texture(0) {}

	virtual ~DX11Texture();

	void D3D11_SetRenderer(DX11Renderer* renderer) { m_pDXRenderer = renderer; }


	// Initialization
public:
	virtual SResult LoadFromFile(const string& filePath, unsigned int w = 0, unsigned int h = 0, unsigned int mipLevels = 0);
	virtual SResult LoadCubemapFromFile(const string& basePath, unsigned int singleW = 0, unsigned int singleH = 0);
	virtual SResult CreateEmpty(unsigned int w, unsigned int h, unsigned int mipLevels, ETextureType type, SColor clearcolor);
	
	virtual SResult CreateEmptyArray(unsigned int count, unsigned int w, unsigned int h,
		ETextureType type = eTEXTURE_R8G8B8A8_UNORM, unsigned int mipLevels = 1, SColor clearcolor = SColor::Black());
	
	virtual SResult LoadArraySliceFromFile(unsigned int i, const string& filePath);
	virtual SResult FillArraySlice(unsigned int i, const SColor& color);
	virtual SResult ResizeArray(unsigned int count);

	// Texture will be non-dynamic and non-staged.
	// format - If DXGI_FORMAT_UNKNOWN, the same resource format will be used.
	SResult D3D11_InitializeFromExistingResource(ID3D11Texture2D* pResource, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN);

public:
	void SetSpecification(const string& specification);
	virtual const string& GetSpecification(void) const;
	virtual ETextureType GetType(void);
	virtual SResult GetSize(unsigned int* pW, unsigned int* pH);
	virtual unsigned int GetArraySize() const;

	virtual bool IsInitialized() const;
	virtual bool IsDynamic() const { return m_bDynamic; }
	virtual bool IsStaged() const { return m_bStaged; }
	virtual bool IsCubemap() const { return IsInitialized() && m_bIsCubemap; }
	virtual bool IsArray() const { return m_bArray; }


	// Data access and manipulation
public:
	virtual SResult Lock(void **pPixels, unsigned int* pnPixels, unsigned int* pnRowPitch = 0, unsigned int iArraySlice = 0);
	virtual SResult Unlock();
	virtual bool IsLocked() const { return m_bLocked; }

	virtual SResult Fill(SColor color);
	virtual SResult SampleStaged(const Vec2f& texcoords, void* pData) const;
	virtual SResult SampleStagedBilinear(Vec2f texcoords, void* pData) const;
	virtual void* GetStagedData();


	// DX
	ID3D11ShaderResourceView* D3D11_GetSRV()
	{
		return m_pDXSRV;
	}

private:
	static size_t BitsPerPixel(REFGUID targetGuid, IWICImagingFactory* pWIC);
	static void GetCubemapImageName(string& name, ECubemapSide side);
	static unsigned int GetDXCubemapArraySlice(ECubemapSide side);

	// If w == 0 or h == 0, the actual file size is used as the size and no scaling will happen. w and h reference values will be updated with the actual sizes
	SResult LoadTextureImage(const string& cFileName, unsigned int& w, unsigned int& h, unsigned char** pBuffer, size_t& imageStride, size_t& imageSize, DXGI_FORMAT& loadedFormat);
};

SP_NMSPACE_END

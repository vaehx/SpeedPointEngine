#pragma once

#include "SPrerequisites.h"

#include <comdef.h>
#include <d2d1.h>
#include <wincodec.h>
#include <memory>

SP_NMSPACE_BEG

// IWICImagineFactory must not be forward declared in SpeedPoint NameSpace!
SP_NMSPACE_END
struct IWICImagingFactory;
SP_NMSPACE_BEG

S_API const char* GetWICPixelFormatName(WICPixelFormatGUID fmt);

struct S_API SLoadedImage
{
	unsigned int width;
	unsigned int height;
	unsigned int bytePerPixel;
	unsigned char* buffer;
	size_t imageStride;
	size_t imageSize;
	WICPixelFormatGUID pixelFormat;
};

class S_API CImageLoader
{
private:
	static size_t BitsPerPixel(REFGUID targetGuid, IWICImagingFactory* pWIC);

	template<class T> class S_API ScopedObject
	{
	public:
		explicit ScopedObject(T *p = 0) : _pointer(p) {}
		~ScopedObject()
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
		ScopedObject(const ScopedObject&);
		ScopedObject& operator=(const ScopedObject&);

		T* _pointer;
	};


public:
	// Description:
	//	Loads the given image from file and fills the given image structure
	//  The buffer in the filled image structure is created with new. You can use ClearLoadedImage() to free the memory
	// Arguments:
	//	file - absolute system path to file
	//	scaleToWidth, scaleToHeight - If 0, will use size of actual image. Otherwise, the image will be rescaled if possible
	//	convertToFmt - If possible, the image will be converted to this format. If not set, this will use format of image file
	static SResult Load(const string& file, SLoadedImage* pImage,
		unsigned int scaleToWidth = 0, unsigned int scaleToHeight = 0,
		WICPixelFormatGUID convertToFmt = GUID_WICPixelFormatUndefined);

	static void ClearLoadedImage(SLoadedImage* pImage);
};

SP_NMSPACE_END

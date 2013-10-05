// ******************************************************************************************

//	SpeedPoint DirectX9 Framebuffer Object

// ******************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\SFrameBuffer.h>
#include <d3d9.h>

namespace SpeedPoint
{
	// SpeedPoint DirectX9 Framebuffer Object
	class S_API SDirectX9FrameBuffer : public SFrameBuffer		 
	{
	public:
		SpeedPointEngine*	pEngine;
		LPDIRECT3DTEXTURE9	pTexture;
		LPDIRECT3DSURFACE9	pSurface;		

		SResult Initialize( SpeedPointEngine* pEng, int nW, int nH );
		void Clear( void );
	};
}
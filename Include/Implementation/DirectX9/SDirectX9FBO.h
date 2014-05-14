// ******************************************************************************************

//	SpeedPoint DirectX9 Framebuffer Object

// ******************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\IFBO.h>
#include <d3d9.h>

namespace SpeedPoint
{

	// Forward declarations
	class S_API SpeedPointEngine;

	// SpeedPoint DirectX9 Framebuffer Object
	class S_API SDirectX9FrameBuffer : public IFBO	 
	{
	public:
		SpeedPointEngine*	pEngine;
		LPDIRECT3DTEXTURE9	pTexture;
		LPDIRECT3DSURFACE9	pSurface;		

		virtual SResult Initialize( SSpeedPointEngine* pEngine, IRenderer* pRenderer, int nW, int nH );
		virtual bool IsInitialized();
		virtual void Clear( void );
		
		virtual SSpeedPointEngine* GetEngine()
		{
			return pEngine;
		}

		// just use this function as a proxy to SSpeedPointEngine::GetRenderer
		virtual IRenderer* GetRenderer()
		{			
			if (pEngine == 0) return 0;
			return pEngine->GetRenderer();
		}
	};

	typedef class S_API SDirectX9FrameBuffer SDirectX9FBO;
}
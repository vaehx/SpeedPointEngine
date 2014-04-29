// ********************************************************************************************

//	SpeedPoint DirectX9 Output Plane

// ********************************************************************************************

#pragma once
#include <Abstract\IOutputPlane.h>
#include "SDirectX9VertexBuffer.h"
#include "SDirectX9IndexBuffer.h"
#include "SDirectX9Shader.h"
#include <d3d9.h>
#include <Util\SMatrix.h>

namespace SpeedPoint
{
	// SpeedPoint DirectX9 Output Plane
	class S_API SDirectX9OutputPlane : public IOutputPlane
	{
	public:		
		SpeedPointEngine*		pEngine;
		SDirectX9Renderer*		pDXRenderer;
		SDirectX9VertexBuffer		vertexBuffer;
		SDirectX9IndexBuffer		indexBuffer;
		SDirectX9Shader			mergeShader;
		SMatrix				mProjection;
		SMatrix				mView;		

		// Default constructor
		SDirectX9OutputPlane()
			: pEngine(NULL),
			pDXRenderer(NULL)
		{
		}

		// Initialize the output plane
		SResult Initialize(SpeedPointEngine* eng, IRenderer* renderer, int nW, int nH);

		// Render the output plane using the renderer specified when initializing
		SResult Render(IFrameBuffer* pGBufferAlbedo, IFrameBuffer* pLightingBuffer);

		// Clearout the output plane
		SResult Clear(void);
	};
}
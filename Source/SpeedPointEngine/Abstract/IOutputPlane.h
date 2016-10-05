// ********************************************************************************************

//	SpeedPoint Renderer specific Output Plane (abstract)

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once

#include "SPrerequisites.h"
#include "Matrix.h"

namespace SpeedPoint
{

	// Forward declarations
	struct S_API IFBO;
	class S_API SpeedPointEngine;
	struct S_API IRenderer;
	struct S_API IIndexBuffer;
	struct S_API IVertexBuffer;


	// SpeedPoint Renderer specific Output Plane (abstract)
	class S_API IOutputPlane
	{
	public:
		// Initialize the output plane
		virtual SResult Initialize( SpeedPointEngine* pEngine, IRenderer* pRenderer, int nW, int nH ) = 0;

		virtual bool IsInitialized() = 0;

		// Render the output plane
		virtual SResult Render( IFBO* pGBufferAlbedo, IFBO* pLightingBuffer ) = 0;

		// Clearout the output plane
		virtual SResult Clear( void ) = 0;
		
		virtual IVertexBuffer* GetVertexBuffer() = 0;
		virtual IIndexBuffer* GetIndexBuffer() = 0;

		virtual SMatrix& GetProjectionMatrix() = 0;
		virtual SMatrix& GetViewMatrix() = 0;

		virtual usint32 GetIndexCount() = 0;		
	};
}
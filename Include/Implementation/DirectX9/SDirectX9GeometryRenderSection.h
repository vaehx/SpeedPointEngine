// ********************************************************************************************

//	SpeedPoint DirectX9 Geometry Render Section

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>

namespace SpeedPoint
{
	// SpeedPoint DirectX9 Geometry Render Section
	class S_API SDirectX9GeometryRenderSection
	{
	public:
		SpeedPointEngine*	pEngine;
		SDirectX9Renderer*	pDXRenderer;
		UINT			nCurrentPasses;	// Count of passes of the current technique
		SSolid*			pSolid;		// The solid to be rendered currently

		// Default constructor
		SDirectX9GeometryRenderSection()
			: pEngine( NULL ),
			pDXRenderer( NULL ),
			pSolid( NULL ) {};

		// Initialize
		SResult Initialize( SpeedPointEngine* eng, SRenderer* pRenderer );

		// Prepare render data of a single solid
		SResult PrepareSection( SSolid* pSolid );

		// Prepare the GBuffer creation shader for rendering textured or not-textured primitives
		SResult PrepareGBufferCreationShader( bool bTextured );

		// Render a textured Primitive
		SResult RenderTexturedPrimitive( UINT iPrimitive );

		// Render a not-textured Primitive
		SResult RenderUntexturedPrimitive( UINT iPrimitive );

		// Render the geometry of a single primitive
		SResult RenderPrimitiveGeometry( SPrimitive* pPrimitive );

		// Exit the GBuffer creation shader
		SResult ExitGBufferCreationShader( void );

		// Clear
		SResult Clear( void );
	};
}
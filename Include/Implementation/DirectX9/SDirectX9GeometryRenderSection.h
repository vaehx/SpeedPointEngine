// ********************************************************************************************

//	SpeedPoint DirectX9 Geometry Render Section

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>
#include <Abstract\Pipelines\IRenderPipelineSection.h>
#include "SDirectX9Shader.h"
#include "SDirectX9FBO.h"

namespace SpeedPoint
{
	// SpeedPoint DirectX9 Geometry Render Section
	class S_API SDirectX9GeometryRenderSection : public IRenderPipelineSection
	{
	private:
		SpeedPointEngine*		m_pEngine;
		SDirectX9RenderPipeline*	m_pDX9RenderPipeline;
		usint32 			m_nCurrentPasses;	// Count of passes of the current technique		
		ISolid*				m_pCurrentSolid;	// current solid for which the Shader inputs are configured

		SDirectX9Shader m_gBufferShader;
		SDirectX9FBO m_GBufferPosition;	// in world space
		SDirectX9FBO m_GBufferNormals;	// in world space
		SDirectX9FBO m_GBufferTangents;	// in world space		
		SDirectX9FBO m_GBufferAlbedo;

	// Initialization / Destruction
	public:
		// Default constructor
		SDirectX9GeometryRenderSection();

		// Default Destructor
		~SDirectX9GeometryRenderSection();

		// Initialize
		virtual SResult Initialize(SpeedPointEngine* eng, IRenderPipeline* pRenderPipeline);

		// Clearout the section
		virtual SResult Clear(void);

		// Prepare render targets for the geometry section
		SResult PrepareSection();

		// Exit the GBuffer creation shader
		SResult EndSection(void);

	// Render calls
	public:
//~~~~~~~~~~~~
// TODO: Support rendering of multiple solids with one call
//~~~~~~~~~~~~

		// Prepare the GBuffer creation shader for rendering textured or not-textured primitives of one solid
		SResult PrepareShaderInput(ISolid* pSolid, bool bTextured);		

		// Render the geometry of ALL primitives of a solid to the current G-Buffer
		// Loops through all passes and calls Render(Un)TexturedPrimitive()
		SResult RenderSolidGeometry(ISolid* pSolid, bool bTextured);

		// Render a textured Primitive
		SResult RenderTexturedPrimitive(UINT iPrimitive);

		// Render a not-textured Primitive
		SResult RenderUntexturedPrimitive(UINT iPrimitive);

		// Render the geometry of a single primitive
		// Normally called by RenderTexturedPrimitive() and RenderUntexturedPrimitive()
		SResult RenderPrimitiveGeometry(SPrimitive* pPrimitive);		

		// Free the shader from the current input
		SResult FreeShaderInput();

	// Getter / Setter
	public:
		SDirectX9FBO* GetAlbedoFBO() { return &m_GBufferAlbedo; }
		
		// Normals are in world space
		SDirectX9FBO* GetNormalsFBO() { return &m_GBufferNormals; }

		SDirectX9FBO* GetPositionsFBO() { return &m_GBufferPosition; }
		
		// Tangents are in world space
		SDirectX9FBO* GetTangentsFBO() { return &m_GBufferTangents; }

		usint32 GetCurrentPassCount() { return m_nCurrentPasses; }
	};
}
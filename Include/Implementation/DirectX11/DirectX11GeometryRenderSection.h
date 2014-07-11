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
#include <Abstract\Pipelines\IRenderPipelineSection.h>
#include "DirectX11Effect.h"
#include "DirectX11FBO.h"

SP_NMSPACE_BEG


//////////////////////////////////////////////////////////////////////////////////////////////////

class S_API DirectX11RenderPipeline;
struct S_API ISolid;
struct S_API IRenderPipeline;
struct S_API SPrimitive;

//////////////////////////////////////////////////////////////////////////////////////////////////

class S_API DirectX11GeometryRenderSection : public IRenderPipelineSection
{
private:
	SpeedPointEngine* m_pEngine;
	DirectX11RenderPipeline* m_pDXRenderPipeline;
	usint32 m_nCurrentPasses;	// Count of passes of the current technique		
	ISolid* m_pCurrentSolid;	// current solid for which the Shader inputs are configured

	DirectX11Effect m_gBufferShader;
	DirectX11FBO m_GBufferPosition;	// in world space
	DirectX11FBO m_GBufferNormals;	// in world space
	DirectX11FBO m_GBufferTangents;	// in world space		
	DirectX11FBO m_GBufferAlbedo;

	ID3D11Buffer* m_pConstantsBuffer;	// for the matrices



	// Initialization / Destruction
public:	
	DirectX11GeometryRenderSection();	
	~DirectX11GeometryRenderSection();
	
	virtual SResult Initialize(SpeedPointEngine* eng, IRenderPipeline* pRenderPipeline);	
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
	DirectX11FBO* GetAlbedoFBO() { return &m_GBufferAlbedo; }

	// Normals are in world space
	DirectX11FBO* GetNormalsFBO() { return &m_GBufferNormals; }

	DirectX11FBO* GetPositionsFBO() { return &m_GBufferPosition; }

	// Tangents are in world space
	DirectX11FBO* GetTangentsFBO() { return &m_GBufferTangents; }

	usint32 GetCurrentPassCount() { return m_nCurrentPasses; }
};




//////////////////////////////////////////////////////////////////////////////////////////////////

SP_NMSPACE_END
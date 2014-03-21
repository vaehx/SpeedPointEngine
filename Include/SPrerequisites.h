// ******************************************************************************************

//	SpeedPoint Prerequisites

// ******************************************************************************************

#pragma once
#include "SAPI.h"
#include "SResult.h"
#include "SPoolIndex.h"
#include "SString.h"

#ifndef _NO_WINDOWS_HEADER
#include <Windows.h>
#else
typedef unsigned int UINT;
#endif

// Structure of this file:
// It is mainly structured into the main projects, the SpeedPoint engine contains
// Normally, each non-general class needs to have an Abstract version
// This abstract version is always on top of the current list. Below are the implementations.
// Like this:
// /*abs*/ class A_Abstract;
//	   class A_Implementation1;
// /*abs*/ class B_Abstract;
//	   class B_Implementation1;
//	   class B_Implementation2;
//
// Please keep this structure!

#define SP_UNIQUE unsigned int
#define SP_TRIVIAL -1

namespace SpeedPoint
{

	class	S_API SpeedPointEngine;

	// ---------------------------- General ---------------------------------
	struct	S_API SPrimitive;
	struct	S_API SColor;	
	class	S_API SpeedPointEngine;
	struct	S_API SVector3;
	struct	S_API SVector2;
	struct	S_API SMatrix;
	class	S_API SPlane;	
	class	S_API STransformable;
	class	S_API SCamera;		
	struct	S_API SVertex;
	struct	S_API SMaterial;			// derives from SPhysicsMaterial
	class	S_API SBoundBox;
	struct	S_API SSettings;			// contains SRendererSettings

	// ---------------------------- Logging ---------------------------------	
	//class	S_API SLogStream;

	// --------------------------- Settings ---------------------------------
/*abs*/ class	S_API SRendererSettings;
	class	S_API SDirectX9Settings;
	class	S_API SDirectX10Settings;
	class	S_API SDirectX11Settings;
	class	S_API SOpenGLSettings;	


	// ---------------------------- Lighting -------------------------------
	enum	S_API S_LIGHT_TYPE;
	class	S_API SLight;
	class	S_API SLightSystem;

	// ---------------------------- Animation -------------------------------
/*abs*/	class	S_API SAnimationBundle;
	class	S_API SBasicAnimationBundle;
/*abs*/ class	S_API SAnimationSequence;
	class	S_API SBasicAnimationSequence;
/*abs*/ class	S_API SAnimationKey;
	class	S_API SBasicAnimationKey;
/*abs*/ class	S_API SAnimationJoint;
	class	S_API SBasicAnimationJoint;
	
	// --------------------------- Geometry ---------------------------------
/*abs*/	class	S_API SSolid;
	class	S_API SBasicSolid;
/*abs*/	class	S_API SSolidSystem;
	class	S_API SBasicSolidSystem;
/*abs*/	class	S_API STerrain;
	class	S_API SBasicTerrain;
/*abs*/	class	S_API SModel; // IS THIS ACTUALLY AN ABSTRACT??
/*abs*/	class	S_API SEntity; // IS THIS ACTuALLY AN ABSTRACT?
/*abs*/	class	S_API SInteractive; // IS THIS AACTuALLLY AN ABSTRACt??

	// --------------------------- Octree ----------------------------------
/*abs*/ class	S_API SOctree;
	class	S_API SBasicOctree;
/*abs*/ class	S_API SOctreeNode;
	class	S_API SBasicOctreeNode;
	
	// ------------------------------- AI -----------------------------------
/*abs*/ class	S_API SBot;
	class	S_API SBasicBot;
	class	S_API SWaynode;
/*abs*/ class	S_API SWaynet;
	class	S_API SBasicWaynet;

	// ----------------------------- Sound ----------------------------------
/*abs*/ class	S_API SSoundSystem;
	class	S_API SDirectSoundSystem;
/*abs*/ class	S_API SSound;			// This is purely a sound resource!
	class	S_API SDirectSoundResource;

	// ---------------------------- Resources -------------------------------
/*abs*/	class	S_API SResourcePool;		// Update notice: combined with SResourceAccess in SPv2.0
	class	S_API SDirectX9ResourcePool;
	class	S_API SDirectX10ResourcePool;
	class	S_API SDirectX11ResourcePool;
	class	S_API SOpenGLResourcePool;
/*abs*/ class	S_API SIndexBuffer;
	class	S_API SDirectX9IndexBuffer;
	class	S_API SDirectX10IndexBuffer;
	class	S_API SDirectX11IndexBuffer;
	class	S_API SOpenGLIndexBuffer;
/*abs*/ class	S_API SVertexBuffer;
	class	S_API SDirectX9VertexBuffer;
	class	S_API SDirectX10VertexBuffer;
	class	S_API SDirectX11VertexBuffer;
	class	S_API SOpenGLVertexBuffer;
	enum	S_API S_TEXTURE_TYPE;
/*abs*/ class	S_API STexture;
	class	S_API SDirectX9Texture;
	class	S_API SDirectX10Texture;
	class	S_API SDirectX11Texture;
	class	S_API SOpenGLTexture;

	// --------------------------- Renderer --------------------------------
	enum	S_API S_RENDERER_TYPE;
/*abs*/ class	S_API SRenderer;	// holds the render pipeline
	class	S_API SDirectX9Renderer;
	class	S_API SDirectX10Renderer;
	class	S_API SDirectX11Renderer;
	class	S_API SOpenGLRenderer;
	enum	S_API S_RENDER_STATE;
/*abs*/ class	S_API SRenderPipeline;
	class	S_API SDirectX9RenderPipeline;
	class	S_API SDirectX10RenderPipeline;
	class	S_API SDirectX11RenderPipeline;
	class	S_API SOpenGLRenderPipeline;
/*abs*/ class	S_API SOutputPlane;
	class	S_API SDirectX9OutputPlane;
	class	S_API SDirectX10OutputPlane;
	class	S_API SDirectX11OutputPlane;
	class	S_API SOpenGLOutputPlane;

	class	S_API SDirectX9GeometryRenderSection;	

	// --------------------------- Effects ---------------------------------
/*abs*/ class	S_API SShader;
	class	S_API SDirectX9PixelShader;
	class	S_API SDirectX9VertexShader;	
	class	S_API SDirectX10PixelShader;
	class	S_API SDirectX10VertexShader;
	class	S_API SDirectX11PixelShader;
	class	S_API SDirectX11VertexShader;
	class	S_API SOpenGLPixelShader;
	class	S_API SOpenGLVertexShader;

	// -------------------------- Viewport ---------------------------------
/*abs*/ class	S_API SViewport;
	class	S_API SDirectX9Viewport;
	class	S_API SDirectX10Viewport;
	class	S_API SDirectX11Viewport;
	class	S_API SOpenGLViewport;

	// -------------------------- FrameBuffers -----------------------------
/*abs*/ class	S_API SFrameBuffer;
	class	S_API SDirectX9FrameBuffer;
	class	S_API SDirectX10FrameBuffer;
	class	S_API SDirectX11FrameBuffer;
	class	S_API SOpenGLFrameBuffer;

	// --------------------------- Particles -------------------------------
/*abs*/ class	S_API SParticle;
	struct	S_API SBasicParticle;
/*abs*/ class	S_API SParticleSystem;
	class	S_API SBasicParticleSystem;

	// ---------------------------- Physics --------------------------------
	enum	S_API S_PHYSICSMATERIAL_PRESET;
/*abs*/	class	S_API SPhysical;
	class	S_API SPhysicsBody;
/*abs*/ class	S_API SCollisionShape;
	class	S_API SPhysicsCollisionShape;
	class	S_API SPhysicsLiquid;
	class	S_API SPhysicsWind;
/*abs*/ class	S_API SPhysicalCollection;
	class	S_API SPhysicsSystem;
	struct	S_API SPhysicsMaterial;

	// ---------------------------- Scripting --------------------------------
	class	S_API RunPowderCommand;
	class	S_API RunPowderEnvironment;
	class	S_API RunPowderError;
	class	S_API RunPowderScript;
	class	S_API RunPowderVariable;


	// ======== ENUMERATIONS ============

	// SpeedPoint Interface Type
	enum S_API S_RENDERER_TYPE
	{
		S_DIRECTX9,
		S_DIRECTX10,
		S_DIRECTX11,
		S_OPENGL
	};

	// Present Quality (Present Interval)
	enum S_API S_PRESENT_QUALITY
	{
		S_PRESENT_HIGHQUALITY,
		S_PRESENT_MEDIUMQUALITY,
		S_PRESENT_LOWQUALITY
	};

	// SpeedPoint DepthStencil Surface quality
	enum S_API S_DEPTHSTENCIL_QUALITY
	{
		S_DEPTHSTENCIL_HIGHQUALITY,
		S_DEPTHSTENCIL_LOWQUALITY
	};

	// SpeedPoint MultiSample type
	enum S_API S_MULTISAMPLE_TYPE
	{
		S_MULTISAMPLE_NONE = 0,
		S_MULTISAMPLE_NONMASKABLE = 1,
		S_MULTISAMPLE_2 = 2,
		S_MULTISAMPLE_3 = 3,
		S_MULTISAMPLE_4 = 4,
		S_MULTISAMPLE_5 = 5,
		S_MULTISAMPLE_6 = 6,
		S_MULTISAMPLE_7 = 7,
		S_MULTISAMPLE_8 = 8,
		S_MULTISAMPLE_9 = 9,
		S_MULTISAMPLE_10 = 10,
		S_MULTISAMPLE_11 = 11,
		S_MULTISAMPLE_12 = 12,
		S_MULTISAMPLE_13 = 13,
		S_MULTISAMPLE_14 = 14,
		S_MULTISAMPLE_15 = 15,
		S_MULTISAMPLE_16 = 16
	};

	// SpeedPoint MultiSample Quality
	enum S_API S_MULTISAMPLE_QUALITY
	{
		S_MULTISAMPLEQUALITY_HIGH,
		S_MULTISAMPLEQUALITY_MEDIUM,
		S_MULTISAMPleQUALITY_LOW
	};

	// SpeedPoint Renderer Processing mode
	enum S_API S_PROCESSING_MODE
	{
		S_PROCESS_HARDWARE,
		S_PROCESS_SOFTWARE,
		S_PROCESS_MIXED
	};

	// SpeedPoint BackBuffer format
	enum S_API S_BACKBUFFER_FORMAT
	{
		S_BACKBUFFER32,
		S_BACKBUFFER16				
	};

	// SpeedPoint IndexBuffer Format
	enum S_API S_INDEXBUFFER_FORMAT
	{
		S_INDEXBUFFER_16,
		S_INDEXBUFFER_32,
	};

	// SpeedPoint RenderPipeline current state
	// this enumeration is also used to specify the render pipeline task
	enum S_API S_RENDER_STATE
	{
		S_RENDER_NONE,		// NoOp

		S_RENDER_BEGINFRAME,	// BeginScene()
		
		S_RENDER_GEOMETRY,	// Render solids -> create GBuffer		

		S_RENDER_LIGHTING,	// Render lights -> calculate Lighting Buffer
		
		S_RENDER_POST,	//// TODO // Render Post Effects (Blur, ...) -> ????
		
		S_RENDER_ENDFRAME,	// Render Output Plane -> create BackBuffer
		
		S_RENDER_PRESENT	// Present BackBuffer to viewport
	};

	// Command Buffer
	enum S_API S_COMMAND_TYPE
	{
		// GPU Commands
		S_CMD_GPU_BEGINFRAME,
		S_CMD_GPU_RENDERSOLID,
		S_CMD_GPU_RENDERPOST,
		S_CMD_GPU_PRESENT,
		S_CMD_GPU_ENDFRAME

		// CPU Commands
		////// TODO
	};

	// Type of the texture
	enum S_API S_TEXTURE_TYPE
	{
		S_TEXTURE_RGBA,
		S_TEXUTRE_RGBX,
		S_TEXTURE_FILE
	};

	// SpeedPoint Projection type
	enum S_API S_PROJECTION_TYPE
	{
		S_PROJECTION_PERSPECTIVE,
		S_PROJECTION_ORTHOGRAPHIC
	};

	// SpeedPoint Light Shape Type
	enum S_API S_LIGHT_TYPE
	{
		S_LIGHT_POINT,
		S_LIGHT_SPOT,
		S_LIGHT_DIRECTIONAL
	};

	// Primitive Type just to clarify
	enum S_API S_PRIMITIVE_TYPE
	{
		S_PRIM_COMPLEX,
		S_PRIM_COMPLEX_PLANE,
		S_PRIM_PARTICLE
	};

	// SpeedPoint Primitive render type
	enum S_API S_PRIMITIVE_RENDER_TYPE
	{
		S_PRIM_RENDER_TRIANGLELIST,
		S_PRIM_RENDER_LINELIST,
		S_PRIM_RENDER_POINTLIST,
		S_PRIM_RENDER_TRIANGLESTRIP
	};	

}
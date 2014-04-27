// ******************************************************************************************

//	SpeedPoint Game Engine Prerequisites

//	(c) 2011-2014 Pascal R. aka iSmokiieZz
//	All rights reserved.

//	The SpeedPoint Prerequisites file should be included by every header file in this
//	project. It contains all classes as forward declarations and offers easy pointer
//	addressing in the header files.

//	If this file grows too large, then split it into several files.

// ******************************************************************************************

#ifndef SPREREQUISITES_H
#define SPREREQUISITES_H

#pragma once

#include "Util\SAPI.h" // __declspec macro S_API

#include "Util\SWindowsSpecific.h"

#include "Util\SResult.h"	// SResult, IExceptionProxy and Assertion
#include "Util\SAssert.h"

#include "Util\SPoolIndex.h"
#include "Util\SString.h"

// ------------------------------------------------------------------------------------------
// Structure of this file:
// It is mainly structured into the main projects, the SpeedPoint engine contains
// Normally, each non-general class needs to have an Abstract version
// This abstract version is always on top of the current list. Below are the implementations.
// Like this:
// /*abs*/ class A_Abstract;
// /*abs*/ class B_Abstract;
//
//	   class A_Implementation1;
//	   class B_Implementation1;
//
//	   class A_Implementation2;
//	   class B_Implementation2;
//
// Please keep this structure!
// ------------------------------------------------------------------------------------------

#define SP_TRIVIAL -1

namespace SpeedPoint
{	
	typedef unsigned __int32 usint32; // Unsigned 32bit integer
	typedef unsigned __int64 usint64; // Unsigned 64bit integer
	typedef unsigned __int8 byte; // 1 Byte
	typedef float f32; // 32bit precision float value
	typedef double f64; // 64bit (double-)precision float value

	// SpeedPoint unique identifier index (unsigned)
	typedef usint32 SP_UNIQUE;

	// --------------------------- COMPONENTS --------------------------------

	class	S_API SSpeedPointEngine;	
	class	S_API SpeedPointEngine;	// same as SSpeedPointEngine (typedef)
	class	S_API SPhysicsEngine;
	
	class	S_API SFramePipeline;
	class	S_API SFrameEngine;	// same as SFramePipeline (typedef)
	class	S_API SDynamicsPipeline;

	// --------------------- Observing / Event handling -----------------------

	class	S_API SEventCallbackTable;
	class	S_API SEventCallbackTableEntry;
	class	S_API SEventHandler;
	class	S_API SEventParameter;
	class	S_API SEventParameters;
	class	S_API SObservedObject;	// the class, all pipelines should inherit from

	// ---------------------------- General -----------------------------------
	struct	S_API SPrimitive;
	struct	S_API SColor;		
	struct	S_API SVector3;
	struct	S_API SVector2;
	struct	S_API SMatrix;
	struct	S_API SMatrix4;
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
/*abs*/ class	S_API SSolidSystem;
/*abs*/	class	S_API STerrain;
/*abs*/	class	S_API SModel; // IS THIS ACTUALLY AN ABSTRACT??
/*abs*/	class	S_API SEntity; // IS THIS ACTuALLY AN ABSTRACT?
/*abs*/	class	S_API SInteractive; // IS THIS AACTuALLLY AN ABSTRACt??
	
	// ----
	// Basic Implementation of the Geometry classes, For Physical implementation see below!

	class	S_API SBasicSolid;
	class	S_API SBasicSolidSystem;
	class	S_API SBasicTerrain;

	// --------------------------- Octree ----------------------------------
/*abs*/ class	S_API SOctree;
/*abs*/ class	S_API SOctreeNode;

	class	S_API SBasicOctree;
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

/*abs*/ class	S_API SRenderPipelineSection;
	class	S_API SDirectX9GeometryRenderSection;	
	class	S_API SDirectX9LightingRenderSection;
	class	S_API SDirectX9PostRenderSection;

	// --------------------------- Effects ---------------------------------
/*abs*/ class	S_API SShader;
	class	S_API SDirectX9Shader;	
	class	S_API SDirectX10Shader;	
	class	S_API SDirectX11Shader;	
	class	S_API SOpenGLShader;	

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

	// SpeedPoint FramePipeline Stages	
	// This enumeration also contains the stages for Dynamics Pipeline and render pipeline!	
	enum S_API S_FRAMEPIPELINE_STAGE
	{
		// !! When adding a Framepipeline stage, make also sure to:
		// !!	- Add event
		// !!	- Add check for default skipstage in SSpeedPointEngine::Initialize()
		// !!	- Embed it into the Framepipeline source code	
		// !!	- Add skipstage flag

		S_BEGINFRAME = 0x01,

		S_DYNAMICS = 0x0100,
		S_DYNAMICS_ANIM = 0x0200,
		S_DYNAMICS_PHYSICS = 0x0300,
		S_DYNAMICS_INPUT = 0x0400,
		S_DYNAMICS_SCRIPT = 0x0500,		
		S_DYNAMICS_RENDERSCRIPT = 0x0600,

		S_RENDER = 0x1000,
		S_RENDER_BEGIN = 0x1100,
		S_RENDER_GEOMETRY = 0x1200,
		S_RENDER_LIGHTING = 0x1300,
		S_RENDER_POST = 0x1400,
		S_RENDER_PRESENT = 0x1500,

		S_ENDFRAME = 0x2000,

		S_IDLE = 0x4000	// frame pipeline waits for next BeginFrame()-Call
	};


	// Flag bytes: (hex)
	// Byte1: 01	02	04	08	10	20	40	80
	// Byte2: 100	200	400	800	1000	2000	4000	8000
	// Byte3: 10000	20000	40000	80000	100000	200000	400000	800000
	// Byte4: 1000000 2000000 4000000 8000000 10000000 20000000 40000000 80000000

	// Flag values for variable storing framepipeline stages to be skipped
	enum S_API S_FRAMEPIPELINE_STAGE_SKIP_FLAGS
	{	
		S_SKIP_IDLE = 0x00,	// frame pipeline waits for next BeginFrame()-Call

		S_SKIP_BEGINFRAME = 0x01,

		S_SKIP_DYNAMICS = 0x100,
		S_SKIP_DYNAMICS_ANIM = 0x200,
		S_SKIP_DYNAMICS_PHYSICS = 0x400,
		S_SKIP_DYNAMICS_INPUT = 0x800,
		S_SKIP_DYNAMICS_SCRIPT = 0x1000,
		S_SKIP_DYNAMICS_RENDERSCRIPT = 0x2000,

		S_SKIP_RENDER = 0x10000,
		S_SKIP_RENDER_BEGIN = 0x20000,
		S_SKIP_RENDER_GEOMETRY = 0x40000,
		S_SKIP_RENDER_LIGHTING = 0x80000,
		S_SKIP_RENDER_POST = 0x100000,
		S_SKIP_RENDER_PRESENT = 0x200000,

		S_SKIP_ENDFRAME = 0x1000000
	};

	// SpeedPoint FramePipeline State Events (for use with Observer System)
	enum S_API S_FRAMEPIPELINE_STAGE_EVENTS
	{
		S_E_BEGINFRAME = 0x10,

		S_E_DYNAMICS = 0x1000,
		S_E_DYNAMICS_ANIM = 0x1100,
		S_E_DYNAMICS_PHYSICS = 0x1200,
		S_E_DYNAMICS_INPUT = 0x1300,
		S_E_DYNAMICS_SCRIPT = 0x1400,
		S_E_DYNAMICS_RENDERSCRIPT = 0x1500,

		S_E_RENDER = 0x10000,
		S_E_RENDER_BEGIN = 0x11000,
		S_E_RENDER_GEOMETRY = 0x12000,
		S_E_RENDER_GEOMETRY_BEGIN = 0x12100,
		S_E_RENDER_GEOMETRY_CALLS = 0x12200,
		S_E_RENDER_GEOMETRY_END = 0x12300,
		S_E_RENDER_GEOMETRY_EXIT = S_E_RENDER_GEOMETRY_END,
		S_E_RENDER_LIGHTING = 0x13000,
		S_E_RENDER_LIGHTING_PREPARE = 0x13100,
		S_E_RENDER_POST = 0x14000,
		S_E_RENDER_PRESENT = 0x15000,	// = S_E_RENDER_END
		S_E_RENDER_END = 0x15000,	// = S_E_RENDER_PRESENT

		S_E_ENDFRAME = 0x100000,

		S_E_IDLE = 0x00,

		S_E_DESTRUCT = 0xffffffff
	};

	// SpeedPoint Render Pipeline Interface Type
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
} // namespace SpeedPoint

// ------------------------------------------------------------------------------------------

#endif // SPREREQUISITES_H

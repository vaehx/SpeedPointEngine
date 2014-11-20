//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine Prerequisites
//
//	(c) 2011-2014 Pascal R. aka iSmokiieZz
//	All rights reserved.
//
//	The SpeedPoint Prerequisites file should be included by every header file in this
//	project. It contains all classes as forward declarations and offers easy pointer
//	addressing in the header files.
//
//	If this file grows too large, then split it into several files.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SPREREQUISITES_H
#define SPREREQUISITES_H

#pragma once

// Shortenization for the namespace SpeedPoint declaration
#define SP_NMSPACE_BEG namespace SpeedPoint {
#define SP_NMSPACE_END }

//////////////////////////////////////////////////////////////////////////////////////////////////
// Important core headers.

#include "Util\SAPI.h" // __declspec macro S_API

#include "Util\SWindowsSpecific.h"
#include <vector>
#include <cmath>

#include "Util\SResult.h"	// SResult, IExceptionProxy
#include "Util\SAssert.h"

#include "Util\SPoolIndex.h"
#include "Util\SString.h"
// DO NOT ADD ANY HEADER HERE if you do not know why!Many things depend on it!





#define SP_TRIVIAL -1

// also check against wrong debug values if in debug mode
#ifdef _DEBUG
	#define IS_VALID_PTR(ptr) (ptr && (unsigned int)ptr != 0xC0000005 && (unsigned int)ptr != 0xCDCDCDCD && (unsigned int)ptr != 0xCCCCCCCC)
#else
	#define IS_VALID_PTR(ptr) (ptr)
#endif


#define SP_SAFE_RELEASE(x) if(IS_VALID_PTR(x)) { x->Release(); x = nullptr; }

// Delete an array safely, avoids memory heap violations if array has length 1
#define SP_SAFE_DELETE_ARR(x, len) \
	if(IS_VALID_PTR(x) && len == 1) delete x; else if((x) && len > 1) delete[] x;

// Clear and release all items in a vector. x has to be a vector.
#define SP_SAFE_RELEASE_CLEAR_VECTOR(x) \
	if((x).size() > 0) for(auto it = (x).begin(); it != (x).end(); ++it) SP_SAFE_RELEASE((*it));




// Bits - use this to define flags
#define BIT_1 1 << 0
#define BIT_2 1 << 1
#define BIT_3 1 << 2
#define BIT_4 1 << 3
#define BIT_5 1 << 4
#define BIT_6 1 << 5
#define BIT_7 1 << 6
#define BIT_8 1 << 7

#define BITN(n) 1 << (n - 1)








SP_NMSPACE_BEG

#define ILINE inline


//////////////////////////////////////////////////////////////////////////////////////////////////
// Basic datatypes

typedef unsigned __int8 usbyte; // 1 Byte
typedef unsigned __int16 usword;
typedef unsigned __int32 usint32; // Unsigned 32bit integer
typedef unsigned __int64 usint64; // Unsigned 64bit integer
typedef float f32; // 32bit precision float value
typedef double f64; // 64bit (double-)precision float value

// SpeedPoint unique identifier index (unsigned)
typedef usint32 SP_UNIQUE;


//////////////////////////////////////////////////////////////////////////////////////////////////
// Some math utility functions

static inline float cotf(float val) { return (1.0f / tanf(val)); }
static inline double cot(double val) { return (1.0f / tan(val)); }
static inline double cot(float val) { return (1.0f / tan((double)val)); }

#define SP_PI 3.14159265358979323f
#define SP_UNIT_DEG_RAD 0.01745329251994f
#define SP_UNIT_RAD_DEG 57.29577951308232f
#define SP_DEG_TO_RAD(deg) deg * SP_UNIT_DEG_RAD
#define SP_RAD_TO_DEG(rad) rad * SP_UNIT_RAD_DEG

//////////////////////////////////////////////////////////////////////////////////////////////////
/*

// !!

// Comment: I threw all the forward declarations out of the code because
//	we dont want to feign not defined classes. If you want to use forward declarations for ptrs
//	in a specific header file, then put it right there, please.

// !!

// --------------------------- COMPONENTS --------------------------------

class	S_API SSpeedPointEngine;
typedef class S_API SSpeedPointEngine SpeedPointEngine;
class	S_API SPhysicsEngine;

class	S_API SFramePipeline;
typedef class S_API SFramePipeline SFrameEngine;
class	S_API SDynamicsPipeline;

// --------------------- Observing / Event handling -----------------------

class	S_API SEventCallbackTable;
struct	S_API SEventCallbackTableEntry;
class	S_API SEventHandler;
struct	S_API SEventParameter;
class	S_API SEventParameters;
class	S_API SObservedObject;	// the class, all pipelines should inherit from

// ---------------------------- General -----------------------------------
struct	S_API SPrimitive;
struct	S_API SColor;
struct	S_API SVector3;
struct	S_API SVector2;
struct	S_API SMatrix;
typedef struct S_API SMatrix SMatrix4;
class	S_API SPlane;
class	S_API STransformable;
class	S_API SCamera;
struct	S_API SVertex;
struct	S_API SMaterial;			// derives from SPhysicsMaterial
class	S_API SBoundBox;
struct	S_API SSettings;			// contains IRendererSettings

// ---------------------------- Logging ---------------------------------	
class	S_API SLogStream;
typedef S_API void(*SLogHandler)(SResult, SString);

// --------------------------- Settings ---------------------------------
class	S_API IRendererSettings;
class	S_API SDirectX9Settings;
class	S_API SDirectX10Settings;
class	S_API DirectX11Settings;
class	S_API SOpenGLSettings;

// ---------------------------- Lighting -------------------------------
enum	S_API S_LIGHT_TYPE;
class	S_API SLight;
class	S_API SLightSystem;

// ---------------------------- Animation -------------------------------
class	S_API IAnimationBundle;
class	S_API SBasicAnimationBundle;
class	S_API IAnimationSequence;
class	S_API SBasicAnimationSequence;
class	S_API IAnimationKey;
class	S_API SBasicAnimationKey;
class	S_API IAnimationJoint;
class	S_API SBasicAnimationJoint;

// --------------------------- Geometry ---------------------------------
class	S_API ISolid;
class	S_API ISolidSystem;
class	S_API ITerrain;
class	S_API IModel; // IS THIS ACTUALLY AN ABSTRACT??
class	S_API IEntity; // IS THIS ACTuALLY AN ABSTRACT?
class	S_API IInteractive; // IS THIS AACTuALLLY AN ABSTRACt??

// ----
// Basic Implementation of the Geometry classes, For Physical implementation see below!

class	S_API SBasicSolid;
class	S_API SBasicSolidSystem;
class	S_API SBasicTerrain;

// --------------------------- Octree ----------------------------------
class	S_API IOctree;
class	S_API IOctreeNode;

class	S_API SBasicOctree;
class	S_API SBasicOctreeNode;

// ------------------------------- AI -----------------------------------
class	S_API IAI;	// AI Engine
class	S_API IBot;
class	S_API SBasicBot;
class	S_API SWaynode;
class	S_API IWaynet;
class	S_API SBasicWaynet;

// ----------------------------- Sound ----------------------------------
class	S_API ISoundSystem;
class	S_API SDirectSoundSystem;
class	S_API ISound;			// This is purely a sound resource!
class	S_API SDirectSoundResource;

// ---------------------------- Resources -------------------------------
class	S_API IResourcePool;		// Update notice: combined with SResourceAccess in SPv2.0
class	S_API SDirectX9ResourcePool;
class	S_API SDirectX10ResourcePool;
class	S_API DirectX11ResourcePool;

class	S_API SOpenGLResourcePool;
class	S_API IIndexBuffer;
class	S_API SDirectX9IndexBuffer;
class	S_API SDirectX10IndexBuffer;
class	S_API DirectX11IndexBuffer;
class	S_API SOpenGLIndexBuffer;
class	S_API IVertexBuffer;
class	S_API SDirectX9VertexBuffer;
class	S_API SDirectX10VertexBuffer;
class	S_API DirectX11VertexBuffer;
class	S_API SOpenGLVertexBuffer;
enum	S_API S_TEXTURE_TYPE;
class	S_API ITexture;
class	S_API SDirectX9Texture;
class	S_API SDirectX10Texture;
class	S_API DirectX11Texture;
class	S_API SOpenGLTexture;

// --------------------------- Renderer --------------------------------
enum	S_API S_RENDERER_TYPE;
class	S_API IRenderer;	// holds the render pipeline
class	S_API SDirectX9Renderer;
class	S_API SDirectX10Renderer;
class	S_API DirectX11Renderer;
class	S_API SOpenGLRenderer;
class	S_API IRenderPipeline;
class	S_API SDirectX9RenderPipeline;
class	S_API SDirectX10RenderPipeline;
class	S_API DirectX11RenderPipeline;
class	S_API SOpenGLRenderPipeline;
class	S_API IOutputPlane;
class	S_API SDirectX9OutputPlane;
class	S_API SDirectX10OutputPlane;
class	S_API DirectX11OutputPlane;
class	S_API SOpenGLOutputPlane;

class	S_API IRenderPipelineSection;

class	S_API SDirectX9GeometryRenderSection;
class	S_API SDirectX9LightingRenderSection;
class	S_API SDirectX9PostRenderSection;

class	S_API DirectX11GeometryRenderSection;
class	S_API DirectX11LightingRenderSection;
class	S_API DirectX11PostRenderSection;

class	S_API IRenderDeviceCapabilities;
class	S_API DirectX11RenderDeviceCaps;

// -------------------------- Command Buffering -------------------------



class S_API SCommandQueue;
struct S_API SCommandDescription;

struct S_API SDrawSolidData;
struct S_API SDrawPrimitiveData;

// --------------------------- Effects ---------------------------------
class	S_API IShader;
class	S_API SDirectX9Shader;
class	S_API SDirectX10Shader;
class	S_API DirectX11Shader;
class	S_API SOpenGLShader;

// -------------------------- Viewport ---------------------------------
class	S_API IViewport;
class	S_API SDirectX9Viewport;
class	S_API SDirectX10Viewport;
class	S_API DirectX11Viewport;
class	S_API SOpenGLViewport;

// -------------------------- FrameBuffers -----------------------------
class	S_API IFBO; // IFrameBufferObject
class	S_API SDirectX9FBO;
class	S_API SDirectX10FBO;
class	S_API DirectX11FBO;
class	S_API SOpenGLFBO;

// --------------------------- Particles -------------------------------
class	S_API IParticle;
struct	S_API SBasicParticle;
class	S_API IParticleSystem;
class	S_API SBasicParticleSystem;

// ---------------------------- Physics --------------------------------
enum	S_API S_PHYSICSMATERIAL_PRESET;
class	S_API IPhysical;
class	S_API SPhysicsBody;
class	S_API ICollisionShape;
class	S_API SPhysicICollisionShape;
class	S_API SPhysicsLiquid;
class	S_API SPhysicsWind;
class	S_API IPhysicalCollection;
class	S_API SPhysicsSystem;
struct	S_API SPhysicsMaterial;
class	S_API SPhysIntersectInfo;

// ---------------------------- Scripting --------------------------------
class	S_API IScriptEngine;	// Scripting Engine
class	S_API RunPowderCommand;
class	S_API RunPowderEnvironment;
class	S_API RunPowderError;
class	S_API RunPowderScript;
class	S_API RunPowderVariable;


*/

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
	S_SKIP_IDLE = 0x01,	// frame pipeline waits for next BeginFrame()-Call

	S_SKIP_BEGINFRAME = 0x02,

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
	S_E_RENDER_GEOMETRY_DRAWSOLID = 0x12201,
	S_E_RENDER_GEOMETRY_DRAWPRIMITIVE = 0x12202,
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

// how are the geometry drawcalls handled
enum S_API S_GEOMETRY_RENDER_INTERACTION_STRATEGY
{
	eGEOMRENDER_STRATEGY_COMMANDS,
	eGEOMRENDER_STRATEGY_EVENTS,
	eGEOMRENDER_STRATEGY_HYBRID
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

// SpeedPoint MultiSample count
enum S_API EMSAACount
{
	eMSAA_NONE = 0,
	eMSAA_NONMASKABLE = 1,
	eMSAA_2 = 2,
	eMSAA_3 = 3,
	eMSAA_4 = 4,
	eMSAA_5 = 5,
	eMSAA_6 = 6,
	eMSAA_7 = 7,
	eMSAA_8 = 8,
	eMSAA_9 = 9,
	eMSAA_10 = 10,
	eMSAA_11 = 11,
	eMSAA_12 = 12,
	eMSAA_13 = 13,
	eMSAA_14 = 14,
	eMSAA_15 = 15,
	eMSAA_16 = 16
};

// SpeedPoint MultiSample Quality
enum S_API EMSAAQuality
{
	eMSAAQUALITY_HIGH,
	eMSAAQUALITY_MEDIUM,
	eMSAAQUALITY_LOW
};

// SpeedPoint Renderer Vertex Processing mode (Hardware, Software, Mixed)
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

// Render command types used for the command buffering
enum S_API S_RENDER_COMMAND_TYPE
{
	eSRCMD_NONE,
	eSRCMD_DRAWSOLID,
	eSRCMD_DRAWPRIMITIVE
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
 
SP_NMSPACE_END

// ------------------------------------------------------------------------------------------

#endif // SPREREQUISITES_H

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

#define ILINE __forceinline

//////////////////////////////////////////////////////////////////////////////////////////////////
// Important core headers.

#include "SAPI.h" // __declspec macro S_API

#include <vector>
using std::vector;
#include <cmath>

#include "SResult.h"	// SResult, IExceptionProxy
#include "SAssert.h"	// including IS_VALID_PTR

#include "SPoolIndex.h"

#include <string>
#include "strutils.h"
#include <sstream>
using std::string;

#include <algorithm>
#include <functional>
#include <cctype>

#include "CLog.h"
// DO NOT ADD ANY HEADER HERE if you do not know why!




#define SP_TRIVIAL -1

#define RETURN_ON_ERR(exec) if (Failure((exec))) { return SpeedPoint::S_ERROR; }
#define SP_SAFE_RELEASE(x) if(IS_VALID_PTR(x)) { x->Release(); x = nullptr; }

// Delete an array if is valid ptr and len > 0 and sets ptr to 0
#define SP_SAFE_DELETE_ARR(x, len) \
	if(IS_VALID_PTR((x)) && (len) > 0) { delete[] (x); (x) = 0; }

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


//////////////////////////////////////////////////////////////////////////////////////////////////
// Basic datatypes

typedef unsigned __int8 usbyte; // 1 Byte
typedef unsigned __int16 usword;
typedef unsigned __int32 usint32; // Unsigned 32bit integer
typedef unsigned __int64 usint64; // Unsigned 64bit integer
typedef float f32; // 32bit precision float value
typedef double f64; // 64bit (double-)precision float value
typedef unsigned __int16 u16; // unsigned short
typedef unsigned __int32 u32; // unsigned int
typedef unsigned __int64 u64; // unsigned long

// SpeedPoint unique identifier index (unsigned)
typedef usint32 SP_UNIQUE;

#define MAX_UINT32 0xffffffffui32
//#define MAX_UINT64 0xffffffffffffffff

//////////////////////////////////////////////////////////////////////////////////////////////////
// Some math utility functions

static inline float cotf(float val) { return (1.0f / tanf(val)); }
static inline double cot(double val) { return (1.0f / tan(val)); }
static inline double cot(float val) { return (1.0f / tan((double)val)); }

#define SP_PI 3.14159265358979323f
#define SP_UNIT_DEG_RAD 0.01745329251994f
#define SP_UNIT_RAD_DEG 57.29577951308232f
#define SP_DEG_TO_RAD(deg) ((float)deg * SP_UNIT_DEG_RAD)
#define SP_RAD_TO_DEG(rad) rad * SP_UNIT_RAD_DEG

static inline bool IsPowerOfTwo(unsigned long n) { return (n & (n - 1)) == 0; }

// n^2
static inline unsigned long pow2(unsigned long n) { return n * n; }

// 2^n
static inline unsigned long PowerOfTwo(unsigned long n) { return (1 << n); }

//////////////////////////////////////////////////////////////////////////////////////////////////

enum S_API EForEachHandlerState
{
	FOREACH_CONTINUE = 0,
	FOREACH_BREAK = 1,
	FOREACH_ABORT_ERR = 2
};

template<typename T>
struct S_API IForEachHandler
{
public:
	ILINE virtual EForEachHandlerState Handle(const T* t, void* pCustomData = nullptr) = 0;	
};




//////////////////////////////////////////////////////////////////////////////////////////////////

//
//
//
//  TODO:  Throw these enumeration out of here!!!!!
//
//
//




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
	S_OPENGL,
	S_VULKAN
};

inline static const char* GetRendererTypeName(const S_RENDERER_TYPE& type)
{
	switch (type)
	{
	case S_DIRECTX9: return "DIRECTX9";
	case S_DIRECTX10: return "DIRECTX10";
	case S_DIRECTX11: return "DIRECTX11";
	case S_OPENGL: return "OPENGL";
	case S_VULKAN: return "VULKAN";
	default:
		return "UNKNOWN";
	}
}

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

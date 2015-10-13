#pragma once

#include <math.h>


#define IS_VALID_PTR(x) ((x))

typedef unsigned __int16 u16;
typedef unsigned __int32 u32;
typedef unsigned __int64 u64;

typedef float f32;


struct SVertex
{
	float x, y, z;
	float nx, ny, nz;
	float u, v;
};

struct SVector3
{
	float x, y, z;

	SVector3() : x(0), y(0), z(0) {}
	SVector3(const SVector3& v) : x(v.x), y(v.y), z(v.z) {}
	SVector3(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}
};

typedef struct SVector3 Vec3f;

inline float RoughnessToShininess(float m)
{
	return 2.0f / (m * m) - 2.0f;
}

inline float ShininessToRoughness(float alpha)
{
	return sqrtf(2.0f / (alpha + 2.0f));
}
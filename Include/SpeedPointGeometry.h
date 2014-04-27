// ******************************************************************************************

//	This file is part of the SpeedPoint Game Engine

//	(c) 2011 - 2014 Pascal Rosenkranz aka. iSmokiieZz
//	All rights reserved.

// ******************************************************************************************

#pragma once

#ifndef SP_GEOMETRY_COMPONENT

// BEGIN TODO: Implement Particles, then remove this definition
#define SP_NO_PARTICLES
// END TODO

// ******************************************************************************************

// Include abstract geometry component classes

#include <Util\STransformable.h>
#include <Abstract\STexture.h>
#include <Abstract\SOctreeNode.h>
#include <Abstract\SOctree.h>

#ifndef SP_NO_PARTICLES
#include <Abstract\SParticle.h>
#include <Abstract\SParticleSystem.h>
#endif

#include <Util\SVertex.h>
#include <Abstract\SSolid.h>
#include <Abstract\STerrain.h>

// ******************************************************************************************

#define SP_GEOMETRY_COMPONENT
#endif
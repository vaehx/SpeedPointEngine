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
#include <Abstract\ITexture.h>
#include <Abstract\IOctreeNode.h>
#include <Abstract\IOctree.h>

#ifndef SP_NO_PARTICLES
#include <Abstract\IParticle.h>
#include <Abstract\IParticleSystem.h>
#endif

#include <Util\SVertex.h>
#include <Abstract\ISolid.h>
#include <Abstract\ITerrain.h>

// ******************************************************************************************

#define SP_GEOMETRY_COMPONENT
#endif
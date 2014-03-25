// ******************************************************************************************

// SpeedPoint Engine Main Header

// This file will include all Subprerequisite-Headers unless they are disabled by a
// specific defined flag using #define ...

// -----------------------------------------------------------------------------------------
// SpeedPoint Engine
// (c) 2011-2014, Pascal Rosenkranz.

// Dev Version: 2.0.2
// Beta Version: 0.2.0
// Release Version: 0.0.2

// ******************************************************************************************

#pragma once
#include <SPrerequisites.h>

#ifndef SP_ALL_COMPONENTS_LOADED

// ******************************************************************************************

// common includes will allways be loaded
#include <SpeedPointCommon.h>

#ifndef SP_NO_GEOMETRY
#include <SpeedPointGeometry.h>
#endif

#ifndef SP_NO_PHYSICS
#include <SpeedPointPhysics.h>
#endif

#ifndef SP_NO_RENDERING
#include <SpeedPointRendering.h>
#endif

#ifndef SP_NO_ANIMATIONS
#include <SpeedPointAnimations.h>
#endif

#ifndef SP_NO_SCRIPTING
#include <SpeedPointScripting.h>
#endif

#ifndef SP_NO_SOUND
#include <SpeedPointSound.h>
#endif

#ifndef SP_NO_AI
#include <SpeedPointAI.h>
#endif

// ******************************************************************************************

#define SP_ALL_COMPONENTS_LOADED
#endif
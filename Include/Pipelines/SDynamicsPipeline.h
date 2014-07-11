// ******************************************************************************************

// This file is part of the SpeedPoint Engine

// ******************************************************************************************

#pragma once

#include <SPrerequisites.h>
#include <Observer\SObservedObject.h>

SP_NMSPACE_BEG

class S_API FramePipeline;

// SpeedPoint Dynamics Pipeline
class S_API SDynamicsPipeline : public SObservedObject
{
private:
	SpeedPointEngine*	m_pEngine;
	FramePipeline*		m_pFramePipeline;	// pointer to the handling frame pipeline

public:
	// Default constructor
	SDynamicsPipeline()
		: m_pEngine(0)
	{
	}

	// Default destructor
	~SDynamicsPipeline()
	{
		SObservedObject::~SObservedObject();
	}

	// Initialize the dynamics pipeline with pointer to the SpeedPoint Engine and to frame pipeline
	SResult Initialize(SpeedPointEngine* m_pEngine, FramePipeline* pFramePipeline);

	// Clear the dynamics pipeline
	SResult Clear();

	// Do the animation thing
	SResult DoAnimation();

	// Do the Physics thing
	// (Destruction, Forces/Momentums, velocities, ... everything the physics engine supports)
	SResult DoPhysics();

	// Do the Interaction thing
	SResult DoInteraction();

	// Do the scripting thing
	SResult DoScriptExecution();

	// Do the renderscript thing
	SResult DoRecalcRenderscript();
};


SP_NMSPACE_END
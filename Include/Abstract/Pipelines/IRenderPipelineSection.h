// ********************************************************************************************

//	SpeedPoint Render Pipeline State

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ********************************************************************************************

#pragma once
#include <SPrerequisites.h>

SP_NMSPACE_BEG

class S_API SpeedPointEngine;
struct S_API IRenderPipeline;


// SpeedPoint Render Pipeline State (abstract)
// This class contains function which all sections of the Render Pipeline shall provide.
// The sections are there to minimize code in the IRenderPipeline implementation classes.
// There are some implementations like SDirectX9GeometryRenderSection, ~LightingRenderSection, ~PostRenderSection
struct S_API IRenderPipelineSection
{	
public:
	// Initialize the Render Pipeline State
	virtual SResult Initialize(SpeedPointEngine* pEngine, IRenderPipeline* pRenderPipeline) = 0;

	// Clear the Render Pipeline state
	virtual SResult Clear(void) = 0;
};


SP_NMSPACE_END
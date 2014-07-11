// ******************************************************************************************

// This file is part of the SpeedPoint Engine

// ******************************************************************************************

#include <Pipelines\SDynamicsPipeline.h>
#include <SpeedPointEngine.h>
#include <Pipelines\FramePipeline.h>

namespace SpeedPoint
{
	S_API SResult SDynamicsPipeline::Initialize(SpeedPointEngine* m_pEngine, FramePipeline* pFramePipeline)
	{
		return S_SUCCESS;
	}

	// ******************************************************************************************

	S_API SResult SDynamicsPipeline::DoAnimation()
	{
		return S_NOTIMPLEMENTED;
	}

	// ******************************************************************************************

	S_API SResult SDynamicsPipeline::DoPhysics()
	{
		return S_NOTIMPLEMENTED;
	}

	// ******************************************************************************************
	
	S_API SResult SDynamicsPipeline::DoInteraction()
	{
		return S_NOTIMPLEMENTED;
	}

	// ******************************************************************************************
	
	S_API SResult SDynamicsPipeline::DoScriptExecution()
	{
		return S_NOTIMPLEMENTED;
	}	

	// ******************************************************************************************

	S_API SResult SDynamicsPipeline::Clear()
	{
		return S_SUCCESS;
	}

	// ******************************************************************************************
	
	S_API SResult SDynamicsPipeline::DoRecalcRenderscript()
	{
		return S_NOTIMPLEMENTED;
	}
}
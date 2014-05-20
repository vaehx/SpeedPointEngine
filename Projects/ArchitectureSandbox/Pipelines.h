
// Pipelines

#pragma once

#include "Engines.h"



struct IPipeline
{
};


struct IFramePipeline
{
	virtual EResult Initialize(IEngine* engine) = 0;
};
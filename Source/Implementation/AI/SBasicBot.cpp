// ********************************************************************************************

//	This file is part of the SpeedPoint Game Engine

//	(c) 2011-2014 Pascal Rosenkranz aka iSmokiieZz
//	All rights reserved.

// ********************************************************************************************

#include <Implementation\AI\SBasicBot.h>

namespace SpeedPoint
{
	S_API SResult SBasicBot::InitScriptAttributes(void)
	{
		return S_NOTIMPLEMENTED;
	}

	S_API void SBasicBot::InitScriptEnvironment(RunPowderEnvironment* pEnv)
	{		
	}

	S_API void SBasicBot::SetInteractionRadius(float fRadius)
	{

	}

	S_API float SBasicBot::GetInteractionRadius(void)
	{
		return 0.0f;
	}

	S_API void SBasicBot::SetName(const SString& src)
	{
	}

	S_API SString* SBasicBot::GetName(void)
	{
		return 0;
	}
}
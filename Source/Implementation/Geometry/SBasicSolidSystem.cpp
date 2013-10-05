// SpeedPoint Solid System - A collection of solids

#include <Implementation\Geometry\SBasicSolidSystem.h>

namespace SpeedPoint
{
	// **********************************************************************************
	
	S_API SResult SBasicSolidSystem::Initialize( SpeedPointEngine* engine )
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API SSolid* SBasicSolidSystem::GetSolid( SP_ID id )
	{
		return NULL;
	}

	// **********************************************************************************

	S_API SResult SBasicSolidSystem::Clear( void )
	{
		return S_ERROR;
	}

}
// ******************************************************************************************

//	SpeedPoint Command Buffer (abstract)

//	This is a virtual class. You cannot instantiate it.
//	Please use Specific Implementation to instantiate.

// ******************************************************************************************

#pragma once

#include <SPrerequisites.h>

namespace SpeedPoint
{
	// SpeedPoint Command Buffer Task (abstract)
	class S_API SCommandBufferEntry
	{	
	public:
		// Initialize
		/////virtual SResult Initialize( SpeedPointEngine* pEngine, S_COMMAND_TYPE type ) = 0;

		// Get the command type
		/////virtual S_COMMAND_TYPE GetCommandType( void ) = 0;
	};

	// SpeedPoint Command Buffer (abstract)
	class S_API SCommandBuffer
	{
	public:
		// Get the latest task that has been added
		////// TODO
	};
}
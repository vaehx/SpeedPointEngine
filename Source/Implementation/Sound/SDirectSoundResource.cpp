// SpeedPoint DirectSound Sound Resource

#include <Implementation\Sound\SDirectSoundResource.h>

namespace SpeedPoint
{
	
	// **********************************************************************************

	S_API SResult SDirectSoundResource::Initialize( ISoundSystem* pSystem ) 
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API SResult SDirectSoundResource::LoadFromFile( char* cFileName, char* specification )
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API SString SDirectSoundResource::GetSpecification( void )
	{
		return SString();
	}

	// **********************************************************************************

	S_API SResult SDirectSoundResource::Clear( void )
	{
		return S_ERROR;
	}
}
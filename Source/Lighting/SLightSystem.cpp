// SpeedPoint Light System

#include <Lighting\SLightSystem.h>

namespace SpeedPoint
{
		// **********************************************************************************

		S_API SResult SLightSystem::AddLight( SLight* pLight )
		{
			return S_ERROR;
		}
		
		// **********************************************************************************

		S_API SLight* SLightSystem::GetLight( SP_ID iUID )
		{
			return NULL;
		}

		// **********************************************************************************

		S_API SLight* SLightSystem::GetLightBySpecification( SString cSpec )
		{
			return NULL;
		}

		// **********************************************************************************

		S_API SString SLightSystem::GetLightSpecification( SP_ID iUID )
		{
			return SString();
		}
		
		// **********************************************************************************

		S_API SP_ID SLightSystem::GetLightUID( SString spec )
		{
			return SP_ID();
		}

		// **********************************************************************************

		S_API SResult SLightSystem::RemoveLight( SP_ID iUID )
		{
			return S_ERROR;
		}

		// **********************************************************************************

		S_API SResult SLightSystem::ForEachLight( void (*iterationFunc)(SLight*, const SP_ID&) )
		{
			return S_ERROR;
		}

		// **********************************************************************************

		S_API SResult SLightSystem::GetLightsInRange( float fRadius, SP_ID** pLightIds )
		{
			return S_ERROR;
		}

		// **********************************************************************************

		S_API SResult SLightSystem::Clear( void )
		{
			return S_ERROR;
		}
}
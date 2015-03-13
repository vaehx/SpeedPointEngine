// SpeedPoint Light System

#include <Lighting\SLightSystem.h>

namespace SpeedPoint
{
	// **********************************************************************************

	S_API SLightSystem::~SLightSystem()
	{
		Clear();
	}

	// **********************************************************************************

	S_API SResult SLightSystem::AddLight(SLight* pLight)
	{
		return S_ERROR;	
	}

	// **********************************************************************************

	S_API unsigned int SLightSystem::GetLightCount()
	{
		return m_plLights.GetUsageSize();
	}
		
	// **********************************************************************************

	S_API SLight* SLightSystem::GetLight(SP_ID iUID)
	{
		return NULL;
	}

	// **********************************************************************************

	S_API SLight* SLightSystem::GetLightBySpecification(SString cSpec)
	{
		return NULL;
	}

	// **********************************************************************************

	S_API SString SLightSystem::GetLightSpecification(SP_ID iUID)
	{
		return SString();
	}
	
	// **********************************************************************************

	S_API SP_ID SLightSystem::GetLightUID(SString spec)
	{
		return SP_ID();
	}

	// **********************************************************************************

	S_API SResult SLightSystem::RemoveLight(SP_ID iUID)
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API SResult SLightSystem::ForEachLight(void(*iterationFunc)(SLight*, const SP_ID&))
	{
		return S_ERROR;
	}

	// **********************************************************************************

	S_API SResult SLightSystem::GetLightsInRange(SVector3 vPosition, f32 fRadius, SP_ID** pLightIds, usint32* pnIDs)
	{
		if (pLightIds == 0 || *pLightIds != 0) return S_INVALIDPARAM;

		*pLightIds = new SP_ID[m_plLights.GetUsageSize()];
		
		usint32 nFoundLights = 0;
		for (usint32 i = 0; i < m_plLights.GetSize(); ++i)
		{
			SPool<SLight>::SPoolObject* pLightObject =  m_plLights.GetPoolObjectByIndirectionIndex(i);
			if (pLightObject != 0 && pLightObject->iIndirectionIndex != __TRIVIAL)
			{									
				float fMinDist = (fRadius + pLightObject->tInstance.fRadius);
				if (SVector3(vPosition - pLightObject->tInstance.vPosition).LengthSq() >(fMinDist * fMinDist))
					continue;

				(*pLightIds)[nFoundLights].iIndex = pLightObject->iIndirectionIndex;
				(*pLightIds)[nFoundLights].iVersion = pLightObject->iVersion;
				nFoundLights++;

				if (nFoundLights >= m_plLights.GetUsageSize())
					break;
			}
		}

		if (pnIDs) *pnIDs = nFoundLights;

		return S_SUCCESS;
	}

	// **********************************************************************************

	S_API SResult SLightSystem::Clear(void)
	{
		m_plLights.Clear();		
		return S_SUCCESS;
	}
}
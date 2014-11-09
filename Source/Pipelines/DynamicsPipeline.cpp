// ******************************************************************************************

// This file is part of the SpeedPoint Engine

// ******************************************************************************************

#include <Pipelines\DynamicsPipeline.h>
#include <SpeedPointEngine.h>
#include <Pipelines\FramePipeline.h>
#include <Abstract\IApplication.h>

SP_NMSPACE_BEG


S_API SResult DynamicsPipeline::Run()
{
	m_pEngine->GetApplication()->Update(0.0f);
	
	return S_SUCCESS;


	/*
	// Physics
	for (uint32 iPhysicalBody; iPhysicalBody < m_pPhysics->GetNumRegisteredBodies(); ++iPhysicalBody)
	{
		IPhysicalBody* pBody = m_pPhysics->GetBodyByIndex(iPhysicalBody);
		if (!pBody)
			continue;

		pBody->OnPreUpdate();

		SIntersectionInformation* pIntersectionInformations = 0;
		uint32 nIntersections = m_pPhysics->TestIntersection(pBody, &pIntersectionInformations);
		if (nIntersections == 0)
			continue;

		pBody->OnIntersections(pIntersectionInformations, nIntersections);

		SP_SAFE_DELETE_ARR(pIntersectionInformations, nIntersections);

		pBody->OnPostUpdate();
	}

	// AI
	for (uint32 iBot; iBot < m_pAISystem->GetNumRegisteredBots(); ++iBot)
	{
		IBot* pBot = m_pAISystem->GetBotByIndex(iBot);
		if (!pBot)
			continue;

		pBot->OnUpdate();
	}

	// Script
	for (uint32 iScript; iScript < m_pScriptSystem->GetNumActiveScripts(); ++iScript)
	{
		IScript* pScript = m_pScriptSystem->GetScriptByIndex(iScript);
		if (!pScript)
			continue;

		pScript->OnUpdate();
	}
	*/
}


SP_NMSPACE_END
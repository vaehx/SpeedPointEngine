//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Engine FrontEnd Conceptual pseudocode
//
//	written by iSmokiieZz
//	(c) 2011-2014, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Stuff that is equal in all methods:

namespace SpeedPoint
{
	enum S_API S_GEOMETRY_INPUT_STRATEGIES
	{
		eGEOMINPUTSTRATEGY_COMMANDS,	// Method 1: excellent for static render calls, bad for flexibility
		eGEOMINPUTSTRATEGY_EVENTS,	// Method 2: excellent for dynamic render calls and flexibility
		eGEOMINPUTSTRATEGY_HYBRID	// Method 3: excellent for dynamic and static render calls, high flexibility
	};
}

class MyGameClass
{
public:
	SP_ID			m_iSolid;
	SSpeedPointEngine	m_Engine;

	// Initialization:
	bool InitEngine();
	bool InitLevel();	// in actual games would be LoadLevel(char* level) or something like this

	// Tick:
	bool Tick();

	// Shutdown:
	bool Shutdown();	
};

// could derive a game interface from that:
namespace SpeedPoint
{
	class S_API IApplication
	{
		// Initialization
	public:
		virtual SResult InitEngine() = 0;
		virtual SResult InitLevel() = 0;

		// Tick
	public:
		virtual SResult Update() = 0;
		virtual SResult Render() = 0;

		// Shutdown
	public:
		virtual SResult Shutdown() = 0;

		// Getter / setter
	public:
		virtual IEngine* GetEngine() = 0;		
	};
}

int main(void)
{
	MyGameClass game;

	// Initialize the engine
	if (!game.InitEngine())
		return false;

	// Initialize the level geoemtry, static render command queue, etc...
	if (!game.InitLevel())
		return false;

	// Mainloop:
	while (true)
	{
		if (!game.Tick())
			break;
	}

	game.Shutdown();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Method 1: (using eGEOMINPUTSTRATEGY_COMMANDS)

bool MyGameClass::InitEngine()
{
	SEngineSettings settings;
	settings.SetResolution(1024, 768);	// same as: settings.app.nXResolution = 1024; settings.app.nYResolution = 768;
	
	if (Failure(m_Engine.Initialize(settings)))
		return false;

	return true;
}

bool MyGameClass::InitLevel()
{
	// SetupLevelGeometry
	SSolid* pSolid = m_Engine.AddSolid(&m_iSolid);
	pSolid->Initialize(&m_Engine);
	
	SVertex* pVertices = new SVertex[] {
		...
	};

	SVtxIndex* pIndices = new SVtxIndex[] {
		...
	};

	if (Failure(pSolid->FillGeometry(pVertices, nVertices, pIndices, nIndices))) // pay attention to set nVertices and pIndices anywhere
	{
		return false;
	}

	// SetupLevelRenderCommandQueue
	if (Success(m_Engine.RecordRenderCommands())
	{
		pSolid->RenderGeometry();	// WILL FAIL, BECAUSE NOT IN CORRECT SECTION, otherwise requires SSolid to keep its own id

		m_Engine.RenderSolidGeometry(m_iSolid); // CORRECT
		m_Engine.StopRenderCommands();
	}
	else
	{
		return false;
	}

	return true;
}

bool MyGameClass::Tick()
{
	if (Failure(m_Engine.BeginFrame(eGEOMINPUTSTRATEGY_COMMANDS)))
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Method 2: (using eGEOMINPUTSTRATEGY_EVENTS) (= default)

/*
namespace SpeedPoint
{
	enum S_API S_EVENT_PARAMETER_INDEX
	{
		eEPARAM_SENDER,
		eEPARAM_CUSTOM	
	};	
}
*/

struct MyCustomEventData
{
	MyGameClass*	pGame;
};

SResult OnDrawCalls(SEventParameters* pParams)
{
	// do ALL render calls you want to do here

	// TODO: maybe try to use unsigned int-indices for event parameters instead of strings.. thats cruel :/		
	MyGameClass* pGame = ((MyCustomEventData*)pParams->Get(eEPARAM_CUSTOM))->pGame;

	return pGame->m_Engine.RenderSolid(pGame->m_iSolid);
}

bool MyGameClass::InitEngine()
{
	// Setup the engine itself
	SEngineSettings settings;
	settings.SetResolution(1024, 768);

	if (Failure(m_Engine.Initialize(settings)))
		return false;

	// Setup the user event data passed as parameter for all events
	MyCustomEventData customEventData;
	customEventData.pGame = &game;	// set game anywhere
	m_Engine.SetCustomCustomGlobalEventData((void*)&customEventData);

	return true;
}

bool MyGameClass::InitLevel()
{
	// SetupLevelGeometry
	SSolid* pSolid = m_Engine.AddSolid(&m_iSolid);
	pSolid->Initialize(&m_Engine);
	SVertex* pVertices = new SVertex[] {
		...
	};
	SVtxIndex* pIndices = new SVtxIndex[] {
		...
	};
	if (Failure(pSolid->FillGeometry(pVertices, nVertices, pIndices, nIndices))) // pay attention to set nVertices and pIndices anywhere
	{
		return 0;
	}	
}

bool MyGameClass::Tick()
{
	if (Failure(m_Engine.BeginFrame(eGEOMINPUTSTRATEGY_EVENTS)))
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Method 3 (eGEOMINPUTSTRATEGY_HYBRID)

struct MyCustomEventData
{
	MyGameClass*	pGame;
};

// fired if an eSRCMD_DRAWSOLID command was reached here
// Other events:
//	OnRenderTerrain() - fired when eSRCMD_DRAWTERRAIN reached
//	OnRenderPrimitive() - fired when eSRCMD_DRAWPRIMITIVE reached or for all primitives of a solid in eSRCMD_DRAWSOLID
SResult OnRenderSolid(SEventParameters* pParams)
{	
	MyGameClass* pGame = ((MyCustomEventData*)pParams->Get(eEPARAM_CUSTOM))->pGame;

	if (...)// check whether the render solid should be called here or not
	{
		pGame->m_Engine.RenderSolid(pGame->m_iSolid);

		// if you want to render solids multiple times or solids that have not been called previously, you
		// can do it now
		pGame->m_Engine.RenderSolid(pGame->m_iOtherSolid);
		pGame->m_Engine.RenderSolid(pGame->)
	}

	return S_SUCCESS;
}

bool MyGameClass::InitEngine()
{
	SEngineSettings settings;
	settings.SetResolution(1024, 768);	// same as: settings.app.nXResolution = 1024; settings.app.nYResolution = 768;

	if (Failure(m_Engine.Initialize(settings)))
		return false;

	// Setup the user event data passed as parameter for all events
	MyCustomEventData customEventData;
	customEventData.pGame = &game;	// set game anywhere
	m_Engine.SetCustomCustomGlobalEventData((void*)&customEventData);

	return true;
}

bool MyGameClass::InitLevel()
{
	// SetupLevelGeometry
	SSolid* pSolid = m_Engine.AddSolid(&m_iSolid);
	pSolid->Initialize(&m_Engine);

	SVertex* pVertices = new SVertex[] {
		...
	};

	SVtxIndex* pIndices = new SVtxIndex[] {
		...
	};

	if (Failure(pSolid->FillGeometry(pVertices, nVertices, pIndices, nIndices))) // pay attention to set nVertices and pIndices anywhere
	{
		return false;
	}

	// SetupLevelRenderCommandQueue
	if (Success(m_Engine.RecordRenderCommands())
	{
		pSolid->RenderGeometry();	// WILL FAIL, BECAUSE NOT IN CORRECT SECTION, otherwise requires SSolid to keep its own id

		m_Engine.RenderSolidGeometry(m_iSolid); // CORRECT
		m_Engine.StopRenderCommands();
	}
	else
	{
		return false;
	}

	return true;
}

bool MyGameClass::Tick()
{
	if (Failure(m_Engine.BeginFrame(eGEOMINPUTSTRATEGY_HYBRID)))
		return false;

	return true;
}
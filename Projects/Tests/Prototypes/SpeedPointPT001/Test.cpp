/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Prototype Test Project Source File
//
//	Remember to be frugal with log calls, as SpeedPoint already throws a huge amount of them!
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "Test.h"
#include "TestUtil.h"
#include "Geometry.h"

#include <Implementation\DirectX11\DirectX11Renderer.h>



void TestLogHandler::OnLog(SpeedPoint::SResult res, const SpeedPoint::SString& formattedMsg)
{
	std::cout << formattedMsg << std::endl;
}

void Test::OnLogReport(SpeedPoint::SResult res, const SpeedPoint::SString& msg)
{
}

void CalcLookAt(SpeedPoint::SCamera& cam, const SpeedPoint::SVector3& lookAt)
{
	SpeedPoint::SVector3 dir = SpeedPoint::SVector3Normalize(lookAt - cam.position);
	cam.rotation.x = asinf(dir.y);	
	cam.rotation.y = acosf(dir.z / sqrtf(1.0f - (dir.y * dir.y)));
	if (dir.x < 0.0f)
		cam.rotation.y = (2.0f * SP_PI) - cam.rotation.y;

	cam.rotation.z = 0.0f;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

bool Test::KeyPressed(EKey key) const
{
	switch (key)
	{
	case KEY_MOVE_FORWARD: return ::GetAsyncKeyState(0x57) < 0;	//W
	case KEY_MOVE_BACKWARD: return ::GetAsyncKeyState(0x53) < 0;	//S
	case KEY_MOVE_LEFT: return ::GetAsyncKeyState(0x41) < 0;	//A
	case KEY_MOVE_RIGHT: return ::GetAsyncKeyState(0x44) < 0;	//D
	case KEY_MOVE_UP: return ::GetAsyncKeyState(0x45) < 0;		//E
	case KEY_MOVE_DOWN: return ::GetAsyncKeyState(0x51) < 0;	//Q
	case KEY_SHIFT: return ::GetAsyncKeyState(VK_SHIFT) < 0;
	default:
		return false;
	}	
}


/////////////////////////////////////////////////////////////////////////////////////////////////

bool Test::Start(HWND hWnd, HINSTANCE hInstance)
{
	m_pEngine = new SpeedPoint::SpeedPointEngine();
	m_pEngine->RegisterApplication(this);
	
	SpeedPoint::SSettingsDesc& dsc = m_pEngine->GetSettings()->Get();	
	dsc.app.nXResolution = 1024;
	dsc.app.nYResolution = 768;
	dsc.app.hWnd = hWnd;	
	dsc.render.fClipNear = 0.1f;
	dsc.render.bEnableVSync = true;
	dsc.render.vsyncInterval = 1;	
	dsc.render.bRenderWireframe = false;	
	dsc.render.fTerrainDMFadeRange = 5.0f;

	SpeedPoint::SResult initResult = SpeedPoint::S_SUCCESS;	
	EXEC_CONDITIONAL(initResult, m_pEngine->InitializeLogger(&logHandler));
	EXEC_CONDITIONAL(initResult, m_pEngine->GetLog()->SetLogLevel(SpeedPoint::ELOGLEVEL_DEBUG));	
	EXEC_CONDITIONAL(initResult, m_pEngine->InitializeFramePipeline());
	EXEC_CONDITIONAL(initResult, m_pEngine->InitializeRenderer(SpeedPoint::S_DIRECTX11, SpeedPoint::DirectX11Renderer::GetInstance(), true));
	EXEC_CONDITIONAL(initResult, m_pEngine->InitializeResourcePool());
	EXEC_CONDITIONAL(initResult, m_pEngine->InitializeScene(new SpeedPoint::Scene()));

	if (Failure(initResult))
	{
		delete m_pEngine;
		return false;
	}

	m_pEngine->GetLog()->SetLogFile("App.log");


	m_pScene = m_pEngine->GetLoadedScene();
	
	// pay attention that FinishINitialization() will call OnInitGeometry()!
	m_pEngine->FinishInitialization();

	SpeedPoint::IRenderer* pRenderer = m_pEngine->GetRenderer();

	// Initialize viewport

	SpeedPoint::SProjectionDesc projDsc;
	projDsc.bUseEngineZPlanes = false;
	projDsc.farZ = 200.0f;
	projDsc.nearZ = 0.1f;
	projDsc.fov = 40;
	projDsc.projectionType = SpeedPoint::S_PROJECTION_PERSPECTIVE;	
	pRenderer->GetTargetViewport()->SetProjectionByDesc(projDsc);

	pCamera = pRenderer->GetTargetViewport()->GetCamera();
	pCamera->position = SpeedPoint::SVector3(0, 5.0f, -60.0f);
	//pCamera->LookAt(SpeedPoint::SVector3(0, 0, 0));
	alpha = 0.0f;	

	nDumpedFrames = 0;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void Test::OnInitGeometry()
{	
	// Load resources

	SpeedPoint::ITexture* pTestTextureMap;
	m_pEngine->GetResources()->LoadTexture("..\\..\\res\\brick.bmp", 768, 768, "tex01", &pTestTextureMap);

	SpeedPoint::ITexture* pTestNormalMap;
	m_pEngine->GetResources()->LoadTexture("..\\..\\res\\brick_n.bmp", 768, 768, "tex01n", &pTestNormalMap);

	SpeedPoint::ITexture* pTestDetailMap;
	m_pEngine->GetResources()->LoadTexture("..\\..\\res\\grass_dm.bmp", 640, 640, "terrain_dm", &pTestDetailMap);

	SpeedPoint::ITexture* pTestColorMap;
	m_pEngine->GetResources()->LoadTexture("..\\..\\res\\grass_cm.bmp", 64, 64, "terrain_cm", &pTestColorMap);


	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Load first 3ds model

	SpeedPoint::Scene myScene;
	myScene.Initialize(m_pEngine);
	pTest3DSObject = myScene.LoadStaticObjectFromFile("..\\..\\res\\truck.3ds");
	
	myScene.CreateNormalsGeometry(pTest3DSObject, &pTest3DSNormalsObject);	

	// create test references
	for (unsigned int i = 0; i < TEST_REFS; ++i)
	{	
		pTestRefs[i] = pTest3DSObject->CreateReferenceObject();
		pTestRefs[i]->vPosition.x += ((float)i) * 3.0f;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Add first geometry

	GeometryLib::GenerateBox(m_pEngine, testObject.GetGeometry(), testObject.GetSingleMaterial(), 1.0f, 1.0f, 1.0f);
	testObject.vPosition = SpeedPoint::SVector3(0.0, 0.0, 0.0f);

	// Set object material
	SpeedPoint::SMaterial* testMat = m_pEngine->GetResources()->AddNewMaterial("testMat");
	testMat->textureMap = pTestTextureMap;
	testMat->normalMap = pTestNormalMap;
	
	testObject.SetSingleMaterial(testMat);


	///////////////////////////////////////////////////////////////////////1/////////////////////////////////
	// Create terrain

	m_pScene->CreateTerrain(40.0f, 40.0f, 60, 60, 0.0f, pTestColorMap, pTestDetailMap);
	m_pEngine->GetSettings()->SetTerrainDetailMapFadeRadius(10.0f);

	///////////////////////////////////////////////////////////////////////1/////////////////////////////////	

	///////////////////////////////////////////////////////////////////////1/////////////////////////////////
	// Debug stuff

	SpeedPoint::SCamera testCam;
	testCam.position = SpeedPoint::SVector3(-30.0f, 14.0f, -200.0f);
	testCam.rotation = SpeedPoint::SVector3(0.0f, 0.0f, 0.0f);	

}

/////////////////////////////////////////////////////////////////////////////////////////////////

bool Test::Tick()
{	
	alpha += 0.01f;
	if (alpha > 2.0f * SP_PI)
		alpha = 0.0f;	

	if (nDumpedFrames < 0)
	{
		m_pEngine->GetRenderer()->DumpFrameOnce();
		nDumpedFrames++;
	}

	// Start the frame pipeline
	if (Failure(m_pEngine->ExecuteFramePipeline()))
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void Test::Render()
{

	// UPDATE:

	testObject.vRotation += SpeedPoint::SVector3(0.0f, 0.01f, 0.0f);	
	testObject.vRotation.x = SP_PI * 0.5f;
	testObject.vPosition = SpeedPoint::SVector3(0, 0.0f, 0.0f);

	pTest3DSObject->vPosition = testObject.vPosition;
	pTest3DSObject->vRotation = testObject.vRotation;
	pTest3DSObject->vSize = SpeedPoint::SVector3(0.3f, 0.3f, 0.3f);
	
	pTest3DSNormalsObject->vPosition = pTest3DSObject->vPosition;
	pTest3DSNormalsObject->vRotation = pTest3DSObject->vRotation;
	pTest3DSNormalsObject->vSize = pTest3DSObject->vSize;

	/*
	float camTurnRad = 3.0f;
	SpeedPoint::SVector3 camLookAt;
	camLookAt.x = sinf(alpha) * camTurnRad;
	camLookAt.y = 3.0f;
	camLookAt.z = cosf(alpha) * camTurnRad;
	*/	

	float moveDiff = (KeyPressed(KEY_SHIFT) ? 0.3f : 0.1f);

	if (KeyPressed(KEY_MOVE_FORWARD)) pCamera->position.z += moveDiff;
	if (KeyPressed(KEY_MOVE_BACKWARD)) pCamera->position.z -= moveDiff;
	if (KeyPressed(KEY_MOVE_LEFT)) pCamera->position.x += moveDiff;
	if (KeyPressed(KEY_MOVE_RIGHT)) pCamera->position.x -= moveDiff;
	if (KeyPressed(KEY_MOVE_UP)) pCamera->position.y += moveDiff;
	if (KeyPressed(KEY_MOVE_DOWN)) pCamera->position.y -= moveDiff;

	//pCamera->LookAt(SpeedPoint::SVector3(0, 0, 0));	

	pCamera->RecalculateViewMatrix();
	






	// RENDER

	//if (Failure(m_pScene->GetTerrain()->RenderTerrain()))
	//	m_pEngine->LogE("Failed render terrain!");


	//testObject.Render();
	pTest3DSObject->Render();
	if (Failure(pTest3DSNormalsObject->Render()))
		m_pEngine->LogE("Failed render 3ds model!");

	for (unsigned int i = 0; i < TEST_REFS; ++i)
	{		
		float x = (float)(i % 30);
		float z = (float)((float)i - x) / 30.0f;

		pTestRefs[i]->vRotation = pTest3DSObject->vRotation;
		pTestRefs[i]->vPosition = pTest3DSObject->vPosition + SpeedPoint::SVector3(x * 15.0f, 0, z * 15.0f);
		pTestRefs[i]->vSize = pTest3DSObject->vSize;
		pTestRefs[i]->Render();
	}

}

/////////////////////////////////////////////////////////////////////////////////////////////////

bool Test::Stop()
{
	for (unsigned int i = 0; i < 10; ++i)
		delete pTestRefs[i];

	m_pScene->Clear();
	testObject.Clear();
	
	m_pEngine->Shutdown();	
	delete m_pEngine;

	return true;
}

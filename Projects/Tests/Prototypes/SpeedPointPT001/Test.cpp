/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Prototype Test Project Source File
//
//	Remember to be frugal with log calls, as SpeedPoint already throws a huge amount of them!
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "Test.h"
#include "TestUtil.h"

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

bool Test::Start(HWND hWnd, HINSTANCE hInstance)
{
	m_pEngine = new SpeedPoint::SpeedPointEngine();
	m_pEngine->RegisterApplication(this);
	
	SpeedPoint::SSettingsDesc dsc;
	dsc.app.nXResolution = 1024;
	dsc.app.nYResolution = 768;
	dsc.app.hWnd = hWnd;	
	dsc.render.fClipNear = 0.1f;
	dsc.render.bEnableVSync = true;
	dsc.render.bRenderWireframe = false;
	dsc.mask = ENGSETTING_RESOLUTION | ENGSETTING_HWND | ENGSETTING_ENABLEVSYNC | ENGSETTING_CLIPPLANES | ENGSETTING_WIREFRAME;

	SpeedPoint::SResult initResult = SpeedPoint::S_SUCCESS;
	m_pEngine->GetSettings()->Set(dsc);
	EXEC_CONDITIONAL(initResult, m_pEngine->InitializeLogger(&logHandler));
	EXEC_CONDITIONAL(initResult, m_pEngine->GetLog()->SetLogLevel(SpeedPoint::ELOGLEVEL_DEBUG));	
	EXEC_CONDITIONAL(initResult, m_pEngine->InitializeFramePipeline());
	EXEC_CONDITIONAL(initResult, m_pEngine->InitializeRenderer(SpeedPoint::S_DIRECTX11, SpeedPoint::DirectX11Renderer::GetInstance(), true));
	EXEC_CONDITIONAL(initResult, m_pEngine->InitializeResourcePool());

	if (Failure(initResult))
	{
		delete m_pEngine;
		return false;
	}

	m_pEngine->GetLog()->SetLogFile("App.log");

	m_pEngine->FinishInitialization();

	// Initialize viewport

	m_pEngine->GetRenderer()->GetTargetViewport()->Set3DProjection(SpeedPoint::S_PROJECTION_PERSPECTIVE, 50, 0, 0);
	pCamera = m_pEngine->GetRenderer()->GetTargetViewport()->GetCamera();
	pCamera->position = SpeedPoint::SVector3(0, 5.0f, -10.0f);
	pCamera->LookAt(SpeedPoint::SVector3(0, 0, 0));
	alpha = 0.0f;

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


	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Add first geometry	

	SpeedPoint::SInitialGeometryDesc initialGeom;
	initialGeom.pVertices = new SpeedPoint::SVertex[24];
	initialGeom.pIndices = new SpeedPoint::SIndex[36];

	//						  Position		   Normal		Tangent		    UV
	// Top:
	initialGeom.pVertices[ 0] = SpeedPoint::SVertex( 0.5f, 0.5f,-0.5f,	 0.0f, 1.0f, 0.0f,	0.0f, 0.0f, 0.0f,	0.0f, 0.0f);
	initialGeom.pVertices[ 1] = SpeedPoint::SVertex(-0.5f, 0.5f,-0.5f,	 0.0f, 1.0f, 0.0f,	0.0f, 0.0f, 0.0f,	1.0f, 0.0f);
	initialGeom.pVertices[ 2] = SpeedPoint::SVertex(-0.5f, 0.5f, 0.5f,	 0.0f, 1.0f, 0.0f,	0.0f, 0.0f, 0.0f,	1.0f, 1.0f);
	initialGeom.pVertices[ 3] = SpeedPoint::SVertex( 0.5f, 0.5f, 0.5f,	 0.0f, 1.0f, 0.0f,	0.0f, 0.0f, 0.0f,	0.0f, 1.0f);
			       
	// Back:
	initialGeom.pVertices[ 4] = SpeedPoint::SVertex( 0.5f, 0.5f, 0.5f,	 0.0f, 0.0f, 1.0f,	0,0,0,			0.0f, 0.0f);
	initialGeom.pVertices[ 5] = SpeedPoint::SVertex(-0.5f, 0.5f, 0.5f,	 0.0f, 0.0f, 1.0f,	0,0,0,			1.0f, 0.0f);
	initialGeom.pVertices[ 6] = SpeedPoint::SVertex(-0.5f,-0.5f, 0.5f,	 0.0f, 0.0f, 1.0f,	0,0,0,			1.0f, 1.0f);
	initialGeom.pVertices[ 7] = SpeedPoint::SVertex( 0.5f,-0.5f, 0.5f,	 0.0f, 0.0f, 1.0f,	0,0,0,			0.0f, 1.0f);
		
	// Bottom:
	initialGeom.pVertices[ 8] = SpeedPoint::SVertex( 0.5f,-0.5f, 0.5f,	 0.0f,-1.0f, 0.0f,	0,0,0,			0.0f, 0.0f);
	initialGeom.pVertices[ 9] = SpeedPoint::SVertex(-0.5f,-0.5f, 0.5f,	 0.0f,-1.0f, 0.0f,	0,0,0,			1.0f, 0.0f);
	initialGeom.pVertices[10] = SpeedPoint::SVertex(-0.5f,-0.5f,-0.5f,	 0.0f,-1.0f, 0.0f,	0,0,0,			1.0f, 1.0f);
	initialGeom.pVertices[11] = SpeedPoint::SVertex( 0.5f,-0.5f,-0.5f,	 0.0f,-1.0f, 0.0f,	0,0,0,			0.0f, 1.0f);

	// Front:
	initialGeom.pVertices[12] = SpeedPoint::SVertex( 0.5f,-0.5f,-0.5f,	 0.0f, 0.0f,-1.0f,	0,0,0,			0.0f, 0.0f);
	initialGeom.pVertices[13] = SpeedPoint::SVertex(-0.5f,-0.5f,-0.5f,	 0.0f, 0.0f,-1.0f,	0,0,0,			1.0f, 0.0f);
	initialGeom.pVertices[14] = SpeedPoint::SVertex(-0.5f, 0.5f,-0.5f,	 0.0f, 0.0f,-1.0f,	0, 0, 0,		1.0f, 1.0f);
	initialGeom.pVertices[15] = SpeedPoint::SVertex( 0.5f, 0.5f,-0.5f,	 0.0f, 0.0f,-1.0f,	0, 0, 0,		0.0f, 1.0f);

	// Left:
	initialGeom.pVertices[16] = SpeedPoint::SVertex( 0.5f,-0.5f, 0.5f,	 1.0f, 0.0f, 0.0f,	0, 0, 0,		0.0f, 0.0f);
	initialGeom.pVertices[17] = SpeedPoint::SVertex( 0.5f,-0.5f,-0.5f,	 1.0f, 0.0f, 0.0f,	0, 0, 0,		1.0f, 0.0f);
	initialGeom.pVertices[18] = SpeedPoint::SVertex( 0.5f, 0.5f,-0.5f,	 1.0f, 0.0f, 0.0f,	0, 0, 0,		1.0f, 1.0f);
	initialGeom.pVertices[19] = SpeedPoint::SVertex( 0.5f, 0.5f, 0.5f,	 1.0f, 0.0f, 0.0f,	0, 0, 0,		0.0f, 1.0f);

	// Right:
	initialGeom.pVertices[20] = SpeedPoint::SVertex(-0.5f,-0.5f,-0.5f,	-1.0f, 0.0f, 0.0f,	0, 0, 0,		0.0f, 0.0f);
	initialGeom.pVertices[21] = SpeedPoint::SVertex(-0.5f,-0.5f, 0.5f,	-1.0f, 0.0f, 0.0f,	0, 0, 0,		1.0f, 0.0f);
	initialGeom.pVertices[22] = SpeedPoint::SVertex(-0.5f, 0.5f, 0.5f,	-1.0f, 0.0f, 0.0f,	0, 0, 0,		1.0f, 1.0f);
	initialGeom.pVertices[23] = SpeedPoint::SVertex(-0.5f, 0.5f,-0.5f,	-1.0f, 0.0f, 0.0f,	0, 0, 0,		0.0f, 1.0f);	


	initialGeom.nVertices = 24;

	// Top:
	initialGeom.pIndices[0] = 0; initialGeom.pIndices[1] = 1; initialGeom.pIndices[2] = 2;
	initialGeom.pIndices[3] = 2; initialGeom.pIndices[4] = 3; initialGeom.pIndices[5] = 0;

	// Back:
	initialGeom.pIndices[6] = 4; initialGeom.pIndices[7] = 5; initialGeom.pIndices[8] = 6;
	initialGeom.pIndices[9] = 6; initialGeom.pIndices[10] = 7; initialGeom.pIndices[11] = 4;

	// Bottom:
	initialGeom.pIndices[12] = 8; initialGeom.pIndices[13] = 9; initialGeom.pIndices[14] = 10;
	initialGeom.pIndices[15] = 10; initialGeom.pIndices[16] = 11; initialGeom.pIndices[17] = 8;

	// Front:
	initialGeom.pIndices[18] = 12; initialGeom.pIndices[19] = 13; initialGeom.pIndices[20] = 14;
	initialGeom.pIndices[21] = 14; initialGeom.pIndices[22] = 15; initialGeom.pIndices[23] = 12;

	// Left:
	initialGeom.pIndices[24] = 16; initialGeom.pIndices[25] = 17; initialGeom.pIndices[26] = 18;
	initialGeom.pIndices[27] = 18; initialGeom.pIndices[28] = 19; initialGeom.pIndices[29] = 16;

	initialGeom.pIndices[30] = 20; initialGeom.pIndices[31] = 21; initialGeom.pIndices[32] = 22;
	initialGeom.pIndices[33] = 22; initialGeom.pIndices[34] = 23; initialGeom.pIndices[35] = 20;
	initialGeom.nIndices = 36;

	m_pEngine->LogD("Generating tangents of initialGeom...");

	// calculate tangents	
	for (unsigned int i = 0; i < initialGeom.nIndices; i += 3)
	{
		unsigned int i1 = initialGeom.pIndices[i], i2 = initialGeom.pIndices[i + 1], i3 = initialGeom.pIndices[i + 2];
		SpeedPoint::SVertex &v1 = initialGeom.pVertices[i1], &v2 = initialGeom.pVertices[i2], &v3 = initialGeom.pVertices[i3];
		v1.CalcTangent(v2, v3);
		v2.tx = v1.tx; v2.ty = v1.ty; v2.tz = v1.tz;
		v3.tx = v1.tx; v3.ty = v1.ty; v3.tz = v1.tz;
	}


	testObject.vPosition = SpeedPoint::SVector3(0.0, 0.0, 0.0f);
	testObject.Init(m_pEngine, m_pEngine->GetRenderer(), &initialGeom);

	SpeedPoint::Material& testObjMat = testObject.GetMaterial();
	testObjMat.textureMap = pTestTextureMap;
	testObjMat.normalMap = pTestNormalMap;


	///////////////////////////////////////////////////////////////////////1/////////////////////////////////
	// Create terrain
	
	testTerrain.Initialize(m_pEngine, 20, 20);
	testTerrain.CreatePlanar(10.0f, 10.0f, 0.0f);


	///////////////////////////////////////////////////////////////////////1/////////////////////////////////

	m_pEngine->GetRenderer()->DumpFrameOnce();	

	///////////////////////////////////////////////////////////////////////1/////////////////////////////////
	// Debug stuff

	SpeedPoint::SCamera testCam;
	testCam.position = SpeedPoint::SVector3(-5.0f, 10.0f, -5.0f);
	testCam.rotation = SpeedPoint::SVector3(-0.7f, 0.0f, 0.0f);
	//testCam.LookAt(SpeedPoint::SVector3(0, 0, 0));

}

/////////////////////////////////////////////////////////////////////////////////////////////////

bool Test::Tick()
{	
	alpha += 0.01f;
	if (alpha > 2.0f * SP_PI)
		alpha = 0.0f;	

	// Start the frame pipeline
	if (Failure(m_pEngine->ExecuteFramePipeline()))
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void Test::Render()
{	


	testObject.vRotation += SpeedPoint::SVector3(0.01f, 0.00f, 0.00f);		
	testObject.vPosition = SpeedPoint::SVector3(0, 4.0f, -8.0f);	
	

	float camTurnRad = 5.0f;
	pCamera->position.x = sinf(alpha) * camTurnRad;
	pCamera->position.y = 3.0f;
	pCamera->position.z = cosf(alpha) * camTurnRad;
	
	pCamera->LookAt(SpeedPoint::SVector3(0,0,0));
	//CalcLookAt(*pCamera, testObject.vPosition);
	pCamera->RecalculateViewMatrix();
	
	testTerrain.RenderTerrain();
	testObject.Render();

}

/////////////////////////////////////////////////////////////////////////////////////////////////

bool Test::Stop()
{
	testTerrain.Clear();
	testObject.Clear();
	
	m_pEngine->Shutdown();	
	delete m_pEngine;

	return true;
}

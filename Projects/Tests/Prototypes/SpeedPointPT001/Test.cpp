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


/////////////////////////////////////////////////////////////////////////////////////////////////

bool Test::Start(HWND hWnd, HINSTANCE hInstance)
{
	m_pEngine = new SpeedPoint::SpeedPointEngine();
	m_pEngine->RegisterApplication(this);
	
	SpeedPoint::SSettingsDesc dsc;
	dsc.app.nXResolution = 1024;
	dsc.app.nYResolution = 768;
	dsc.app.hWnd = hWnd;	
	dsc.render.bEnableVSync = true;
	dsc.mask = ENGSETTING_RESOLUTION | ENGSETTING_HWND | ENGSETTING_ENABLEVSYNC;

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

	m_pEngine->FinishInitialization();

	// Initialize viewport

	m_pEngine->GetRenderer()->GetTargetViewport()->Set3DProjection(SpeedPoint::S_PROJECTION_PERSPECTIVE, 50, 0, 0);
	pCamera = m_pEngine->GetRenderer()->GetTargetViewport()->GetCamera();
	pCamera->position = SpeedPoint::SVector3(0, 5.0f, 10.0f);
	pCamera->LookAt(SpeedPoint::SVector3(0, 0, 0));

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

	// Add first geometry	

	SpeedPoint::SInitialGeometryDesc initialGeom;
	initialGeom.pVertices = new SpeedPoint::SVertex[24];
	initialGeom.pIndices = new SpeedPoint::SIndex[36];

	//						  Position		   Normal		Tangent		    UV
	// Top:
	initialGeom.pVertices[ 0] = SpeedPoint::SVertex( 0.5f, 0.5f,-0.5f,	 0.0f, 1.0f, 0.0f,	-1.0f, 0.0f, 0.0f,	0.0f, 0.0f);
	initialGeom.pVertices[ 1] = SpeedPoint::SVertex(-0.5f, 0.5f,-0.5f,	 0.0f, 1.0f, 0.0f,	-1.0f, 0.0f, 0.0f,	1.0f, 0.0f);
	initialGeom.pVertices[ 2] = SpeedPoint::SVertex(-0.5f, 0.5f, 0.5f,	 0.0f, 1.0f, 0.0f,	-1.0f, 0.0f, 0.0f,	1.0f, 1.0f);
	initialGeom.pVertices[ 3] = SpeedPoint::SVertex( 0.5f, 0.5f, 0.5f,	 0.0f, 1.0f, 0.0f,	-1.0f, 0.0f, 0.0f,	0.0f, 1.0f);
			       
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

	// calculate tangents
	SpeedPoint::SVector3 upVec(0, 1.0f, 0);
	for (unsigned int iVtx = 0; iVtx < 24; ++iVtx)
	{
		initialGeom.pVertices[iVtx].CalcTangent(upVec);
	}


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

	testObject.vPosition = SpeedPoint::SVector3(0.0, 0.0, 0.0f);
	testObject.Init(m_pEngine, m_pEngine->GetRenderer(), &initialGeom);

	SpeedPoint::Material& testObjMat = testObject.GetMaterial();
	testObjMat.textureMap = pTestTextureMap;
	testObjMat.normalMap = pTestNormalMap;
	m_pEngine->GetRenderer()->DumpFrameOnce();

}

/////////////////////////////////////////////////////////////////////////////////////////////////

bool Test::Tick()
{	
	// Start the frame pipeline
	if (Failure(m_pEngine->ExecuteFramePipeline()))
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void Test::Render()
{



	testObject.vRotation += SpeedPoint::SVector3(0.02f, 0.03f, 0.01f);
	testObject.vRotation.z = 0.3f;
	testObject.vPosition = SpeedPoint::SVector3(0, 3.0f, -8.0f);
	if (Failure(testObject.Render()))
	{
		m_pEngine->LogE("Rendering failed. Stopping engine...");
		Stop();
	}


}

/////////////////////////////////////////////////////////////////////////////////////////////////

bool Test::Stop()
{
	testObject.Clear();

	m_pEngine->GetLog()->SaveToFile("Application.log", true);
	m_pEngine->Shutdown();	
	delete m_pEngine;

	return true;
}

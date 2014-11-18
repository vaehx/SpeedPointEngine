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

	m_pEngine->GetSettings()->Set(dsc);
	m_pEngine->InitializeLogger(&logHandler);
	m_pEngine->GetLog()->SetLogLevel(SpeedPoint::ELOGLEVEL_DEBUG);
	m_pEngine->InitializeFramePipeline();		
	m_pEngine->InitializeRenderer(SpeedPoint::S_DIRECTX11, SpeedPoint::DirectX11Renderer::GetInstance(), true);
	m_pEngine->InitializeResourcePool();	

	m_pEngine->FinishInitialization();

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void Test::OnInitGeometry()
{	
	// Load resources

	SpeedPoint::ITexture* pTestTexture;
	SpeedPoint::SResult res = m_pEngine->GetResources()->AddTexture("..\\..\\res\\tex02.bmp", 512, 512, "tex01", &pTestTexture, nullptr);

	// Add first geometry	

	SpeedPoint::SInitialGeometryDesc initialGeom;
	initialGeom.pVertices = new SpeedPoint::SVertex[14];
	initialGeom.pIndices = new SpeedPoint::SIndex[36];

	//						  Position		   Normal		Tangent		    UV
	initialGeom.pVertices[0] = SpeedPoint::SVertex( 0.5f,  0.5f, -0.5f,		0, 1.0f, 0,	-1.0f, 0.0f, 0.0f,	0.0f, 0.0f);
	initialGeom.pVertices[1] = SpeedPoint::SVertex(-0.5f,  0.5f, -0.5f,		0, 1.0f, 0,	-1.0f, 0.0f, 0.0f,	1.0f, 0.0f);
	initialGeom.pVertices[2] = SpeedPoint::SVertex(-0.5f,  0.5f,  0.5f,		0, 1.0f, 0,	-1.0f, 0.0f, 0.0f,	1.0f, 1.0f);
	initialGeom.pVertices[3] = SpeedPoint::SVertex( 0.5f,  0.5f,  0.5f,		0, 1.0f, 0,	-1.0f, 0.0f, 0.0f,	0.0f, 1.0f);

	initialGeom.pVertices[4] = SpeedPoint::SVertex( 0.5f, -0.5f,  0.5f,		0,0,0,		0,0,0,			0.0f, 2.0f);
	initialGeom.pVertices[5] = SpeedPoint::SVertex(-0.5f, -0.5f,  0.5f,		0,0,0,		0,0,0,			1.0f, 2.0f);
	initialGeom.pVertices[6] = SpeedPoint::SVertex(-0.5f, -0.5f, -0.5f,		0,0,0,		0,0,0,			1.0f, 3.0f);
	initialGeom.pVertices[7] = SpeedPoint::SVertex( 0.5f, -0.5f, -0.5f,		0,0,0,		0,0,0,			0.0f, 3.0f);

	initialGeom.pVertices[8] = SpeedPoint::SVertex( 0.5f,  0.5f,  0.5f,		0,0,0,		0,0,0,			-1.0f, 2.0f);
	initialGeom.pVertices[9] = SpeedPoint::SVertex( 0.5f,  0.5f, -0.5f,		0,0,0,		0,0,0,			-1.0f, 3.0f);
	initialGeom.pVertices[10] = SpeedPoint::SVertex(-0.5f, -0.5f, 0.5f,		0,0,0,		0,0,0,			2.0f, 2.0f);
	initialGeom.pVertices[11] = SpeedPoint::SVertex(-0.5f, -0.5f, -0.5f,		0,0,0,		0,0,0,			2.0f, 3.0f);

	initialGeom.pVertices[12] = SpeedPoint::SVertex( 0.5f,  0.5f, -0.5f,		0,0,0,		0,0,0,			0.0f, 4.0f);
	initialGeom.pVertices[13] = SpeedPoint::SVertex(-0.5f,  0.5f, -0.5f,		0,0,0,		0,0,0,			1.0f, 4.0f);

	initialGeom.nVertices = 14;

	initialGeom.pIndices[0] = 0; initialGeom.pIndices[1] = 1; initialGeom.pIndices[2] = 2;
	initialGeom.pIndices[3] = 2; initialGeom.pIndices[4] = 3; initialGeom.pIndices[5] = 0;

	initialGeom.pIndices[6] = 3; initialGeom.pIndices[7] = 2; initialGeom.pIndices[8] = 5;
	initialGeom.pIndices[9] = 5; initialGeom.pIndices[10] = 4; initialGeom.pIndices[11] = 3;

	initialGeom.pIndices[12] = 4; initialGeom.pIndices[13] = 5; initialGeom.pIndices[14] = 6;
	initialGeom.pIndices[15] = 6; initialGeom.pIndices[16] = 7; initialGeom.pIndices[17] = 4;

	initialGeom.pIndices[18] = 8; initialGeom.pIndices[19] = 4; initialGeom.pIndices[20] = 7;
	initialGeom.pIndices[21] = 7; initialGeom.pIndices[22] = 9; initialGeom.pIndices[23] = 8;

	initialGeom.pIndices[24] = 5; initialGeom.pIndices[25] = 10; initialGeom.pIndices[26] = 11;
	initialGeom.pIndices[27] = 11; initialGeom.pIndices[28] = 6; initialGeom.pIndices[29] = 5;

	initialGeom.pIndices[30] = 7; initialGeom.pIndices[31] = 6; initialGeom.pIndices[32] = 13;
	initialGeom.pIndices[33] = 13; initialGeom.pIndices[34] = 12; initialGeom.pIndices[35] = 7;
	initialGeom.nIndices = 36;

	testObject.vPosition = SpeedPoint::SVector3(0.0, 0.0, 0.0f);
	testObject.Init(m_pEngine, m_pEngine->GetRenderer(), &initialGeom);

	SpeedPoint::Material& testObjMat = testObject.GetMaterial();
	testObjMat.textureMap = pTestTexture;


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



	testObject.vRotation += SpeedPoint::SVector3(0, 0.009f, 0.00f);
	testObject.vPosition = SpeedPoint::SVector3(0, 3.0f, -6.0f);
	testObject.Render();




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

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

/////////////////////////////////////////////////////////////////////////////////////////////////

void OnLogReport(SpeedPoint::SResult res, SpeedPoint::SString msg)
{
	std::cout << msg;
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
	dsc.mask = ENGSETTING_RESOLUTION | ENGSETTING_HWND;

	m_pEngine->GetSettings()->Set(dsc);
	m_pEngine->InitializeLogger();
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
	initialGeom.pVertices = new SpeedPoint::SVertex[4];
	initialGeom.pIndices = new SpeedPoint::SIndex[6];

	//						  Position		   Normal		Tangent		    UV
	initialGeom.pVertices[0] = SpeedPoint::SVertex(-1.0f, 0, -1.0f,		0, 0, -1.0f,	-1.0f, 0.0f, 0.0f,	1.0f, 0.0f);
	initialGeom.pVertices[1] = SpeedPoint::SVertex(-1.0f, 1.0f, 1.0f,	0, 0, -1.0f,	-1.0f, 0.0f, 0.0f,	1.0f, 1.0f);
	initialGeom.pVertices[2] = SpeedPoint::SVertex( 1.0f, 1.0f, 1.0f,	0, 0, -1.0f,	-1.0f, 0.0f, 0.0f,	0.0f, 1.0f);
	initialGeom.pVertices[3] = SpeedPoint::SVertex( 1.0f, 0, -1.0f,		0, 0, -1.0f,	-1.0f, 0.0f, 0.0f,	0.0f, 0.0f);
	initialGeom.nVertices = 4;

	initialGeom.pIndices[0] = 0; initialGeom.pIndices[1] = 1; initialGeom.pIndices[2] = 3;
	initialGeom.pIndices[3] = 1; initialGeom.pIndices[4] = 2; initialGeom.pIndices[5] = 3;	
	initialGeom.nIndices = 6;

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



	testObject.vRotation += SpeedPoint::SVector3(0, 0.0003f, 0.00f);
	testObject.vPosition = SpeedPoint::SVector3(0, 0, -4.0f);
	testObject.Render();




}

/////////////////////////////////////////////////////////////////////////////////////////////////

bool Test::Stop()
{
	testObject.Clear();

	m_pEngine->Shutdown();	
	delete m_pEngine;

	return true;
}

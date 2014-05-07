/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Prototype Test Project Source File
//
//	Remember to be frugal with log calls, as SpeedPoint already throws a huge amount of them!
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "Test.h"
#include "TestUtil.h"

/////////////////////////////////////////////////////////////////////////////////////////////////

void OnLogReport(SpeedPoint::SResult res, SpeedPoint::SString msg)
{
	std::cout << msg;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

bool Test::Start(HWND hWnd, HINSTANCE hInstance)
{
	// Start the engine
	settings.app.bWindowed = true;
	settings.app.hWnd = hWnd;
	if (Failure(m_Engine.Start(settings))) return false;

	// Add a triangle	
	SpeedPoint::ISolid* pSolid;
	pSolid = m_Engine.AddSolid(&testSolid, 0);
	pSolid->Initialize(&m_Engine, false);

	SpeedPoint::SMaterial mat;
	mat.colDiffuse = SpeedPoint::SColor(0.5f, 0.4f, 0.1f, 0.3f);
	pSolid->SetMaterial(mat);

	SpeedPoint::SVertex* pVertices = new SpeedPoint::SVertex[4];
	DWORD* pIndices = new DWORD[6];
	pVertices[0] = SpeedPoint::SVertex(-0.5f,-0.5f,0.5f, 0,0,-1.0f, -1.0f,0.0f,0.0f, 0.0f,0.0f);
	pVertices[2] = SpeedPoint::SVertex( 0.5f, 0.5f,0.0f, 0,0,-1.0f, -1.0f,0.0f,0.0f, 1.0f,1.0f);
	pVertices[3] = SpeedPoint::SVertex( 0.5f,-0.5f,0.0f, 0,0,-1.0f, -1.0f,0.0f,0.0f, 1.0f,0.0f);
	pVertices[1] = SpeedPoint::SVertex(-0.5f, 0.5f,0.0f, 0,0,-1.0f, -1.0f,0.0f,0.0f, 0.0f,1.0f);	
	
	pIndices[0] = 0; pIndices[1] = 1; pIndices[2] = 3;
	pIndices[3] = 1; pIndices[4] = 2; pIndices[5] = 3;

	pSolid->SetGeometryData(pVertices, 4, pIndices, 6);
	delete[] pVertices;
	delete[] pIndices;

	SpeedPoint::SPrimitive myPrimitive;
	myPrimitive.bDraw = true;
	myPrimitive.iFirstIndex = 0;
	myPrimitive.iFirstVertex = 0;
	myPrimitive.iLastIndex = 5;
	myPrimitive.iLastVertex = 3;
	myPrimitive.nPolygons = 2;
	myPrimitive.tRenderType = SpeedPoint::S_PRIM_RENDER_TRIANGLELIST;
	myPrimitive.tType = SpeedPoint::S_PRIM_COMPLEX_PLANE;
	//myPrimitive.iTexture = iTestTexture;
	
	pSolid->AddPrimitive(myPrimitive);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

SpeedPoint::SResult OnRenderCalls(SpeedPoint::SEventParameters* pParams)
{
	SCustomData* pCustomData = (SCustomData*)pParams->Get(SpeedPoint::ePARAM_CUSTOM_DATA);		
	Test* pApplication = pCustomData->pApplication;

	if (Failure(pApplication->m_Engine.RenderSolid(pApplication->testSolid)))
	{
		OutputDebugStringA("Failed render solid!");
		return SpeedPoint::S_ERROR;
	}

	return SpeedPoint::S_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

bool Test::Tick()
{	
	// Start the frame
	if (Failure(m_Engine.BeginFrame()))
		return false;

	// Render
	SpeedPoint::ISolid* pSolid = m_Engine.GetSolid(testSolid);
	if (pSolid && Failure(pSolid->RenderSolid()))
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

bool Test::Stop()
{
	m_Engine.Shutdown();	
	
	return true;
}

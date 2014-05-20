// SpeedPoint Geometry Test Application

#define D3D_DEBUG_INFO

#include "main.h"
#include <iostream>

CApplication	app;
SLogStream	logger;
bool		bAppWentWell;

using namespace std;

// **************************************************************************************************

LRESULT CALLBACK WndProc( HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam )
{
	switch( iMessage )
	{
	case WM_DESTROY:
		app.bRunning = false;
		break;
	case WM_CLOSE:
		app.bRunning = false;
		::PostQuitMessage( 0 );
		break;	
	default:
		return DefWindowProc( hWnd, iMessage, wParam, lParam );
	}

	return false;
}

// **************************************************************************************************

void OnLogReport( SResult res, SString msg )
{
	if( msg.cString != NULL )
	{
		switch( res )
		{
		case S_ERROR: cout << "ERR: "; bAppWentWell = false; break;
		case S_INFO: cout << "INFO: "; break;
		case S_ABORTED: cout << "ABORTED: "; bAppWentWell = false; break;			
		}

		cout << msg.cString << endl;
	}	
}

// **************************************************************************************************

SResult ApplicationMain( void )
{
	MSG msg;

	HINSTANCE hInstance = GetModuleHandle( 0 );	

	// Initialize the logging stream
	logger.Initialize();	
	logger.RegisterHandler( &OnLogReport );

	// Create the window
	logger.Report( S_INFO, "Create Window..." );

	WNDCLASS wc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hbrBackground	= (HBRUSH)GetStockObject( BLACK_BRUSH );
	wc.hCursor		= LoadCursor( hInstance, IDC_ARROW );
	wc.hIcon		= LoadIcon( hInstance, IDI_APPLICATION );
	wc.hInstance		= hInstance;
	wc.lpfnWndProc		= WndProc;
	wc.lpszClassName	= "SGeometryTest";
	wc.lpszMenuName		= NULL;
	wc.style		= CS_VREDRAW | CS_HREDRAW;

	if( FAILED( RegisterClass( &wc ) ) )
	{
		return logger.Report( S_ERROR, "Failed register window class! ");
	}

	if( NULL == ( app.hWnd = CreateWindow(
		"SGeometryTest", "SGeometryTest",
		WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		1024, 768,
		NULL,
		NULL,
		hInstance,
		0 ) ) )
	{
		return logger.Report( S_ERROR, "Failed create Window" );		
	}

	ShowWindow( app.hWnd, SW_SHOWDEFAULT );
	UpdateWindow( app.hWnd );

	// Start the engine
	if( !app.Start() )
	{
		return logger.Report( S_ERROR, "Failed start Engine!" );		
	}

	// Start the message pump
	ZeroMemory( &msg, sizeof( MSG ) );
	app.bRunning = true;

	while( app.bRunning )
	{
		if( GetAsyncKeyState( VK_ESCAPE ) < 0 )
		{
			app.bRunning = false;
			PostQuitMessage( 0 );
		}

		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			if( !app.Tick() )
			{
				app.bRunning = false;
				break;
			}

			if( !app.Render() )
			{
				app.bRunning = false;
				break;
			}
		}
	}

	if( !app.Stop() )
	{
		return logger.Report( S_ERROR, "Failed shutdown engine!" );		
	}

	logger.Clear();
	
	return S_SUCCESS;
}

// **************************************************************************************************

int main( void )
{	
	bAppWentWell = true;

	ApplicationMain();
	if( !bAppWentWell )
	{
		SetFocus( GetConsoleWindow() );
		BringWindowToTop( GetConsoleWindow() );

		char* in = new char[10];
		ZeroMemory( in, 10 );

		cin >> in;
		delete[] in;
	}
	
	return true;
}

// **************************************************************************************************

// Start the engine
SResult CApplication::Start( void )
{
	// Initialize the engine
	SSettings settings;
	settings.hWnd = hWnd;
	settings.nXResolution = 1024;
	settings.nYResolution = 768;
	settings.tyRendererType = S_DIRECTX9;

	// Setup logger
	if( Failure( spEngine.SetCustomLogStream( &logger ) ) )
	{
		return logger.Report( S_ERROR, "Failed setup custom log stream!" );
	}

	if( Failure( spEngine.Start( settings ) ) )
	{
		return logger.Report( S_ERROR, "Failed start SpeedPointEngine!" );		
	}

	// Load the texture for the solid
	SP_ID iTestTexture;
	if( Failure( spEngine.GetResourcePool()->AddTexture( "Textures\\tex_gra_dir.bmp", 256, 256, "test", NULL, &iTestTexture ) ) )
		return logger.Report( S_ERROR, "Failed to load texture!" );

	// Initialize the first solid
	SSolid* pBasicSolid = 0;
	if( NULL == ( pBasicSolid = spEngine.AddSolid( &iBasicSolid ) ) )
	{		
		return logger.Report( S_ERROR, "Failed Add solid!" );		
	}

	if( Failure( pBasicSolid->Initialize( &spEngine, false ) ) )
	{
		return logger.Report( S_ERROR, "Failed initialize solid!" );		
	}

	SMaterial mat;
	mat.colDiffuse = SColor( 0.5f, 0.4f, 0.1f, 0.3f );

	pBasicSolid->SetMaterial( mat );

	SVertex* pVertices = new SVertex[4];
	pVertices[0] = SVertex( -0.5f, -0.5f, 0.5f, 0,0, -1.0f, 0.0f, 0.0f );
	pVertices[1] = SVertex( -0.5f,  0.5f, 0.0f, 0,0, -1.0f, 0.0f, 1.0f );
	pVertices[2] = SVertex(  0.5f,  0.5f, 0.0f, 0,0, -1.0f, 1.0f, 1.0f );
	pVertices[3] = SVertex(  0.5f, -0.5f, 0.0f, 0,0, -1.0f, 1.0f, 0.0f );

	DWORD* pIndices = new DWORD[6];
	pIndices[0] = 0; pIndices[1] = 1; pIndices[2] = 3;
	pIndices[3] = 1; pIndices[4] = 2; pIndices[5] = 3;

	pBasicSolid->SetGeometryData( pVertices, 4, pIndices, 6 );

	SPrimitive myPrimitive;
	myPrimitive.bDraw = true;
	myPrimitive.iFirstIndex = 0;
	myPrimitive.iFirstVertex = 0;
	myPrimitive.iLastIndex = 5;
	myPrimitive.iLastVertex = 3;
	myPrimitive.nPolygons = 2;
	myPrimitive.tRenderType = S_PRIM_RENDER_TRIANGLELIST;
	myPrimitive.tType = S_PRIM_COMPLEX_PLANE;
	myPrimitive.iTexture = iTestTexture;

	pBasicSolid->AddPrimitive( myPrimitive );

	// Setup camera position
	SCamera* pCamera = spEngine.GetViewport( 0 )->GetCamera();

	pCamera->Move( 0, 0, -10.0f );
	//pCamera->Rotate( 0, 1.51f, 0 );


	return logger.Report( S_INFO, "Started Application successfully!" );
}

// **************************************************************************************************

// Tick
SResult CApplication::Tick( void )
{
	SSolid* pBasicSolid = spEngine.GetSolid( iBasicSolid );
	pBasicSolid->Turn( 0, 0.001f, 0 );

	return S_SUCCESS;
}

// **************************************************************************************************

// Render
SResult CApplication::Render( void )
{
	if( Failure( spEngine.BeginFrame() ) )
	{
		return logger.Report( S_ERROR, "Failed Begin Frame!" );
	}

	if( Failure( spEngine.RenderSolid( iBasicSolid ) ) )
	{
		return logger.Report( S_ERROR, "Failed Render solid!" );
	}	

	if( Failure( spEngine.EndFrame( NULL ) ) )
	{
		return logger.Report( S_ERROR, "Failed End Frame!" );
	}

	return S_SUCCESS;
}

// **************************************************************************************************

// Stop
SResult CApplication::Stop( void )
{
	if( Failure( spEngine.Shutdown() ) )
	{
		return logger.Report( S_ERROR, "Failed shutdown engine!" );
	}

	return logger.Report( S_INFO, "Engine shut down properly" );
}
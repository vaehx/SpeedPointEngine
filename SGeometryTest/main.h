// SpeedPoint Geometry Test main header

#include <SpeedPoint.h>
#include <Abstract\SSolid.h>
#include <Abstract\SViewport.h>
#include <SVertex.h>
#include <Windows.h>
#include <SPrimitive.h>
#include <SMaterial.h>

using namespace SpeedPoint;


// Test Application
class CApplication
{
public:
	bool			bRunning;
	bool			bError;
	SpeedPointEngine	spEngine;
	HWND			hWnd;
	SP_ID			iBasicSolid;

	SResult Start( void );
	SResult Tick( void );
	SResult Render( void );		
	SResult Stop( void );
};
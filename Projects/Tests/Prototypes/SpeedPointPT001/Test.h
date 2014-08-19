/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Prototype Test Project Source File
//
//	Remember to be frugal with log calls, as SpeedPoint already throws a huge amount of them!
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#define SP_NO_PHYSICS
#include <SpeedPoint.h>
#include <SpeedPointApplication.h>
#include <Implementation\Geometry\VisibleObject.h>
#include <Windows.h>

// Foward declarate Log Report Handler
void OnLogReport(SpeedPoint::SResult res, SpeedPoint::SString msg);

class Test : public SpeedPoint::SpeedPointApplication
{
private:
	SpeedPoint::VisibleObject testObject;

protected:
	void OnInitGeometry();

public:	
	bool Start(HWND hWnd, HINSTANCE hInstance);
	bool Tick();

	void Render();

	bool Stop();
}; 
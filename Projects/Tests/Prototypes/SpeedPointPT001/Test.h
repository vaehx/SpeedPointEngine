/////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Prototype Test Project Source File
//
//	Remember to be frugal with log calls, as SpeedPoint already throws a huge amount of them!
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SpeedPoint.h>
#include <Windows.h>

// Foward declarate Log Report Handler
void OnLogReport(SpeedPoint::SResult res, SpeedPoint::SString msg);

class Test;

struct SCustomData
{
	Test* pApplication;
};

class Test
{
public:
	SpeedPoint::SpeedPointEngine m_Engine;
	SpeedPoint::SSettings settings;
	SpeedPoint::SP_ID testSolid;

	bool Start(HWND hWnd, HINSTANCE hInstance);
	bool Tick();
	bool Stop();
}; 
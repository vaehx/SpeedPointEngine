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
#include <Abstract\IObject.h>
#include <Implementation\Geometry\StaticObject.h>
#include <Windows.h>


#define EXEC_CONDITIONAL(res, exec) if (SpeedPoint::Success(res)) { res = exec; }


class TestLogHandler : public SpeedPoint::ILogHandler
{
public:
	virtual void OnLog(SpeedPoint::SResult res, const SpeedPoint::SString& formattedMsg);
};

class Test : public SpeedPoint::SpeedPointApplication
{
private:
	SpeedPoint::StaticObject testObject;
	SpeedPoint::SCamera* pCamera;
	TestLogHandler logHandler;

protected:
	void OnInitGeometry();

public:		
	virtual void OnLogReport(SpeedPoint::SResult res, const SpeedPoint::SString& msg);
	bool Start(HWND hWnd, HINSTANCE hInstance);
	bool Tick();

	void Render();

	bool Stop();
}; 
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
#include <Implementation\Geometry\Terrain.h>
#include <Implementation\Geometry\Scene.h>
#include <Windows.h>


#define EXEC_CONDITIONAL(res, exec) if (SpeedPoint::Success(res)) { res = exec; }


enum EKey
{
	KEY_MOVE_FORWARD,
	KEY_MOVE_BACKWARD,
	KEY_MOVE_LEFT,
	KEY_MOVE_RIGHT,
	KEY_MOVE_UP,
	KEY_MOVE_DOWN,
	KEY_SHIFT
};


class TestLogHandler : public SpeedPoint::ILogHandler
{
public:
	virtual void OnLog(SpeedPoint::SResult res, const SpeedPoint::SString& formattedMsg);
};

class Test : public SpeedPoint::SpeedPointApplication
{
private:
	float alpha;
	unsigned int nDumpedFrames;
	SpeedPoint::StaticObject testObject;
	SpeedPoint::IStaticObject* pTest3DSObject;
	SpeedPoint::IScene* m_pScene;
	SpeedPoint::SCamera* pCamera;
	TestLogHandler logHandler;

protected:
	void OnInitGeometry();

public:		
	virtual void OnLogReport(SpeedPoint::SResult res, const SpeedPoint::SString& msg);
	bool KeyPressed(EKey key) const;
	bool Start(HWND hWnd, HINSTANCE hInstance);
	bool Tick();

	void Render();

	bool Stop();
}; 
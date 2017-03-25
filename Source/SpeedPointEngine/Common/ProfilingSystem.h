//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SPrerequisites.h"
#include "ChunkedObjectPool.h"
#include <string>

SP_NMSPACE_BEG

// Provides methods for counting the elapsed time using the high performance counter.
struct S_API ProfilingTimer
{
	LONGLONG elapsed; // number of elapsed counts
	LONGLONG startCount;
	LONGLONG freq; // counts per second
	bool running;

	ProfilingTimer(const LONGLONG& _freq)
		: running(false), startCount(0), elapsed(0), freq(_freq) {}

	ProfilingTimer()
		: ProfilingTimer(0) {}

	// Starts the timer clearing the previous duration.
	// Does nothing if the timer is already running.
	inline void Start()
	{
		if (running)
			return;

		running = true;
		elapsed = 0;

		if (freq == 0)
			QueryPerformanceFrequency((LARGE_INTEGER*)&freq);

		QueryPerformanceCounter((LARGE_INTEGER*)&startCount);
	}

	// Restarts the timer keeping the previous duration.
	// Does nothing if the timer is already running.
	inline void Resume()
	{
		if (running)
			return;

		running = true;

		QueryPerformanceCounter((LARGE_INTEGER*)&startCount);
	}

	inline void Stop()
	{
		if (!running)
			return;
	
		static LONGLONG current;
		QueryPerformanceCounter((LARGE_INTEGER*)&current);

		elapsed += current - startCount;
		running = false;
	}

	// Returns the elapsed time in seconds
	inline double GetDuration() const
	{
		if (freq == 0)
			return 0.0;
		else
			return (double)elapsed / (double)freq;
	}
};

// Describes a (nested) code section that is being profiled by the ProfilingSystem
struct S_API ProfilingSection
{
	ProfilingTimer timer;
	std::string name;
	unsigned short level;

	ProfilingSection(const LONGLONG& timerFreq)
		: timer(timerFreq) {}

	ProfilingSection()
		: ProfilingSection(0) {}
};

class S_API ProfilingSystemIntrnl
{
private:
	bool m_bEnabled;
	unsigned short m_CurrentLevel; // Indentation level
	unsigned char m_CurrentSectionPool;
	vector<ProfilingSection> m_Sections[2];
	LONGLONG m_PCFrequency; // Performance counter frequency

	ProfilingTimer m_FrameTimer;
	double m_LastFrameDuration;

	ProfilingTimer m_RecentSampleTimer; // used to sample min/max framerate every n seconds
	double m_MinFrameDuration;
	double m_MaxFrameDuration;
	double m_MinRecentFrameDuration; // updated every n seconds
	double m_MaxRecentFrameDuration; // updated every n seconds

public:
	ProfilingSystemIntrnl();

	bool IsEnabled() const;
	void Enable();
	void Disable();

	unsigned int StartSection(const char* name);
	void ResumeSection(unsigned int sectionId);
	void EndSection(unsigned int sectionId);

	// Should be called at the end of each frame
	void NextFrame();

	const vector<ProfilingSection>* GetLastFrameSections();

	// In seconds
	const double& GetLastFrameDuration() const { return m_LastFrameDuration; }

	// In seconds
	const double& GetMinRecentFrameDuration() const { return m_MinRecentFrameDuration; }

	// In seconds
	const double& GetMaxRecentFrameDuration() const { return m_MaxRecentFrameDuration; }
};

// Frame profiling system
// !! This system is not multi-threading capable yet !!
class S_API ProfilingSystem
{
public:
	static ProfilingSystemIntrnl* Get()
	{
		static ProfilingSystemIntrnl* profilingSystem = 0;

		if (!profilingSystem)
			profilingSystem = new ProfilingSystemIntrnl();

		return profilingSystem;
	}

	// Starts a new profiling section and returns the id of the section.
	// Returns UINT_MAX if profiling is not enabled or the section pool reached
	// its maximum size.
	inline static unsigned int StartSection(const char* name)
	{
		return Get()->StartSection(name);
	}

	// After EndSection(), this method can be called to restart the section
	// without resetting its timer stats.
	inline static void ResumeSection(unsigned int sectionId)
	{
		Get()->ResumeSection(sectionId);
	}

	// timerId - id returned by StartSection()
	inline static void EndSection(unsigned int sectionId)
	{
		Get()->EndSection(sectionId);
	}
};

SP_NMSPACE_END

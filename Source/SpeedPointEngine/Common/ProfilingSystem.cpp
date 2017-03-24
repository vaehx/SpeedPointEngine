//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2017 Pascal Rosenkranz, All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "ProfilingSystem.h"

SP_NMSPACE_BEG

ProfilingSystemIntrnl::ProfilingSystemIntrnl()
	: m_bEnabled(true),
	m_CurrentSectionPool(0),
	m_CurrentLevel(0),
	m_LastFrameDuration(0.0),
	m_MinFrameDuration(DBL_MAX),
	m_MaxFrameDuration(DBL_MIN),
	m_MinRecentFrameDuration(0.0),
	m_MaxRecentFrameDuration(0.0)
{
	// The frequency of the performance counter is fixed at system boot and consistent across all processors
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_PCFrequency);

	// Reserve some sections already, for a small performance boost on startup
	m_Sections[0].reserve(10);
	m_Sections[1].reserve(10);
}

S_API bool ProfilingSystemIntrnl::IsEnabled() const
{
	return m_bEnabled;
}

S_API void ProfilingSystemIntrnl::Enable() 
{
	m_bEnabled = true;
}

S_API void ProfilingSystemIntrnl::Disable()
{
	m_bEnabled = false;
}

S_API unsigned int ProfilingSystemIntrnl::StartSection(const char* name)
{
	if (!m_bEnabled)
		return UINT_MAX;

	// On application start, start this timer with the very first section
	if (!m_RecentSampleTimer.running)
		m_RecentSampleTimer.Start();

	vector<ProfilingSection>* pool = &m_Sections[m_CurrentSectionPool];
	if (pool->size() >= UINT_MAX)
		return UINT_MAX;

	ProfilingSection section(m_PCFrequency);
	section.name = name;
	section.level = m_CurrentLevel++; // post-increment: 0, 1, 2, ...

	pool->push_back(section);
	pool->back().timer.Start();

	return pool->size() - 1; // direct indices
}

S_API void ProfilingSystemIntrnl::ResumeSection(unsigned int sectionId)
{
	if (!m_bEnabled)
		return;

	vector<ProfilingSection>* pool = &m_Sections[m_CurrentSectionPool];
	if (sectionId >= pool->size())
		return;

	ProfilingSection* section = &pool->at(sectionId);
	section->timer.Resume();

	++m_CurrentLevel;
}

S_API void ProfilingSystemIntrnl::EndSection(unsigned int sectionId)
{
	if (!m_bEnabled)
		return;

	vector<ProfilingSection>* pool = &m_Sections[m_CurrentSectionPool];
	if (sectionId >= pool->size())
		return;

	ProfilingSection* section = &pool->at(sectionId);
	section->timer.Stop();

	--m_CurrentLevel;
}

S_API void ProfilingSystemIntrnl::NextFrame()
{
	if (!m_bEnabled)
		return;

	// Stop all current section timers that are still running
	vector<ProfilingSection>* currentSections = &m_Sections[m_CurrentSectionPool];
	for (auto itSection = currentSections->begin(); itSection != currentSections->end(); ++itSection)
	{
		itSection->timer.Stop();
	}

	// Rotate section pools
	m_CurrentSectionPool ^= 0x1; // + 1 % 2
	m_Sections[m_CurrentSectionPool].clear();

	// Reset current indentation level
	m_CurrentLevel = 0;

	// Capture last frame duration
	m_FrameTimer.Stop();
	m_RecentSampleTimer.Stop();

	m_LastFrameDuration = m_FrameTimer.GetDuration();

	if (m_LastFrameDuration > m_MaxFrameDuration)
		m_MaxFrameDuration = m_LastFrameDuration;
	if (m_LastFrameDuration < m_MinFrameDuration)
		m_MinFrameDuration = m_LastFrameDuration;

	// Reset min/max framerate each second
	if (m_RecentSampleTimer.GetDuration() >= 1.0)
	{
		m_MinRecentFrameDuration = m_MinFrameDuration;
		m_MaxRecentFrameDuration = m_MaxFrameDuration;
		m_MinFrameDuration = DBL_MAX;
		m_MaxFrameDuration = DBL_MIN;
		m_RecentSampleTimer.Start();
	}
	else
	{
		m_RecentSampleTimer.Resume();
	}

	m_FrameTimer.Start();
}

S_API const vector<ProfilingSection>* ProfilingSystemIntrnl::GetLastFrameSections()
{
	return &m_Sections[m_CurrentSectionPool ^ 0x1];
}

SP_NMSPACE_END

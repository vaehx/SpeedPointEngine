//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	(c) 2011-2015 Pascal R. aka iSmokiieZz
//	All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SPrerequisites.h>
#include <Abstract\IFramePipeline.h>
#include <chrono>
#include <vector>
#include <string>

using std::vector;
using std::string;

SP_NMSPACE_BEG

struct S_API IRenderPipeline;
struct S_API IGameEngine;


//////////////////////////////////////////////////////////////////////////////////////////////////

struct S_API SFrameDebugTimer
{
	string name;
	unsigned int id;
	//std::chrono::high_resolution_clock::time_point tp1, tp2;	 
	LONGLONG elapsed;
	LONGLONG startCount;
	LONGLONG freq;	
	bool running;

	SFrameDebugTimer() : running(false), elapsed(0), freq(0) {}

	// Clears previous duration
	inline void Start()
	{
		running = true;
		elapsed = 0;

		//tp1 = std::chrono::high_resolution_clock::now();
		if (freq == 0)
			QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
		
		QueryPerformanceCounter((LARGE_INTEGER*)&startCount);		
	}

	// Keeps the current duration
	inline void Resume()
	{		
		running = true;

		QueryPerformanceCounter((LARGE_INTEGER*)&startCount);
	}

	inline void Stop()
	{
		if (!running)
			return;		

		//tp2 = std::chrono::high_resolution_clock::now();
		//std::chrono::duration<double> d = tp2 - tp1;		
		LONGLONG current;
		QueryPerformanceCounter((LARGE_INTEGER*)&current);

		elapsed += current - startCount;
		running = false;
	}

	inline double GetDuration() const
	{
		return (double)elapsed / (double)freq;
	}
};

struct S_API SFrameDebugInfo
{
	SFrameDebugTimer frameTimer;
	SFrameDebugTimer tickTimer; // for the tick function
	SFrameDebugTimer renderTimer; // for the render function

	unsigned long frameCounter;	// enough for 4294967296 ms (+49 days)
	unsigned long lastFrameCounter; // ms

	double lastFrameDuration;

	double frameTimeAcc,
		minFrameTime,
		maxFrameTime;

	double lastMinFrameTime, lastMaxFrameTime;
	
	unsigned int stagedBudgetTimers; // id of the vector of the budget timers staged for rendering
	unsigned int budgetTimerIndent;
	vector<SFrameDebugTimer> budgetTimers[2];	// swapped after each frame

	ILINE vector<SFrameDebugTimer>& GetUnstagedBudgetTimers()
	{
		return budgetTimers[((stagedBudgetTimers + 1) + 1) & 1]; // % 2
	}

	ILINE const vector<SFrameDebugTimer>& GetStagedBudgetTimers() const
	{
		return budgetTimers[stagedBudgetTimers];
	}
};

class S_API CDebugInfo
{
public:
	IGameEngine* m_pEngine;
	SFontRenderSlot* m_pViewMtxRows[4];
	SFontRenderSlot* m_pCamStats;
	SFontRenderSlot* m_pFPS;
	SFontRenderSlot* m_pFrameTimes;
	SFontRenderSlot* m_pTerrain;

	CDebugInfo();

	void Update(SCamera* pCamera, double fps, const SFrameDebugInfo& fdi);

	// Initializes the given variable with a font render slot.
	// If the font render slot is already initialized, the function does nothing.
	inline void InitFontRenderSlot(SpeedPoint::SFontRenderSlot** m_pFRS,
		bool bRightAlign, bool keep, const SpeedPoint::SColor& color, unsigned int x, unsigned int y,
		SpeedPoint::EFontSize fontSize = SpeedPoint::eFONTSIZE_NORMAL);

	void Clear();
};


//////////////////////////////////////////////////////////////////////////////////////////////////

// Helps to shorten clear method by providing a destructor
struct S_API SFramePipeSectionPtr
{
	IFramePipelineSection* pSection;
	usint32 ref;

	SFramePipeSectionPtr() : pSection(nullptr), ref(0) {}
	SFramePipeSectionPtr(IFramePipelineSection* pSec) : pSection(pSec), ref(0) {}
	SFramePipeSectionPtr(const SFramePipeSectionPtr& o)
	{
		pSection = o.pSection;
		ref = o.ref + 1;
	}

	~SFramePipeSectionPtr()
	{
		Clear();
	}

	void Clear()
	{
		if (IS_VALID_PTR(pSection) && ref == 0)
		{
			pSection->Clear();
			delete pSection;
			pSection = 0;
		}
	}

	IFramePipelineSection* operator ->() const
	{
		return pSection;
	}

	operator IFramePipelineSection*() const
	{
		return pSection;
	}
};



//////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_FRAMEPIPELINE_SECTIONS 8

// SpeedPoint Frame Pipeline Engine
// This is the main Pipeline of the Game Engine and handles all other pipeline.
// It also uses an Event System, so that you have access to all pipeline states in your CPP code or your Script
class S_API FramePipeline : public IFramePipeline
{
private:						
	IGameEngine* m_pEngine;

	bool m_bStartedFPSTimer;		
	std::chrono::high_resolution_clock m_HighResClock;		
	std::chrono::time_point<std::chrono::high_resolution_clock>* m_pFPSBeginTimestamp; // is a ptr due to STL. DO NEVER make this variable public!
	std::chrono::time_point<std::chrono::high_resolution_clock>* m_pFrameBeginTimestamp; // is a ptr due to STL. DO NEVER make this variable public!
	std::chrono::time_point<std::chrono::high_resolution_clock>* m_pFrameEndTimestamp; // is a ptr due to STL. DO NEVER make this variable public!
	std::chrono::duration<f64>* m_pdLastFrameDuration; // is a ptr due to STL. DO NEVER make this variable public!
	usint32 m_nFPSTimerFrameCount;
	f64 m_dLastFPS;
	f32 m_fLastFPS;			// extra variable due to possible information loss with float

	CDebugInfo m_DebugInfo;
	SFrameDebugInfo m_FrameDebugInfo;

	// TODO: maybe add m_bLocked to prevent any change of m_pSections during execution
	SFramePipeSectionPtr m_pSections[MAX_FRAMEPIPELINE_SECTIONS];
	unsigned int m_nUsedFramepipelineSections;
	SFramePipeSectionPtr m_pCurrentSection;	

public:
	// Default constructor
	FramePipeline();

	// Default destructor
	~FramePipeline()			
	{
		Clear(); // important due to STL timestamps (chrono)		
	}

	// Initialize the frame pipeline with given SpeedPointEngine ptr
	// Without the engine being set, the framepipeline will most likely do nothing
	virtual void Initialize(IGameEngine* pGameEngine);


	virtual void Clear();

	virtual IFramePipelineSection* GetCurrentSection() const
	{
		return m_pCurrentSection;
	}


	virtual SResult RegisterSection(IFramePipelineSection* pSection);	
	virtual SResult ExecuteSections(usint32 iSkippedSections = DEFAULT_SKIPPED_SECTIONS);

	ILINE virtual unsigned int StartBudgetTimer(const char* name);
	ILINE virtual void ResumeBudgetTimer(unsigned int timerId);
	ILINE virtual void StopBudgetTimer(unsigned int timerId);

	virtual void RenderDebugInfo();
};	

typedef class FramePipeline FrameEngine;


SP_NMSPACE_END
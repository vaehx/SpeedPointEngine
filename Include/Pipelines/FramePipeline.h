//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This file is part of the SpeedPoint Game Engine
//
//	(c) 2011-2014 Pascal R. aka iSmokiieZz
//	All rights reserved.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SPrerequisites.h>
#include <Abstract\IFramePipeline.h>
#include <chrono> // Notice: Requires at least C++11 !

SP_NMSPACE_BEG

struct S_API IRenderPipeline;
struct S_API IGameEngine;

//////////////////////////////////////////////////////////////////////////////////////////////////

// Helps to shorten clear method by providing a destructor
struct SFramePipeSectionPtr
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

	// TODO: maybe add m_bLocked to prevent any change of m_pSections during execution
	std::vector<SFramePipeSectionPtr>* m_pSections;	
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
};	

typedef class FramePipeline FrameEngine;


SP_NMSPACE_END
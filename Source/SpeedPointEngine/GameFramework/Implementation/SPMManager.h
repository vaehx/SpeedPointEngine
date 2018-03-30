#pragma once

#include <Common\SPrerequisites.h>
#include <3DEngine\IGeometry.h>
#include <FileSPM.h>
#include <map>
#include <queue>

SP_NMSPACE_BEG

using std::map;
using std::queue;

// Caches loaded SPM files so they can be accessed by multiple subsystems
// Also provides helper methods for converting SPM structures into engine structures
class S_API SPMManager
{
private:
	unsigned int m_NumMaxCached;
	map<string, CSPMLoader*> m_Cache;
	queue<string> m_EvictionQueue;

public:
	SPMManager(unsigned int numMaxCached);
	~SPMManager();

	// Does not re-load the file if it is still cached
	// absResResource - Absolute resource path to the SPM file
	const CSPMLoader* Load(const string& absResPath);
	
	// If name is empty, uses the absolute resource path of the loaded spm file
	IGeometry* CreateGeometryFromSPM(const CSPMLoader* pSPM, const string& name = "");

	static void FillInitialGeometryDescFromSPM(const CSPMLoader* pSPM, SInitialGeometryDesc& geomDesc);
	static void ClearInitialGeometryDesc(SInitialGeometryDesc& geomDesc);

	static geo::shape* ConvertSPMColShapeToGeoShape(const SSPMColShape* spmShape);

	void Clear();
};

SP_NMSPACE_END


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	SpeedPoint Game Engine
//	Copyright (c) 2011-2018 Pascal Rosenkranz, All rights reserved.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SPrerequisites.h"
#include "Vector3.h"

SP_NMSPACE_BEG

void QuickHull2(
	const Vec3f* poly,
	const unsigned int npolyverts,
	Vec3f** phull,
	unsigned int* pnhullverts,
	unsigned int** phullindices = 0,
	unsigned int* pnfaces = 0);

SP_NMSPACE_END

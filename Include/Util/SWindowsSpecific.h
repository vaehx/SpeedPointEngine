// ******************************************************************************************

//	This file is part of the SpeedPoint Game Engine.

//	(c) 2011-2014 Pascal R. aka iSmokiieZz
//	All rights reserved

// ******************************************************************************************

#ifndef _SWINDOWSSPECIFIC_H
#define _SWINDOWSSPECIFIC_H

#pragma once

// Prevent loading windows header if client application uses MFC libraries
#ifdef _NO_WINDOWS_HEADER
typedef unsigned int UINT;
typedef long HRESULT;
#else
#include <Windows.h>
#endif

#endif
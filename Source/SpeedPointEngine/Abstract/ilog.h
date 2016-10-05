//////////////////////////////////////////////////////////////////////////////////
//
// This file is part of the SpeedPointEngine
// Copyright (c) 2011-2014, iSmokiieZz
// ------------------------------------------------------------------------------
// Filename:	ILog.h
// Created:	11/15/2014 by iSmokiieZz
// Description:
// -------------------------------------------------------------------------------
// History:
//
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SPrerequisites.h"
#include "Matrix.h"

SP_NMSPACE_BEG

struct S_API IFileLogHandler
{
	virtual void OnLog(SResult res, const string& formattedMsg) = 0;
};

enum S_API ELogLevel
{
	ELOGLEVEL_INFO = 0,
	ELOGLEVEL_WARN = 1,
	ELOGLEVEL_DEBUG = 2
};

struct S_API IFileLog
{
	virtual void Clear() = 0;
	virtual SResult SetLogFile(const string& file) = 0;
	virtual SResult RegisterLogHandler(IFileLogHandler* pLogHandler) = 0;
	virtual SResult SetLogLevel(ELogLevel loglevel) = 0;
	virtual ELogLevel GetLogLevel() const = 0;
	virtual SResult Log(SResult res, const string& msg) = 0;
	virtual SResult LogE(const string& msg)
	{
		return Log(S_ERROR, msg);
	}
	virtual SResult LogI(const string& msg)
	{
		return Log(S_INFO, msg);
	}
	virtual SResult LogW(const string& msg)
	{
		return Log(S_WARN, msg);
	}
	virtual SResult LogD(const string& msg)
	{
		return Log(S_DEBUG, msg);
	}

	// Writes cached lines to the disk
	virtual void ReleaseIOQueue() = 0;
};

SP_NMSPACE_END
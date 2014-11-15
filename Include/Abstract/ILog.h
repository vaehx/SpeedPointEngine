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

#include <SPrerequisites.h>

SP_NMSPACE_BEG

struct S_API ILogHandler
{
	virtual void OnLog(SResult res, const SString& formattedMsg) = 0;
};

enum S_API ELogLevel
{
	ELOGLEVEL_INFO = 0,
	ELOGLEVEL_WARN = 1,
	ELOGLEVEL_DEBUG = 2
};

struct S_API ILog
{
	virtual void Clear() = 0;
	virtual SResult SaveToFile(const SString& file, bool replace = false) = 0;
	virtual SResult RegisterLogHandler(ILogHandler* pLogHandler) = 0;
	virtual SResult SetLogLevel(ELogLevel loglevel) = 0;
	virtual ELogLevel GetLogLevel() const = 0;
	virtual SResult Log(SResult res, const SString& msg) = 0;
	virtual SResult LogE(const SString& msg)
	{
		return Log(S_ERROR, msg);
	}
	virtual SResult LogI(const SString& msg)
	{
		return Log(S_INFO, msg);
	}
	virtual SResult LogW(const SString& msg)
	{
		return Log(S_WARN, msg);
	}
	virtual SResult LogD(const SString& msg)
	{
		return Log(S_DEBUG, msg);
	}
};

SP_NMSPACE_END
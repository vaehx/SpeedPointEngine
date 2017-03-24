#pragma once

#include <Common\CLog.h>
#include <deque>
#include <vector>
#include <fstream>

SP_NMSPACE_BEG

// Responsible for listening to new log entries and writing them
// thread-safely and queued to disk.
class S_API FileLogListener : public ILogListener
{
private:
	std::ofstream m_LogFile;
	std::deque<string> m_LineQueue;

public:
	FileLogListener(const string& filename);
	~FileLogListener();

	void Clear();
	void ReleaseQueue();

public: // ILogListener
	virtual void OnLog(SResult res, const string& msg);
};

SP_NMSPACE_END

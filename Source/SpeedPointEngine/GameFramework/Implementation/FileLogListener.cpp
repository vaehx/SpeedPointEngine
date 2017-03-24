#include "FileLogListener.h"

using std::ofstream;

SP_NMSPACE_BEG

FileLogListener::FileLogListener(const string& filename)
{
	if (m_LogFile.is_open())
		m_LogFile.close();

	m_LogFile.open(filename, std::ofstream::trunc);
}

FileLogListener::~FileLogListener()
{
	Clear();
}

void FileLogListener::OnLog(SResult res, const string& msg)
{
	m_LineQueue.push_back(msg);
}

void FileLogListener::ReleaseQueue()
{
	if (m_LineQueue.size() == 0 || !m_LogFile)
		return;

	unsigned int nLines = m_LineQueue.size();
	for (unsigned int i = 0; i < nLines; ++i)
	{
		m_LogFile << m_LineQueue.front();
		m_LineQueue.pop_front();
	}
}

void FileLogListener::Clear()
{
	if (m_LogFile.is_open())
		m_LogFile.close();

	m_LineQueue.clear();
}

SP_NMSPACE_END

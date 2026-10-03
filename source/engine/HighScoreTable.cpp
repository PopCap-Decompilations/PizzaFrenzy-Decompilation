#include "HighScoreTable.h"

#include <algorithm>

#include "Application.h"
#include "HighScoreListHandler.h"
#include "InputStream.h"
#include "InputStreamReader.h"
#include "Thread.h"
#include "URLConnection.h"
#include "Xml.h"

namespace engine
{
	// 0x479840 (folded)
	const std::string& HighScoreTable::getTitle() const
	{
		return m_title;
	}

	// 0x479850 (folded)
	const std::string& HighScoreTable::getName() const
	{
		return m_name;
	}

	// 0x482C90
	int HighScoreTable::getNewEntryIndex() const
	{
		return m_newEntryIndex;
	}

	// 0x482CA0
	void HighScoreTable::clearNewEntryIndex()
	{
		m_newEntryIndex = -1;
	}

	// 0x482CB0
	void HighScoreTable::setStatus(int status)
	{
		if (m_status != status)
			m_status = status;
	}

	// 0x482CC0
	int HighScoreTable::getStatus() const
	{
		EnterCriticalSection(&m_lock);
		int status = m_status;
		LeaveCriticalSection(&m_lock);
		return status;
	}

	// 0x482CE0
	int HighScoreTable::getCount() const
	{
		return m_entries.size();
	}

	// 0x482D00
	bool HighScoreTable::isEmpty() const
	{
		return m_entries.empty();
	}

	// 0x482D70
	void HighScoreTable::sendRequest(URLConnection* request)
	{
		Application* application = getApplication();
		if (m_status == 1)
			return;
		if (request != 0)
		{
			m_request = request;
			RefPtr<Thread> thread(application->createThread(this));
			if (thread->start())
			{
				setStatus(1);
				return;
			}
		}
		setStatus(3);
	}

	// 0x482E60
	int HighScoreTable::run()
	{
		int status;
		if (m_request->connect())
		{
			RefPtr<InputStream> stream(m_request->getInputStream());
			RefPtr<InputStreamReader> reader(new InputStreamReader(stream));
			RefPtr<HighScoreListHandler> handler(new HighScoreListHandler(this));
			try
			{
				XmlParseScope scope;
				parseXml(handler, reader);
				status = 2;
			}
			catch (...)
			{
				status = 3;
			}
		}
		else
		{
			status = 3;
		}
		EnterCriticalSection(&m_lock);
		m_request = 0;
		setStatus(status);
		LeaveCriticalSection(&m_lock);
		return 0;
	}

	// 0x4830A0
	HighScoreTable::iterator HighScoreTable::begin()
	{
		return m_entries.begin();
	}

	// 0x4830B0
	HighScoreTable::iterator HighScoreTable::end()
	{
		return m_entries.end();
	}

	// 0x4830C0
	HighScoreEntry* HighScoreTable::getEntry(unsigned int index) const
	{
		if (index < m_entries.size())
			return m_entries[index];
		return 0;
	}

	// 0x4830F0
	int HighScoreTable::getRankFor(HighScoreEntry* entry)
	{
		iterator it = findPosition(entry);
		if (it != m_entries.end())
			return (*it)->getRank();
		if (m_entries.size() < (unsigned int)m_maxEntries)
			return m_entries.size() + 1;
		return 0;
	}

	// 0x483160
	void HighScoreTable::selectRank(int rank)
	{
		iterator it = std::find_if(m_entries.begin(), m_entries.end(), RankEquals(rank));
		if (it != m_entries.end())
			m_newEntryIndex = it - m_entries.begin();
		else if (rank > 0)
			m_newEntryIndex = m_entries.end() - m_entries.begin() - 1;
		else
			m_newEntryIndex = -1;
	}

	// 0x483220
	HighScoreTable::~HighScoreTable()
	{
		for (iterator it = m_entries.begin(); it != m_entries.end(); ++it)
			(*it)->release();
		m_request = 0;
	}

	// 0x483350
	void HighScoreTable::clear()
	{
		for (iterator it = m_entries.begin(); it != m_entries.end(); ++it)
			(*it)->release();
		m_entries.clear();
	}

	// 0x4833C0 (folded)
	void HighScoreTable::setTitle(const std::string& title)
	{
		m_title = title;
	}

	// 0x483630
	HighScoreTable::HighScoreTable(const std::string& name, int maxEntries)
		: m_name(name), m_status(0), m_maxEntries(maxEntries)
	{
		InitializeCriticalSection(&m_lock);
	}

	// 0x483720
	bool HighScoreTable::insert(HighScoreEntry* entry)
	{
		iterator it = std::lower_bound(m_entries.begin(), m_entries.end(), entry, ByRank());
		unsigned int index = it - m_entries.begin();
		if (index < (unsigned int)m_maxEntries)
		{
			m_newEntryIndex = index;
			entry->addRef();
			iterator position = m_entries.insert(it, entry);
			int rank = entry->getRank();
			for (; position != m_entries.end(); ++position)
				(*position)->setRank(rank++);
			while (m_entries.size() > (unsigned int)m_maxEntries)
			{
				m_entries.back()->release();
				m_entries.pop_back();
			}
			return true;
		}
		return false;
	}

	// 0x483840
	bool HighScoreTable::addEntry(HighScoreEntry* entry)
	{
		m_newEntryIndex = -1;
		int rank = getRankFor(entry);
		if (rank > 0)
		{
			entry->setRank(rank);
			return insert(entry);
		}
		return false;
	}
}

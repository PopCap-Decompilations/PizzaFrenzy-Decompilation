#include "HighScoreListHandler.h"

#include "HighScoreEntry.h"
#include "HighScoreTable.h"
#include "RefPtr.h"
#include "StringUtil.h"

namespace engine
{
	// 0x494870
	void HighScoreListHandler::startElement(const std::string& name, const Properties& attributes)
	{
		if (name == "highscores")
			m_highlight = 0;
		else if (name == "entry")
			m_entryAttributes.clear();
		m_text.clear();
	}

	// 0x4948F0
	void HighScoreListHandler::characters(const std::string& text)
	{
		m_text += text;
	}

	// 0x494910
	void HighScoreListHandler::endElement(const std::string& name)
	{
		if (name == "highscores")
		{
			m_table->selectRank(m_highlight);
		}
		else if (name == "entry")
		{
			RefPtr<HighScoreEntry> entry(m_table->createEntry());
			entry->load(m_entryAttributes);
			if (entry->hasRank())
				m_table->insert(entry);
			else
				m_table->addEntry(entry);
		}
		else if (name == "name")
		{
			m_entryAttributes.setString("player", m_text);
		}
		else if (name == "score")
		{
			m_entryAttributes.setString("points", m_text);
		}
		else if (name == "level")
		{
			m_entryAttributes.setString("level", m_text);
		}
		else if (name == "rank")
		{
			m_entryAttributes.setString("rank", m_text);
		}
		else if (name == "highlight")
		{
			m_highlight = toInt(m_text);
		}
	}

	// 0x494B50
	HighScoreListHandler::HighScoreListHandler(HighScoreTable* table)
		: m_table(table)
	{
	}
}

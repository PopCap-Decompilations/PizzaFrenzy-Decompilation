#include "HighScoreEntry.h"

#include <string>

#include "Properties.h"
#include "StringUtil.h"

namespace engine
{
	// the attribute name at 0x521320 (dynamic initialiser 0x4F8970, destroyed by the atexit thunk 0x4F8C50)
	std::string HighScoreEntry::s_rankKey = "rank";

	// 0x479670
	void HighScoreEntry::setRank(int rank)
	{
		m_rank = rank;
	}

	// 0x479680
	bool HighScoreEntry::hasRank() const
	{
		return m_rank > 0;
	}

	// 0x479690
	void HighScoreEntry::save(Properties& attributes) const
	{
		attributes.setInt(s_rankKey, m_rank);
	}

	// 0x4796B0
	void HighScoreEntry::load(const Properties& attributes)
	{
		m_rank = attributes.getInt(s_rankKey, m_rank);
	}

	// 0x4796D0: a blank row
	void HighScoreEntry::saveEmpty(Properties& attributes) const
	{
		attributes.setInt(s_rankKey, 0);
	}

	// 0x4796F0
	HighScoreEntry::HighScoreEntry()
		: m_rank(0)
	{
	}

	// 0x479740: the rank as the table shows it
	void HighScoreEntry::formatDisplay(Properties& attributes) const
	{
		std::string text;
		format(text, "%d. ", m_rank);
		attributes.setString(s_rankKey, text);
	}

	// 0x411960 (folded): one body with every getter of an int at +0x0C
	int HighScoreEntry::getRank() const
	{
		return m_rank;
	}
}

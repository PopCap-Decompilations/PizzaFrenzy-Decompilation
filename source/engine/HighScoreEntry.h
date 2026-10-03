// engine::HighScoreEntry: an abstract row of a high-score table, with its rank.
#pragma once

#include <string>

#include "Object.h"

namespace engine
{
	class Properties;

	// The game's HighScoreEntry (0x4FA838) adds the player, points and level. Object +0x00, m_rank +0x0C, then the
	// vtordisp (+0x10) and the Interface subobject (+0x14): 0x18 bytes. Implicit destructor (slot 0 is the folded
	// empty-destructor 0x44A0A0).
	class HighScoreEntry : public Object
	{
	public:
		HighScoreEntry();

		virtual int getScore() const = 0;										// slot 1: sort key
		virtual void save(Properties& attributes) const;						// slot 2: rank="%d"
		virtual void load(const Properties& attributes);						// slot 3
		virtual void formatDisplay(Properties& attributes) const;				// slot 4: rank = "%d. "
		virtual void saveEmpty(Properties& attributes) const;					// slot 5: rank = 0 (blank row; this unused)

		void setRank(int rank);
		int getRank() const;					// folded 0x411960 (return the int at +0x0C), called by HighScoreTable
		bool hasRank() const;					// rank > 0

		int m_rank;								// +0x0C 1-based; 0 = none

		static std::string s_rankKey;			// 0x521320: "rank"
	};
}

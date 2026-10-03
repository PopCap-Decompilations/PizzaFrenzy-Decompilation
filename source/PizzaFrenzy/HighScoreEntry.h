// HighScoreEntry: one row of the game's high-score tables (player, points, level).
#pragma once

#include <string>

#include "engine/HighScoreEntry.h"

namespace engine
{
	class Properties;
}

// Created empty by the score tables' factory or filled for a new score; the engine base keeps the rank. Implicit
// destructor (0x4186E0).
class HighScoreEntry : public engine::HighScoreEntry
{
public:
	HighScoreEntry();
	HighScoreEntry(const std::string& player, int points, int level);

	// overrides
	virtual void save(engine::Properties& attributes) const;				// engine::HighScoreEntry slot 2
	virtual void load(const engine::Properties& attributes);				// engine::HighScoreEntry slot 3
	virtual void formatDisplay(engine::Properties& attributes) const;		// engine::HighScoreEntry slot 4
	virtual void saveEmpty(engine::Properties& attributes) const;			// engine::HighScoreEntry slot 5

	// 0x4BE760 (folded): engine::HighScoreEntry slot 1; one body with the getters of an int at +0x2C
	virtual int getScore() const
	{
		return m_points;
	}

	std::string m_player;								// +0x10 "player"
	int m_points;										// +0x2C "points" (the sort key)
	int m_level;										// +0x30 "level"
};

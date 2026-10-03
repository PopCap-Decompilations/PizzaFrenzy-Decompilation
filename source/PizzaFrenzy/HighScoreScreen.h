// HighScoreScreen: the high score screen, its score tables (HighScoreTable) and their sort order (ScoreGreater).
#pragma once

#include <string>
#include <vector>

#include "engine/HighScoreEntry.h"
#include "engine/HighScoreTable.h"
#include "engine/RefPtr.h"
#include "engine/ScoreTableScreen.h"

// ScoreTableScreen's RefPtr members need these complete where the implicit constructor and destructor are generated
// (in every file that creates a HighScoreScreen)
#include "engine/ScoreTableModel.h"
#include "engine/Table.h"

// Descending score order of the table entries: HighScoreTable::findPosition hands it to std::upper_bound, which
// inlines it (std::_Upper_bound<..., ScoreGreater, int>, 0x437150).
struct ScoreGreater
{
	bool operator()(engine::HighScoreEntry* a, engine::HighScoreEntry* b) const
	{
		return a->getScore() > b->getScore();
	}
};

// The game's score table: a HighScoreEntry factory and the descending-score insert position. Six are created by
// HighScoreScreen::init with an inlined constructor; adds no members (size 0x8C, as engine::HighScoreTable); its
// destructor is implicit (0x436D50).
class HighScoreTable : public engine::HighScoreTable
{
public:
	HighScoreTable(const std::string& name, int maxEntries)
		: engine::HighScoreTable(name, maxEntries)
	{
	}

	virtual engine::HighScoreEntry* createEntry();					// slot 1 (engine::HighScoreTable)
	virtual iterator findPosition(engine::HighScoreEntry* entry);	// slot 2 (engine::HighScoreTable)
};

// res/screenLayouts/highScores.xml (game+0xA0): personal and global score tables per game mode, mode arrows, global
// toggle, reset and post. Its constructor (0x403480) and destructor (0x4035C0) are compiler-generated: both were
// emitted in PizzaFrenzy.cpp, and the destructor does not reset the vtables as the user-declared ones do. The members
// end at +0x20C; the vtordisp (+0x20C) and the Interface subobject (+0x210) follow them (size 0x214).
class HighScoreScreen : public engine::ScoreTableScreen
{
public:
	virtual void init();											// slot 96
	virtual void onButtonClicked(const std::string& name);			// slot 97
	virtual void showCurrentTable();								// slot 98
	virtual void setShowGlobal(bool global);						// slot 99
	virtual bool submitScore(bool online);							// slot 100
	virtual void clearScores();										// slot 101
	virtual void loadScores();										// slot 102

	virtual void setScores(engine::HighScoreTable* scores);			// slot 95 (engine::ScoreTableScreen)

	void addDefaultScores(HighScoreTable* table);

	bool m_showGlobal;													// +0x1EC the online scores are shown (not set by the ctor)
	engine::RefPtr<HighScoreTable> m_localTables[3];					// +0x1F0 "speedMode", "memoryMode", "concentrationMode"
																		//        (10 entries)
	engine::RefPtr<HighScoreTable> m_globalTables[3];					// +0x1FC online "S", "M", "C" (201 entries)
	int m_mode;															// +0x208 0 speed, 1 memory, 2 concentration (not set by the
																		//        ctor)
};

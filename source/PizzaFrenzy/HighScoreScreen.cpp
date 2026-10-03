// HighScoreScreen: the high score screen, its score tables (HighScoreTable) and their sort order (ScoreGreater).
#include <algorithm>
#include <string>

#include "HighScoreScreen.h"
#include "engine/Application.h"
#include "engine/Color.h"
#include "engine/Component.h"
#include "engine/HighScoreManager.h"
#include "engine/TextItem.h"
#include "engine/User.h"
#include "engine/UserManager.h"
#include "GameProgress.h"
#include "HighScoreEntry.h"
#include "PizzaFrenzy.h"

// 0x436CA0
engine::HighScoreEntry* HighScoreTable::createEntry()
{
	return new HighScoreEntry();
}

// 0x436DB0
bool HighScoreScreen::submitScore(bool online)
{
	int mode = getGame()->m_mode;
	GameProgress* stats = PizzaFrenzy::getGameStats();
	engine::User* user = engine::UserManager::getInstance()->getCurrentUser();
	if (stats->m_score <= 0)
		return false;
	engine::RefPtr<HighScoreEntry> entry = new HighScoreEntry(user->getName(), stats->m_score, getGame()->m_levelNumber);
	if (online)
	{
		engine::HighScoreManager::getInstance()->submitGlobalScore(m_globalTables[mode], entry);
		m_mode = mode;
		setShowGlobal(true);
		return true;
	}
	if (m_localTables[mode]->addEntry(entry))
	{
		engine::HighScoreManager::getInstance()->saveScores(m_localTables[mode]);
		m_mode = mode;
		setShowGlobal(false);
		return true;
	}
	return false;
}

// 0x436F70
void HighScoreScreen::showCurrentTable()
{
	HighScoreTable* table;
	if (m_showGlobal)
	{
		table = m_globalTables[m_mode];
		if (table->isEmpty())
			engine::HighScoreManager::getInstance()->fetchGlobalScores(table);
	}
	else
	{
		table = m_localTables[m_mode];
	}
	setScores(table);
}

// 0x436FD0
void HighScoreScreen::clearScores()
{
	m_localTables[m_mode]->clear();
	engine::HighScoreManager::getInstance()->saveScores(m_localTables[m_mode]);
	showCurrentTable();
}

// 0x437050
void HighScoreScreen::onButtonClicked(const std::string& name)
{
	if (name == "toggleScoreModeLeft")
	{
		if (--m_mode < 0)
			m_mode = 2;
		showCurrentTable();
	}
	else if (name == "toggleScoreModeRight")
	{
		if (++m_mode == 3)
			m_mode = 0;
		showCurrentTable();
	}
	else if (name == "toggleGlobalHighScore")
	{
		setShowGlobal(!m_showGlobal);
	}
	else
	{
		getGame()->onScreenEvent(name);
	}
}

// 0x437120
HighScoreTable::iterator HighScoreTable::findPosition(engine::HighScoreEntry* entry)
{
	return std::upper_bound(m_entries.begin(), m_entries.end(), entry, ScoreGreater());
}

// 0x4371B0
void HighScoreScreen::setScores(engine::HighScoreTable* scores)
{
	engine::ScoreTableScreen::setScores(scores);
	engine::TextItem* modeText = (engine::TextItem*)getComponent("highScoreModeText");
	modeText->setText(scores->getTitle());
}

// 0x437260: fills an empty table with the default names, then saves it
void HighScoreScreen::addDefaultScores(HighScoreTable* table)
{
	engine::HighScoreManager* manager = engine::HighScoreManager::getInstance();
	if (table->isEmpty())
	{
		table->addEntry(new HighScoreEntry("Lorenzo", 40000, 9));
		table->addEntry(new HighScoreEntry("Paula", 35000, 8));
		table->addEntry(new HighScoreEntry("Lisa", 30000, 7));
		table->addEntry(new HighScoreEntry("Niccolo", 25000, 6));
		table->addEntry(new HighScoreEntry("James", 20000, 5));
		table->addEntry(new HighScoreEntry("Matt", 15000, 4));
		table->addEntry(new HighScoreEntry("Ed", 10000, 3));
		table->addEntry(new HighScoreEntry("Joe", 5000, 2));
	}
	manager->saveScores(table);
}

// 0x437750
void HighScoreScreen::setShowGlobal(bool global)
{
	m_showGlobal = global;
	engine::Component* globalText = getComponent("globalText");
	engine::Component* personalText = getComponent("personalText");
	globalText->setVisible(global);
	personalText->setVisible(!global);
	engine::Component* clearButton = getComponent("ClearScores");
	if (global)
	{
		// the online scores can't be cleared
		clearButton->setColorMode(2);
		clearButton->setColor(engine::Color(0.7f, 0.7f, 0.7f));
		clearButton->setEnabled(false);
	}
	else
	{
		clearButton->setColorMode(0);
		clearButton->setEnabled(true);
	}
	showCurrentTable();
}

// 0x437930
void HighScoreScreen::loadScores()
{
	for (int i = 0; i < 3; ++i)
	{
		engine::HighScoreManager::getInstance()->loadScores(m_localTables[i]);
		addDefaultScores(m_localTables[i]);
	}
}

// 0x437A10
void HighScoreScreen::init()
{
	m_localTables[1] = new HighScoreTable("memoryMode", 10);
	m_localTables[1]->setTitle(engine::getApplication()->loadString(205));
	m_localTables[0] = new HighScoreTable("speedMode", 10);
	m_localTables[0]->setTitle(engine::getApplication()->loadString(206));
	m_localTables[2] = new HighScoreTable("concentrationMode", 10);
	m_localTables[2]->setTitle(engine::getApplication()->loadString(207));
	loadScores();

	m_globalTables[1] = new HighScoreTable("M", 201);
	m_globalTables[1]->setTitle(engine::getApplication()->loadString(205));
	m_globalTables[0] = new HighScoreTable("S", 201);
	m_globalTables[0]->setTitle(engine::getApplication()->loadString(206));
	m_globalTables[2] = new HighScoreTable("C", 201);
	m_globalTables[2]->setTitle(engine::getApplication()->loadString(207));

	m_actionSignal.connect(this, &HighScoreScreen::onButtonClicked);
	m_mode = 0;
	setShowGlobal(false);
}

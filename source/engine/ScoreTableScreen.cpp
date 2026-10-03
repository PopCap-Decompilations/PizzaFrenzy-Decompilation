#include "ScoreTableScreen.h"

#include <string>

#include "Component.h"

namespace engine
{
	// the scroll buttons' commands (0x521244, 0x521260; dynamic initialisers 0x4F8930, 0x4F8950)
	static std::string s_scoreScrollUp = "scoreScrollUp";
	static std::string s_scoreScrollDown = "scoreScrollDown";

	// 0x46B880
	ScoreTableScreen::ScoreTableScreen()
		: m_scrollIndex(0), m_scrollDirection(0), m_scrollDelay(0.0f)
	{
	}

	// 0x46B9A0: moves the table's top row one step in the scroll direction while all its visible rows stay filled
	void ScoreTableScreen::scroll()
	{
		int topRowLimit = 1 - m_scoreTable->getVisibleRows() + m_scoreTable->getRowCount();
		int topRow = m_scoreTable->getTopRow() + m_scrollDirection;
		if (topRow >= 0 && topRow < topRowLimit)
			m_scoreTable->setTopRow(topRow);
	}

	// 0x46B9F0: while a scroll button is held, a step every 0.1 s (the first 0.4 s after the press)
	void ScoreTableScreen::update(UpdateContext& context)
	{
		if (m_scrollDirection != 0)
		{
			m_scrollDelay -= context.elapsed;
			if (m_scrollDelay <= 0.0f)
			{
				scroll();
				m_scrollDelay += 0.1f;
			}
		}
		Screen::update(context);
	}

	// 0x46BA50
	void ScoreTableScreen::onMouseDown(std::string command)
	{
		if (command == s_scoreScrollUp)
		{
			m_scrollDirection = -1;
			scroll();
			m_scrollDelay = 0.4f;
		}
		else if (command == s_scoreScrollDown)
		{
			m_scrollDirection = 1;
			scroll();
			m_scrollDelay = 0.4f;
		}
	}

	// 0x46BB20
	void ScoreTableScreen::onMouseUp(std::string command)
	{
		if (command == s_scoreScrollUp || command == s_scoreScrollDown)
			m_scrollDirection = 0;
	}

	// 0x46BBD0
	void ScoreTableScreen::setScores(HighScoreTable* scores)
	{
		m_scores = scores;
		m_scoreModel = new ScoreTableModel(scores);
		m_scrollIndex = 0;
		m_scoreTable = static_cast<Table*>(getComponent("scoreTable"));
		if (m_scoreTable)
			m_scoreTable->setModel(m_scoreModel);
	}

	// 0x443BB0 (folded)
	void ScoreTableScreen::activate()
	{
		Screen::activate();
	}
}

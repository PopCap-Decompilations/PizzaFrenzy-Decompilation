// engine::ScoreTableScreen: the engine base of the high-score screen (a score table that scrolls while a button is held).
#pragma once

#include <string>

#include "HighScoreTable.h"
#include "RefPtr.h"
#include "ScoreTableModel.h"
#include "Screen.h"
#include "Table.h"

namespace engine
{
	struct UpdateContext;

	// Screen +0x00, members from +0x1D4, then the vtordisp (+0x1EC) and the Interface subobject (+0x1F0): 0x1F4
	// bytes. Gives a score list to the layout's "scoreTable" and scrolls it while "scoreScrollUp" or
	// "scoreScrollDown" is held. The destructor (0x401720, emitted in another object) is implicit: it stores no
	// vtables (so the RefPtr members' classes are included: every file that destroys one instantiates it).
	class ScoreTableScreen : public Screen
	{
	public:
		ScoreTableScreen();

		virtual void activate();						// slot 34 (engine::Component): Screen::activate() (body folded: 0x443BB0)
		virtual void update(UpdateContext& context);	// slot 37 (engine::Component)
		virtual void onMouseDown(std::string command);	// slot 2 (engine::ButtonListener): starts scrolling
		virtual void onMouseUp(std::string command);	// slot 3 (engine::ButtonListener): stops scrolling

		virtual void setScores(HighScoreTable* scores);	// slot 95

		void scroll();

		int m_scrollIndex;						// +0x1D4 reset to 0 by setScores, never read
		int m_scrollDirection;					// +0x1D8 -1 while scoreScrollUp is held, +1 for scoreScrollDown, else 0
		float m_scrollDelay;					// +0x1DC seconds to the next scroll step (0.4 after the press, then 0.1)
		RefPtr<HighScoreTable> m_scores;		// +0x1E0 the score list
		RefPtr<ScoreTableModel> m_scoreModel;	// +0x1E4 table model over m_scores
		RefPtr<Table> m_scoreTable;				// +0x1E8 the "scoreTable" component
	};
}

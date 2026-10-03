#include "ScoreTableModel.h"

#include "HighScoreEntry.h"
#include "HighScoreTable.h"
#include "StringUtil.h"

namespace engine
{
	// 0x490500
	int ScoreTableModel::getRowCount()
	{
		return m_scores->getCount();
	}

	// 0x490510
	bool ScoreTableModel::isReady()
	{
		return m_scores->getStatus() == 2;
	}

	// 0x490530
	int ScoreTableModel::getSelectedRow()
	{
		return m_scores->getNewEntryIndex();
	}

	// 0x490540
	std::string ScoreTableModel::getStatusText()
	{
		switch (m_scores->getStatus())
		{
		case 3:
			return "ERROR";
		case 1:
			return "LOADING";
		case 2:
			return "READY";
		case 0:
			return "CREATED";
		default:
			return ">_<";
		}
	}

	// 0x490600
	void ScoreTableModel::loadRow(int row)
	{
		if (row == m_cachedRow)
			return;
		HighScoreEntry* entry = m_scores->getEntry(row);
		m_rowData.clear();
		entry->formatDisplay(m_rowData);
		std::string rank;
		format(rank, "%d. ", entry->getRank());
		m_rowData.setString("rank", rank);
		m_cachedRow = row;
	}

	// 0x490710
	std::string ScoreTableModel::getText(int row, const Table::Column& column)
	{
		loadRow(row);
		std::string text = m_rowData.getString(column.name, "O_o");
		return text;
	}

	// 0x490800
	TableModel::TableModel()
	{
	}

	// 0x490910
	ScoreTableModel::ScoreTableModel(HighScoreTable* scores)
		: m_scores(scores), m_cachedRow(-1)
	{
	}
}

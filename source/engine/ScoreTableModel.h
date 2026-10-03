// engine::TableModel (the data source of a Table) and engine::ScoreTableModel (a TableModel over a high-score table).
#pragma once

#include <string>
#include <vector>

#include "Object.h"
#include "Properties.h"
#include "RefPtr.h"
#include "Table.h"
#include "sigslot.h"

namespace engine
{
	class HighScoreTable;

	// Row count, cell text by column, ready flag and status text, selected row. Its constructor is emitted in
	// ScoreTableModel.cpp; implicit destructor (0x4908A0). Layout: Object +0x00, m_changed +0x0C, then the vtordisp
	// and Interface (0x24 bytes).
	class TableModel : public Object
	{
	public:
		TableModel();

		// slot 1
		virtual int getRowCount() = 0;
		// slot 2: the column's name is the key
		virtual std::string getText(int row, const Table::Column& column) = 0;
		// slot 3: when false the Table draws getStatusText() instead of the rows
		virtual bool isReady() = 0;
		// slot 4
		virtual std::string getStatusText() = 0;
		// slot 5: the Table scrolls it into view
		virtual int getSelectedRow() = 0;

		sigslot::signal0<> m_changed;			// +0x0C Table::setModel connects to it
	};

	// Set on the layout's "scoreTable" by ScoreTableScreen; implicit destructor (0x490A00). Layout: TableModel
	// +0x00, members +0x1C, then the vtordisp and Interface (0x5C bytes).
	class ScoreTableModel : public TableModel
	{
	public:
		ScoreTableModel(HighScoreTable* scores);

		// fills m_rowData with the entry's properties and "rank" unless row is the cached one
		void loadRow(int row);

		// TableModel
		virtual int getRowCount();
		virtual std::string getText(int row, const Table::Column& column);
		virtual bool isReady();
		virtual std::string getStatusText();
		virtual int getSelectedRow();

		RefPtr<HighScoreTable> m_scores;		// +0x1C the high-score list
		std::vector<std::string> m_columnNames;	// +0x20 guess: destroyed by the destructor, never used
		Properties m_rowData;					// +0x30 string properties of the cached row (entry properties + "rank")
		int m_cachedRow;						// +0x50 row held in m_rowData, -1 = none
	};
}

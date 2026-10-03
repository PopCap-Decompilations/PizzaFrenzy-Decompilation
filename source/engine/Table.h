// engine::Table: text table component showing the rows of a TableModel in columns, with the selected row highlighted.
#pragma once

#include <string>
#include <vector>

#include "Color.h"
#include "Component.h"
#include "RefPtr.h"
#include "sigslot.h"

namespace engine
{
	class Graphics;
	class TableModel;

	// Built by the layout elements <table> and <column>; keeps the inherited type name "Component". Layout:
	// Component +0x00, has_slots<> +0x108, members +0x118, then the vtordisp and Interface (0x180 bytes).
	class Table : public Component, public sigslot::has_slots<>
	{
	public:
		// one column (0x28 bytes); the constructor (inlined in vector::resize) leaves x to updateBounds
		struct Column
		{
			Column()
				: width(0), align(0)
			{
			}

			std::string name;					// +0x00 key of the cell in the model's row data
			int width;							// +0x1C
			int align;							// +0x20 0 left, 1 center, 2 right
			float x;							// +0x24 text anchor computed by updateBounds
		};

		// m_selectedRow is left uninitialised; implicit destructor (0x490390)
		Table();

		// Component
		virtual void draw(Graphics& g);
		virtual void update(UpdateContext& context);
		virtual void updateBounds();

		// disconnects from the old model's m_changed and connects onModelChanged to the new one's
		void setModel(TableModel* model);
		// the slot connected to the model's m_changed: does nothing (body folded with the empty functions)
		void onModelChanged();
		// m_model->getRowCount() (no null check)
		int getRowCount() const;

		// appends a default column; returns its index
		int addColumn();
		void setColumnName(int column, const std::string& name);
		void setColumnWidth(int column, int width);
		void setColumnAlign(int column, int align);

		void setVisibleRows(int rows);
		// body folded with SimpleSoundDX::getSoundCount (both read +0x12C)
		int getVisibleRows() const;
		void setTopRow(int row);
		int getTopRow() const;

		void setBackgroundColor(const Color& color);
		const Color& getBackgroundColor() const;
		void setTextColor(const Color& color);
		const Color& getTextColor() const;
		void setSelectionBackgroundColor(const Color& color);
		void setSelectionTextColor(const Color& color);

		RefPtr<TableModel> m_model;				// +0x118 data source
		std::vector<Column> m_columns;			// +0x11C
		int m_visibleRows;						// +0x12C rows attribute (the parser's default is 10)
		int m_topRow;							// +0x130 first visible row (scroll position)
		int m_selectedRow;						// +0x134 last selection seen by update()
		Color m_backgroundColor;				// +0x138 box colour
		Color m_textColor;						// +0x148 row and status text colour
		Color m_selectionBackgroundColor;		// +0x158 selected-row bar colour
		Color m_selectionTextColor;				// +0x168 selected-row text colour
	};
}

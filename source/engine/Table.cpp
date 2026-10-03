#include "Table.h"

#include <algorithm>
#include <string>

#include "Font.h"
#include "Graphics.h"
#include "Rect.h"
#include "ScoreTableModel.h"
#include "StringUtil.h"

namespace engine
{
	// 0x48F010
	void Table::setVisibleRows(int rows)
	{
		m_visibleRows = rows;
		addTreeFlags(8);
	}

	// 0x48DAB0 (folded)
	int Table::getVisibleRows() const
	{
		return m_visibleRows;
	}

	// 0x48F030
	void Table::setTopRow(int row)
	{
		m_topRow = row;
	}

	// 0x48F040
	int Table::getTopRow() const
	{
		return m_topRow;
	}

	// 0x48F050
	void Table::setBackgroundColor(const Color& color)
	{
		m_backgroundColor = color;
	}

	// 0x48F080
	const Color& Table::getBackgroundColor() const
	{
		return m_backgroundColor;
	}

	// 0x48F090
	void Table::setTextColor(const Color& color)
	{
		m_textColor = color;
	}

	// 0x48F0C0
	const Color& Table::getTextColor() const
	{
		return m_textColor;
	}

	// 0x48F0D0
	void Table::setSelectionBackgroundColor(const Color& color)
	{
		m_selectionBackgroundColor = color;
	}

	// 0x48F100
	void Table::setSelectionTextColor(const Color& color)
	{
		m_selectionTextColor = color;
	}

	// 0x48F1C0
	int Table::getRowCount() const
	{
		return m_model->getRowCount();
	}

	// 0x48F1D0
	void Table::update(UpdateContext& context)
	{
		if (m_model && m_model->isReady())
		{
			int selected = m_model->getSelectedRow();
			if (selected != m_selectedRow || m_topRow >= m_model->getRowCount())
			{
				m_selectedRow = selected;
				m_topRow = selected < m_visibleRows ? 0 : selected - m_visibleRows + 1;
			}
		}
	}

	// 0x48F280
	void Table::setColumnWidth(int column, int width)
	{
		if (column >= 0 && column < (int)m_columns.size())
		{
			m_columns[column].width = width;
			addTreeFlags(8);
		}
	}

	// 0x48F2E0
	void Table::setColumnAlign(int column, int align)
	{
		if (column >= 0 && column < (int)m_columns.size())
			m_columns[column].align = align;
	}

	// 0x48F330
	void Table::updateBounds()
	{
		float x = 0.0f;
		for (int i = 0; i < (int)m_columns.size(); ++i)
		{
			Column& column = m_columns[i];
			switch (column.align)
			{
			case 0:
				column.x = x;
				break;
			case 1:
				column.x = x + column.width * 0.5f;
				break;
			case 2:
				column.x = x + column.width;
				break;
			}
			x += column.width;
		}
		m_bounds.left = m_position.x;
		m_bounds.right = m_position.x + x;
		float rowHeight = m_font ? (float)m_font->getHeight() : 10.0f;
		m_bounds.top = m_position.y;
		m_bounds.bottom = m_position.y + m_visibleRows * rowHeight;
		removeTreeFlags(8);
	}

	// 0x48F410
	void Table::draw(Graphics& g)
	{
		g.setColor(m_backgroundColor);
		g.pushState();
		g.fillRect(m_bounds.getWidth(), m_bounds.getHeight());
		g.popState();

		Font* font = g.getFont();
		float rowHeight = font ? (float)font->getHeight() : 0.0f;
		float y = 0.0f;
		if (m_model)
		{
			if (m_model->isReady())
			{
				int first = std::min(m_topRow, m_model->getRowCount());
				int last = std::min(first + m_visibleRows, m_model->getRowCount());
				for (int row = first; row < last; ++row)
				{
					if (row == m_selectedRow)
					{
						IntRect bar;
						bar.left = 0;
						bar.right = (int)m_bounds.getWidth();
						bar.top = (int)y;
						bar.bottom = (int)(y + rowHeight);
						g.setColor(m_selectionBackgroundColor);
						g.setColorMode(0);
						g.fillRect(bar);
						g.setColor(m_selectionTextColor);
					}
					else
					{
						g.setColor(m_textColor);
					}
					g.setColorMode(2);
					for (int col = 0; col < (int)m_columns.size(); ++col)
					{
						Column& column = m_columns[col];
						std::string text = m_model->getText(row, column);
						g.setHAlign(column.align);
						g.drawString(text.c_str(), column.x, y);
					}
					y += rowHeight;
				}
			}
			else
			{
				std::string status = m_model->getStatusText();
				g.setColor(m_textColor);
				g.setColorMode(2);
				g.setHAlign(1);
				g.setVAlign(1);
				g.drawString(status.c_str(), m_bounds.getWidth() * 0.5f, m_bounds.getHeight() * 0.5f);
			}
		}
		else
		{
			// no model: placeholder cells
			for (int row = 0; row < m_visibleRows; ++row)
			{
				for (int col = 0; col < (int)m_columns.size(); ++col)
				{
					std::string text;
					Column& column = m_columns[col];
					g.setHAlign(column.align);
					format(text, "Data(%d,%d)", row, col);
					g.drawString(text.c_str(), column.x, y);
				}
				y += rowHeight;
			}
		}
	}

	// 0x48F940
	void Table::setColumnName(int column, const std::string& name)
	{
		if (column >= 0 && column < (int)m_columns.size())
			m_columns[column].name = name;
	}

	// 0x4D0470 (folded)
	void Table::onModelChanged()
	{
	}

	// 0x48FD90
	void Table::setModel(TableModel* model)
	{
		if (m_model)
			m_model->m_changed.disconnect(this);
		m_model = model;
		if (m_model)
			m_model->m_changed.connect(this, &Table::onModelChanged);
	}

	// 0x490150
	Table::Table()
		: m_visibleRows(0), m_topRow(0)
	{
		setFlags(5);
	}

	// 0x490490
	int Table::addColumn()
	{
		int index = (int)m_columns.size();
		m_columns.resize(m_columns.size() + 1);
		addTreeFlags(8);
		return index;
	}
}

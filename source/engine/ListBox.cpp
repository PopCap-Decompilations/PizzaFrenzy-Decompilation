#include "ListBox.h"

#include "Application.h"
#include "Font.h"
#include "Graphics.h"
#include "Rect.h"
#include "SoundHandle.h"
#include "User.h"
#include "UserManager.h"

namespace engine
{
	// 0x491580
	void ListBox::onModelChanged()
	{
		addTreeFlags(8);
	}

	// 0x491590
	void ListBox::setSize(float width, float height)
	{
		m_size.set(width, height);
	}

	// 0x4915A0
	void ListBox::setTextColor(const Color& color)
	{
		m_textColor = color;
	}

	// 0x4915D0
	const Color& ListBox::getTextColor() const
	{
		return m_textColor;
	}

	// 0x4915E0
	void ListBox::setBackgroundColor(const Color& color)
	{
		m_backgroundColor = color;
	}

	// 0x491610
	const Color& ListBox::getBackgroundColor() const
	{
		return m_backgroundColor;
	}

	// 0x491620
	void ListBox::setHighlightColor(const Color& color)
	{
		m_highlightColor = color;
	}

	// 0x491650
	const Color& ListBox::getHighlightColor() const
	{
		return m_highlightColor;
	}

	// 0x491660
	void ListBox::setSelectionBackgroundColor(const Color& color)
	{
		m_selectionBackgroundColor = color;
	}

	// 0x491690
	const Color& ListBox::getSelectionBackgroundColor() const
	{
		return m_selectionBackgroundColor;
	}

	// 0x4916A0
	void ListBox::setSelectionTextColor(const Color& color)
	{
		m_selectionTextColor = color;
	}

	// 0x4916D0
	void ListBox::setSelectionHighlightColor(const Color& color)
	{
		m_selectionHighlightColor = color;
	}

	// 0x491700
	void ListBox::setIndent(int indent)
	{
		m_indent = indent;
	}

	// 0x491710
	int ListBox::getSelectedIndex() const
	{
		return m_selectedIndex;
	}

	// 0x471C80 (folded)
	void ListBox::setSelectedIndex(int index)
	{
		m_selectedIndex = index;
	}

	// 0x491720
	void ListBox::updateBounds()
	{
		m_bounds.left = m_position.x;
		m_bounds.right = m_position.x + m_size.x;
		m_bounds.top = m_position.y;
		m_bounds.bottom = m_position.y + m_size.y;
		removeTreeFlags(8);
	}

	// 0x491750
	void ListBox::mouseLeave(const MouseEvent& event)
	{
		m_hoverIndex = -1;
	}

	// 0x491760
	void ListBox::setSounds(const std::string& overSound, const std::string& activeSound)
	{
		Application* app = getApplication();
		if (!overSound.empty())
		{
			app->loadSound(overSound, overSound);
			m_overSound = app->createSound(overSound, true, 0);
		}
		if (!activeSound.empty())
		{
			app->loadSound(activeSound, activeSound);
			m_activeSound = app->createSound(activeSound, true, 0);
		}
	}

	// 0x491820
	int ListBox::getItemCount() const
	{
		return m_model->getCount();
	}

	// 0x491830
	int ListBox::getVisibleRowCount() const
	{
		float rowHeight = m_font ? (float)m_font->getHeight() : 1.0f;
		return (int)(m_size.y / rowHeight);
	}

	// 0x491880
	int ListBox::getItemAt(const Vector2& point) const
	{
		int index = -1;
		Vector2 local(point);
		local.offset(-m_position.x, -m_position.y);
		if (local.y >= 0.0f && local.y < m_bounds.getHeight())
		{
			float rowHeight = m_font ? (float)m_font->getHeight() : 1.0f;
			index = (int)(local.y / rowHeight);
		}
		if (index >= m_model->getCount())
			return -1;
		return index;
	}

	// 0x491930
	void ListBox::mouseMove(const MouseEvent& event)
	{
		int previous = m_hoverIndex;
		m_hoverIndex = getItemAt(event.position);
		if (m_hoverIndex != previous && m_hoverIndex >= 0 && m_overSound)
		{
			if (m_overSound->isPlaying())
				m_overSound->stop();
			m_overSound->play();
		}
	}

	// 0x4919D0
	void ListBox::draw(Graphics& g)
	{
		g.setColor(m_backgroundColor);
		g.fillRect(m_bounds.getWidth(), m_bounds.getHeight());
		Font* font = g.getFont();
		float rowHeight = font ? (float)font->getHeight() : 0.0f;
		float y = 0.0f;
		int count = m_model ? m_model->getCount() : 0;
		for (int i = 0; i < count; i++)
		{
			if (i == m_selectedIndex)
			{
				IntRect row;
				row.left = 0;
				row.right = (int)m_bounds.getWidth();
				row.top = (int)y;
				row.bottom = (int)(y + rowHeight);
				g.setColor(m_selectionBackgroundColor);
				g.setColorMode(0);
				g.fillRect(row);
				if (i == m_hoverIndex)
					g.setColor(m_selectionHighlightColor);
				else
					g.setColor(m_selectionTextColor);
			}
			else if (i == m_hoverIndex)
			{
				g.setColor(m_highlightColor);
			}
			else
			{
				g.setColor(m_textColor);
			}
			std::string text = m_model->getItem(i);
			g.setColorMode(2);
			g.setHAlign(0);
			g.drawString(text.c_str(), (float)m_indent, y);
			y += rowHeight;
		}
	}

	// 0x491C00
	void ListBox::mouseDown(const MouseEvent& event)
	{
		int index = getItemAt(event.position);
		if (index != m_selectedIndex)
		{
			m_selectedIndex = index;
			m_selectionChanged.emit(index);
			if (m_selectedIndex >= 0 && m_activeSound)
			{
				if (m_activeSound->isPlaying())
					m_activeSound->stop();
				m_activeSound->play();
			}
		}
	}

	// 0x491C80
	ListBox::~ListBox()
	{
		if (m_overSound)
			m_overSound->destroy();
		if (m_activeSound)
			m_activeSound->destroy();
	}

	// 0x491EA0
	ListBox::ListBox()
		: m_backgroundColor(0, 0, 0), m_textColor(1, 1, 1), m_highlightColor(1, 1, 0),
		m_selectionBackgroundColor(1, 1, 1), m_selectionTextColor(0, 0, 0), m_selectionHighlightColor(0, 0, 1),
		m_indent(0), m_hoverIndex(-1), m_selectedIndex(-1), m_overSound(NULL), m_activeSound(NULL)
	{
		setFlags(3);
	}

	// 0x492070
	void ListBox::setModel(ListModel* model)
	{
		m_model = model;
		m_model->m_changed.connect(this, &ListBox::onModelChanged);
		addTreeFlags(8);
	}

	// 0x4920F0
	int UserListModel::getCount()
	{
		return m_users->getUserCount();
	}

	// 0x492100
	std::string UserListModel::getItem(int index)
	{
		User* user = m_users->getUser(index);
		if (user)
			return user->getName();
		return ">_<";
	}

	// 0x492170
	ListModel::ListModel()
	{
	}

	// 0x492210
	UserListModel::UserListModel()
	{
		m_users = UserManager::getInstance();
	}
}

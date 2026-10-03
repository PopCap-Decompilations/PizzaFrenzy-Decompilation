#include "Selector.h"

#include <cmath>

#include "Graphics.h"

namespace engine
{
	// 0x47B6C0 (folded)
	int Selector::getSelection() const
	{
		return m_selected;
	}

	// 0x47B6C0 (folded)
	int Selector::getSelectedIndex() const
	{
		return m_selected;
	}

	// 0x47B6D0 (folded)
	int Selector::getCount() const
	{
		return m_children.size();
	}

	// 0x47B6F0
	void Selector::updateBounds()
	{
		m_bounds.clear();
		Component* child = getSelectedChild();
		if (child != 0)
		{
			if (child->testTreeFlags(8))
				child->updateBounds();
			m_bounds.unite(child->getBounds());
		}
		else
		{
			Container::updateBounds();
		}
		m_bounds.scale(fabs(m_scale.x), fabs(m_scale.y));
		m_bounds.offset(m_position.x, m_position.y);
		removeTreeFlags(8);
	}

	// 0x47B780
	void Selector::draw(Graphics& g)
	{
		if (!isVisible())
			return;
		if (m_selected >= (int)m_children.size())
			m_selected = (int)m_children.size() - 1;
		if (m_selected >= 0)
		{
			g.pushState();
			Component* child = m_children.at(m_selected);
			if (child != 0)
			{
				child->setupGraphics(g);
				child->draw(g);
			}
			g.popState();
		}
		if (m_next >= 0)
		{
			g.pushState();
			Component* child = m_children.at(m_next);
			if (child != 0)
			{
				child->setupGraphics(g);
				child->draw(g);
			}
			g.popState();
		}
	}

	// 0x47B880
	void Selector::select(int index, float fadeTime, bool fadeInNext)
	{
		if (fadeTime != 0.0f)
		{
			m_fadeTime = fadeTime;
			m_fadeTimer = fadeTime;
			m_fadeInNext = fadeInNext;
			if (m_next >= 0)
				m_selected = m_next;
			m_next = index;
		}
		else
		{
			if (m_next >= 0)
			{
				m_children.at(m_next)->setAlpha(1.0f);
				m_next = -1;
			}
			m_selected = index;
			m_fadeTime = 0.0f;
			m_fadeTimer = 0.0f;
			if (m_selected >= 0)
				m_children.at(m_selected)->setAlpha(1.0f);
			addTreeFlags(8);
		}
	}

	// 0x47B980
	Component* Selector::getSelectedChild() const
	{
		if (m_selected < 0 || (unsigned int)m_selected >= m_children.size())
			return 0;
		return m_children.at(m_selected);
	}

	// 0x47B9D0
	void Selector::update(UpdateContext& context)
	{
		Container::update(context);
		if (m_fadeTime != 0.0f)
		{
			m_fadeTimer -= context.elapsed;
			if (m_fadeTimer <= 0.0f)
			{
				m_fadeTimer = 0.0f;
				m_fadeTime = 0.0f;
				m_children.at(m_selected)->setAlpha(1.0f);
				m_selected = m_next;
				m_next = -1;
				m_children.at(m_selected)->setAlpha(1.0f);
				addTreeFlags(8);
			}
			else
			{
				float fade = m_fadeTimer / m_fadeTime;
				if (m_fadeInNext)
					m_children.at(m_next)->setAlpha(1.0f - fade);
				else
					m_children.at(m_selected)->setAlpha(fade);
			}
		}
	}

	// 0x47BB40
	Selector::Selector()
		: m_selected(-1), m_next(-1), m_fadeTimer(0.0f), m_fadeTime(0.0f)
	{
		setFlags(5);
	}

	// 0x47BC10
	std::string Selector::getTypeName() const
	{
		return "Selector";
	}
}

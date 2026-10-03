#include "TextTyper.h"

#include <algorithm>

#include "Font.h"
#include "Graphics.h"

namespace engine
{
	// 0x482290
	void TextTyper::setText(const std::string& text)
	{
		TextItem::setText(text);
		reset();
	}

	// 0x4822B0
	void TextTyper::setup(float wrapWidth, float charsPerSecond, float punctuationDelay)
	{
		m_wordWrap = (int)wrapWidth;
		m_charsPerSecond = charsPerSecond;
		m_punctuationDelay = punctuationDelay;
	}

	// 0x4822E0
	void TextTyper::setPaging(int linesPerPage, float pageDelay)
	{
		m_linesPerPage = linesPerPage;
		m_pageDelay = pageDelay;
	}

	// 0x482300
	void TextTyper::reset()
	{
		m_column = 0;
		m_line = 0;
		maskFlags(4);
	}

	// 0x482320
	void TextTyper::pause()
	{
		maskFlags(4);
	}

	// 0x482330
	TextTyper::TextTyper()
	{
		setFlags(4);
		m_column = 0;
		m_line = 0;
		m_linesPerPage = 0;
		m_pageDelay = 0.0f;
	}

	// 0x482430
	bool TextTyper::isFinished() const
	{
		return m_line >= (int)m_lines.size();
	}

	// 0x482480
	void TextTyper::start()
	{
		if (m_line < (int)m_lines.size() && m_column < (int)m_lines[m_line].size())
		{
			setFlags(4);
			unmaskFlags(4);
			m_timer = 0.0f;
		}
	}

	// 0x4824F0
	void TextTyper::finish()
	{
		m_line = m_lines.size() - 1;
		m_column = m_lines[m_line].size() - 1;
	}

	// 0x482540
	void TextTyper::update(UpdateContext& context)
	{
		int fromLine = m_line;
		int fromColumn = m_column + 1;
		m_timer -= context.elapsed;
		if (m_timer < 0.0f && !m_lines.empty())
		{
			float charTime = 1.0f / m_charsPerSecond;
			int count = (int)(context.elapsed / charTime);
			if (count < 1)
				count = 1;
			int column = m_column + count;
			if (column >= (int)m_lines[fromLine].size() && m_linesPerPage > 0 && m_column != (int)m_lines[fromLine].size())
			{
				// the end of a line with paging: pause (at a '.' on the way first)
				if (findPunctuation(fromLine, fromColumn, fromLine, column))
				{
					m_timer = m_punctuationDelay;
					return;
				}
				m_column = m_lines[m_line].size();
				m_timer = m_pageDelay;
				return;
			}
			m_column = column;
			while (m_column >= (int)m_lines[m_line].size() && m_line + 1 < (int)m_lines.size())
			{
				m_column -= m_lines[m_line].size();
				m_line++;
				if (m_linesPerPage > 0)
					break;
			}
			if (findPunctuation(fromLine, fromColumn, m_line, m_column))
				m_timer = m_punctuationDelay;
			else
				m_timer = charTime;
			if (isFinished())
				maskFlags(4);
		}
	}

	// 0x482730
	bool TextTyper::findPunctuation(int fromLine, int fromColumn, int toLine, int toColumn)
	{
		for (int line = fromLine; line <= toLine; line++)
		{
			int lastColumn = (int)m_lines[line].size() - 1;
			if (line == toLine)
				lastColumn = toColumn;
			std::string::size_type found = m_lines[line].find('.', fromColumn);
			if (found != std::string::npos && (int)found < lastColumn)
			{
				m_line = line;
				m_column = found;
				return true;
			}
			fromColumn = 0;
		}
		return false;
	}

	// 0x4827C0
	std::string TextTyper::getTypeName() const
	{
		return "TextTyper";
	}

	// 0x4827F0
	void TextTyper::drawText(Graphics& g)
	{
		g.setHAlign(m_xAlign);
		g.setVAlign(m_yAlign);
		int lines = m_lines.size();
		if (m_linesPerPage > 0)
			lines = m_linesPerPage;
		float height = (float)(m_font->getLineHeight() * lines);
		float y;
		switch (m_yAlign)
		{
		case 1:
			y = g.getTranslation().y - (height - m_font->getLineHeight()) * 0.5f;
			break;
		case 2:
			y = g.getTranslation().y - (height - m_font->getLineHeight());
			break;
		default:
			y = g.getTranslation().y;
			break;
		}
		g.setTranslation(m_firstLineIndent + g.getTranslation().x, y);
		// the last lines up to the one being typed (a page of them when paging), that one up to m_column
		for (unsigned int i = (std::max)(0, m_line - lines + 1); i <= (unsigned int)m_line; i++)
		{
			std::string text;
			if (i == (unsigned int)m_line)
				text = m_lines[i].substr(0, m_column + 1);
			else
				text = m_lines[i];
			g.drawString(text.c_str());
			if (m_style & 2)
			{
				float width = (float)m_font->getStringWidth(text.c_str());
				float x = 0.0f;
				if (m_xAlign == 1)
					x = width * -0.5f;
				else if (m_xAlign == 2)
					x = -width;
				float lineY = 0.0f;
				if (m_yAlign == 2)
					lineY = (float)-(m_font->getLineHeight() / 2);
				else if (m_yAlign == 0)
					lineY = (float)(m_font->getLineHeight() / 2);
				g.drawLine(Vector2(x, lineY), Vector2(x + width, lineY));
			}
			if (m_style & 1)
			{
				int width = m_font->getStringWidth(text.c_str());
				float x = 0.0f;
				if (m_xAlign == 1)
					x = (float)-(width / 2);
				else if (m_xAlign == 2)
					x = -(float)width;
				float lineY = 0.0f;
				if (m_yAlign == 0)
					lineY = (float)m_font->getLineHeight();
				else if (m_yAlign == 1)
					lineY = (float)(m_font->getLineHeight() / 2);
				g.drawLine(Vector2(x, lineY), Vector2(width + x, lineY));
			}
			g.setTranslation(g.getTranslation().x + m_lineIndent, m_font->getLineHeight() + g.getTranslation().y);
		}
	}
}

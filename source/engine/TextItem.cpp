#include "TextItem.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "Application.h"
#include "Font.h"
#include "Graphics.h"

namespace engine
{
	// 0x46F4D0
	void TextItem::setShadow(float opacity, Vector2 offset)
	{
		m_shadowOpacity = opacity;
		m_shadowOffset = offset;
	}

	// 0x46F500
	void TextItem::setIndent(float firstLine, float otherLines)
	{
		m_firstLineIndent = firstLine;
		m_lineIndent = otherLines;
	}

	// 0x46F520
	void TextItem::setXAlign(int align)
	{
		m_xAlign = align;
		addTreeFlags(8);
	}

	// 0x46F540
	void TextItem::setYAlign(int align)
	{
		m_yAlign = align;
		addTreeFlags(8);
	}

	// 0x46F560
	void TextItem::setFont(const std::string& fontName)
	{
		setFont(getApplication()->getFont(fontName.c_str()));
		addTreeFlags(8);
	}

	// 0x46F5A0
	Font* TextItem::getFont() const
	{
		return m_font;
	}

	// 0x46F5B0: with the shadow style the text is drawn first at the shadow offset, with the colour (-1, -1, -1)
	// added and m_shadowOpacity of the alpha
	void TextItem::draw(Graphics& g)
	{
		if (m_text != "" && isVisible())
		{
			if (m_style & 4)
			{
				g.pushState();
				g.translate(m_shadowOffset.x, m_shadowOffset.y);
				g.setColor(-1.0f, -1.0f, -1.0f, 1.0f);
				g.setColorMode(1);
				g.setAlpha(g.getAlpha() * m_shadowOpacity);
				drawText(g);
				g.popState();
			}
			drawText(g);
		}
	}

	// 0x46F670: the widest line (with its indent) by the line count times the line height, placed at the position by
	// the alignments; empty without a font
	void TextItem::updateBounds()
	{
		if (m_font)
		{
			float width = 0.0f;
			for (unsigned int i = 0; i < m_lines.size(); ++i)
			{
				float indent = (i == 0) ? m_firstLineIndent : m_lineIndent;
				float lineWidth = m_lines[i].empty() ? 0.0f : m_font->getStringWidth(m_lines[i].c_str()) + indent;
				if (width <= lineWidth)
					width = lineWidth;
			}
			float height = (float)(m_lines.size() * m_font->getLineHeight());
			float x;
			switch (m_xAlign)
			{
			case 1:
				x = m_position.x - width * 0.5f;
				break;
			case 2:
				x = m_position.x - width;
				break;
			default:
				x = m_position.x;
				break;
			}
			float y;
			switch (m_yAlign)
			{
			case 1:
				y = m_position.y - height * 0.5f;
				break;
			case 2:
				y = m_position.y - height;
				break;
			default:
				y = m_position.y;
				break;
			}
			m_bounds.set(x, y, x + width, y + height);
		}
		else
		{
			m_bounds.clear();
		}
		removeTreeFlags(8);
	}

	// 0x46F830: the lines from the graphics translation, which moves down a line height and right by m_lineIndent
	// after each line (the first line by m_firstLineIndent); for middle and bottom alignment the first line moves up
	// by half or all of the block's height less one line. Strikethrough and underline are lines across the text's
	// width.
	void TextItem::drawText(Graphics& g)
	{
		g.setHAlign(m_xAlign);
		g.setVAlign(m_yAlign);
		float height = (float)(m_lines.size() * m_font->getLineHeight());
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
		for (unsigned int i = 0; i < m_lines.size(); ++i)
		{
			g.drawString(m_lines[i].c_str());
			if (m_style & 2)
			{
				float width = (float)m_font->getStringWidth(m_lines[i].c_str());
				float x = 0.0f;
				if (m_xAlign == 1)
					x = width * -0.5f;
				else if (m_xAlign == 2)
					x = -width;
				float lineY = 0.0f;
				if (m_yAlign == 2)
					lineY = (float)(m_font->getLineHeight() / -2);
				else if (m_yAlign == 0)
					lineY = (float)(m_font->getLineHeight() / 2);
				g.drawLine(Vector2(x, lineY), Vector2(x + width, lineY));
			}
			if (m_style & 1)
			{
				int width = m_font->getStringWidth(m_lines[i].c_str());
				float x = 0.0f;
				if (m_xAlign == 1)
					x = (float)(width / -2);
				else if (m_xAlign == 2)
					x = -(float)width;
				float lineY = 0.0f;
				if (m_yAlign == 0)
					lineY = (float)m_font->getLineHeight();
				else if (m_yAlign == 1)
					lineY = (float)(m_font->getLineHeight() / 2);
				g.drawLine(Vector2(x, lineY), Vector2(width + x, lineY));
			}
			g.setTranslation(m_lineIndent + g.getTranslation().x, m_font->getLineHeight() + g.getTranslation().y);
		}
	}

	// 0x46FC70: flag 1, a shadow offset of (2, 2) at half opacity, the bounds marked dirty (tree flag 8)
	TextItem::TextItem()
	{
		m_xAlign = 0;
		m_yAlign = 0;
		m_style = 0;
		setFlags(1);
		m_shadowOffset = Vector2(2.0f, 2.0f);
		m_shadowOpacity = 0.5f;
		m_wordWrap = 0;
		m_firstLineIndent = 0.0f;
		m_lineIndent = 0.0f;
		addTreeFlags(8);
	}

	// 0x46FDC0
	std::string TextItem::getTypeName() const
	{
		return "TextItem";
	}

	// 0x46FDF0: the lines are released twice (clear here, then the member destructor)
	TextItem::~TextItem()
	{
		m_lines.clear();
	}

	// 0x46FEE0: the lines are split at the two-character escape "\n". With a wrap width a line also breaks at its last
	// space once the width reaches m_wordWrap (the rest of the word is measured into the next line), or before the
	// character that overflows when the line has no space. A backslash before another character stays in the text
	// but is not measured.
	void TextItem::setText(const std::string& text)
	{
		m_text = text;
		m_lines.clear();
		if (m_wordWrap > 0)
		{
			int lineStart = 0;
			int lastSpace = 0;
			int width = (int)m_firstLineIndent;
			unsigned int i;
			for (i = 0; i < text.size(); ++i)
			{
				if (text[i] == ' ')
				{
					lastSpace = i;
					width += m_font->getSpaceWidth();
				}
				else
				{
					if (text[i] == '\\')
					{
						++i;
						if (text[i] == 'n')
						{
							m_lines.push_back(text.substr(lineStart, i - lineStart - 1));
							lineStart = i + 1;
							width = (int)m_lineIndent;
							continue;
						}
					}
					width += m_font->getCharWidth(text[i]);
				}
				if (width >= m_wordWrap)
				{
					if (lastSpace > lineStart)
					{
						m_lines.push_back(text.substr(lineStart, lastSpace - lineStart));
						lineStart = ++lastSpace;
						width = m_font->getStringWidth(text.substr(lineStart, i - lineStart + 1).c_str());
					}
					else
					{
						m_lines.push_back(text.substr(lineStart, i - lineStart));
						lineStart = i + 1;
						lastSpace = lineStart;
						width = (int)m_lineIndent;
					}
				}
			}
			m_lines.push_back(text.substr(lineStart, i - lineStart));
		}
		else
		{
			unsigned int start = 0;
			unsigned int end;
			do
			{
				end = text.find("\\n", start);
				if (end == std::string::npos)
					end = text.size();
				m_lines.push_back(text.substr(start, end - start));
				start = end + 2;
			} while (end != text.size());
		}
		updateBounds();
		addTreeFlags(8);
	}

	// 0x470340: prefix + the value with ',' between groups of three digits (a minus sign counts as a digit), as the
	// only line; the bounds are updated but not marked dirty
	void TextItem::setNumber(int value, const std::string& prefix)
	{
		char buffer[20];
		sprintf(buffer, "%d", value);
		m_lines.clear();
		int from = strlen(buffer) - 1;
		int last = from + from / 3;
		int count = 0;
		for (int to = last; to >= 0; )
		{
			if (count == 3)
			{
				buffer[to--] = ',';
				count = 0;
			}
			buffer[to--] = buffer[from--];
			++count;
		}
		buffer[last + 1] = 0;
		m_text = prefix + std::string(buffer);
		m_lines.push_back(m_text);
		updateBounds();
	}

	// 0x470500: "h:mm:ss" with hours or showHours, else "mm:ss" (the minutes padded too) or "0:ss", as the only line
	void TextItem::setTime(int seconds, bool showHours)
	{
		int second = seconds % 60;
		int minute = seconds / 60 % 60;
		int hour = seconds / 60 / 60;
		char buffer[20];
		if (hour > 0 || showHours)
			sprintf(buffer, "%d:%s%d:%s%d", hour, minute < 10 ? "0" : "", minute, second < 10 ? "0" : "", second);
		else if (minute > 0)
			sprintf(buffer, "%s%d:%s%d", minute < 10 ? "0" : "", minute, second < 10 ? "0" : "", second);
		else
			sprintf(buffer, "0:%s%d", second < 10 ? "0" : "", second);
		m_lines.clear();
		m_text = std::string(buffer);
		m_lines.push_back(m_text);
		updateBounds();
	}
}

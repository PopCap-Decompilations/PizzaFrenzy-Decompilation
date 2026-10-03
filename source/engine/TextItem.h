// engine::TextItem: a text component (line splitting and word wrap, alignment, underline/strikethrough/shadow).
#pragma once

#include <string>
#include <vector>

#include "Component.h"
#include "Point.h"

namespace engine
{
	class Font;
	class Graphics;

	// Component +0x00, members from +0x108, then the vtordisp (+0x158) and the Interface subobject (+0x15C): 0x160
	// bytes. The text is split into m_lines at the two-character escape "\n" and word-wrapped to m_wordWrap
	// pixels; draw() calls drawText (slot 66, overridden by TextTyper), once more offset for the shadow style.
	class TextItem : public Component
	{
	public:
		TextItem();
		virtual ~TextItem();

		virtual std::string getTypeName() const;	// slot 24 (engine::Component): "TextItem"
		virtual void draw(Graphics& g);				// slot 36 (engine::Component)
		virtual void updateBounds();				// slot 38 (engine::Component)

		virtual void drawText(Graphics& g);			// slot 66: the lines at the graphics origin with the font

		// setFont(const std::string&) below calls Component's virtual setFont(Font*) (slot 14) on this object
		using Component::setFont;

		// 0x404280 (copy emitted in another object: inline)
		std::string getText() const
		{
			return m_text;
		}

		void setShadow(float opacity, Vector2 offset);		// the shadow style bit is set separately
		void setIndent(float firstLine, float otherLines);
		void setXAlign(int align);
		void setYAlign(int align);
		void setFont(const std::string& fontName);			// the application's font of that name
		Font* getFont() const;
		void setText(const std::string& text);
		void setNumber(int value, const std::string& prefix);	// prefix + digits with ',' thousands separators
		void setTime(int seconds, bool showHours);				// "h:mm:ss", "m:ss" or "0:ss"

		std::string m_text;						// +0x108 text as set
		std::vector<std::string> m_lines;		// +0x124 lines after splitting and wrapping
		int m_xAlign;							// +0x134 0 left, 1 center, 2 right (xAlign)
		int m_yAlign;							// +0x138 0 top, 1 middle, 2 bottom (yAlign)
		unsigned char m_style;					// +0x13C 1 underline, 2 strikethrough, 4 shadow (style)
		float m_firstLineIndent;				// +0x140 x offset of the first line
		float m_lineIndent;						// +0x144 x offset of the other lines
		int m_wordWrap;							// +0x148 wrap width in pixels, <= 0 no wrapping (wordWrap)
		float m_shadowOpacity;					// +0x14C default 0.5 (shadowOpacity)
		Vector2 m_shadowOffset;					// +0x150 default (2, 2) (shadowOffset)
	};
}

// engine::TextButton: a button whose face is a TextItem drawn in a font and additive colour per state.
#pragma once

#include <string>

#include "Button.h"
#include "Color.h"

namespace engine
{
	class TextItem;

	// Layout element <textButton>: create() makes the caption; onStateChanged gives it the font and colour of the
	// button state. The constructor, destructor and getTypeName are compiler-generated or inline in the original
	// (emitted in ScreenLayoutParser.cpp; the constructor leaves m_text uninitialised). Layout: Button +0x00,
	// members +0x17C, then the vtordisp and Interface (0x20C bytes).
	class TextButton : public Button
	{
	public:
		TextButton();

		// Component
		// "TextButton"
		virtual std::string getTypeName() const;

		// Button
		// font and colour of the state: 0 and 2 normal, 1 over, 3 pressed
		virtual void onStateChanged();

		// slot 91: m_text->setText(text)
		virtual void setText(std::string text);
		// slot 92: the caption's xAlign
		virtual void setXAlign(int align);
		// slot 93: the caption's yAlign
		virtual void setYAlign(int align);
		// slot 94: caption style flags |= style
		virtual void addTextStyle(unsigned char style);
		// slot 95: a copy of the caption's text
		virtual std::string getText() const;
		// slot 96: stores the fonts and creates the caption (a TextItem, flags 2 and 4, black); false if the normal
		// or the over font is empty
		virtual bool create(const std::string& normalFont, const std::string& overFont, const std::string& pressedFont);
		// slot 97
		virtual void setTextColors(const Color& normal, const Color& over, const Color& pressed);

		std::string m_fontName;					// +0x17C normal font resource name
		std::string m_overFontName;				// +0x198 mouse-over font resource name
		std::string m_activeFontName;			// +0x1B4 pressed font resource name
		TextItem* m_text;						// +0x1D0 the caption made by create()
		Color m_upAddColor;						// +0x1D4 additive caption colour, normal
		Color m_overAddColor;					// +0x1E4 additive caption colour, mouse over
		Color m_activeAddColor;					// +0x1F4 additive caption colour, pressed
	};
}

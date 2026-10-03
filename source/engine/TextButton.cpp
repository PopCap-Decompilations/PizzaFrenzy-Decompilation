#include "TextButton.h"

#include "TextItem.h"

namespace engine
{
	// 0x484B20
	TextButton::TextButton()
	{
	}

	// 0x484E40
	std::string TextButton::getTypeName() const
	{
		return "TextButton";
	}

	// 0x495810
	void TextButton::setXAlign(int align)
	{
		m_text->setXAlign(align);
	}

	// 0x495820
	void TextButton::setYAlign(int align)
	{
		m_text->setYAlign(align);
	}

	// 0x495830
	void TextButton::addTextStyle(unsigned char style)
	{
		m_text->m_style |= style;
	}

	// 0x495850
	void TextButton::onStateChanged()
	{
		switch (m_state)
		{
		case 0:
		case 2:
			m_text->setFont(m_fontName);
			m_text->setColor(m_upAddColor);
			break;
		case 1:
			m_text->setFont(m_overFontName);
			m_text->setColor(m_overAddColor);
			break;
		case 3:
			m_text->setFont(m_activeFontName);
			m_text->setColor(m_activeAddColor);
			break;
		}
	}

	// 0x4958F0
	void TextButton::setTextColors(const Color& normal, const Color& over, const Color& pressed)
	{
		m_upAddColor = normal;
		m_overAddColor = over;
		m_activeAddColor = pressed;
	}

	// 0x495960
	void TextButton::setText(std::string text)
	{
		m_text->setText(text);
	}

	// 0x4959C0
	std::string TextButton::getText() const
	{
		return m_text->getText();
	}

	// 0x4959F0
	bool TextButton::create(const std::string& normalFont, const std::string& overFont, const std::string& pressedFont)
	{
		if (normalFont.empty() || overFont.empty())
			return false;
		m_fontName = normalFont;
		m_overFontName = overFont;
		m_activeFontName = pressedFont;
		m_text = new TextItem();
		addChild(m_text);
		m_text->setFont(m_fontName);
		setFlags(2);
		setFlags(4);
		updateBounds();
		m_upAddColor.set(0, 0, 0);
		m_overAddColor.set(0, 0, 0);
		m_activeAddColor.set(0, 0, 0);
		return true;
	}
}

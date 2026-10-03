#include "EditBox.h"

#include <cstdlib>
#include <cstring>
#include <mbstring.h>
#include <string>

#include <windows.h>

#include "Application.h"
#include "Font.h"
#include "Image.h"
#include "TextItem.h"

namespace engine
{
	// 0x471C50
	void EditBox::setMaxLength(int length)
	{
		m_maxLength = length;
	}

	// 0x471C60
	void EditBox::setCursorBlinkTime(float seconds)
	{
		m_blinkTime = seconds;
		m_blinkTimer = seconds;
	}

	// 0x471DB0: the background image, the text (left-aligned in the font) and the cursor image, in that order
	void EditBox::init(const std::string& backgroundImage, const std::string& cursorImage, const std::string& fontName)
	{
		addChild(new Image(getApplication()->getImage(backgroundImage.c_str())));
		m_textItem = new TextItem();
		m_textItem->setFont(fontName);
		m_textItem->setXAlign(0);
		addChild(m_textItem);
		m_cursor = new Image(getApplication()->getImage(cursorImage.c_str()));
		addChild(m_cursor);
		m_font = getApplication()->getFont(fontName.c_str());
	}

	// 0x471FC0
	void EditBox::setTextOffset(const Vector2& offset)
	{
		m_textItem->setPosition(offset);
	}

	// 0x471FD0: only the cursor blinks (the children are not updated)
	void EditBox::update(UpdateContext& context)
	{
		if (m_blinkTime == 0.0f)
			m_cursor->setVisible(true);
		else
		{
			m_blinkTimer -= context.elapsed;
			if (m_blinkTimer < 0.0f)
			{
				m_cursor->setVisible(!m_cursor->isVisible());
				m_blinkTimer = m_blinkTime;
			}
		}
	}

	// 0x472110
	void EditBox::setAction(const std::string& action)
	{
		m_action = action;
	}

	// 0x472130: the cursor image after the text up to the caret
	void EditBox::updateCursor()
	{
		if (m_cursor && m_font)
		{
			int width = m_font->getStringWidth(m_text.substr(0, m_cursorPos).c_str());
			Vector2 position = m_cursorOffset;
			position.x += width;
			m_cursor->setPosition(position);
		}
	}

	// 0x472220
	void EditBox::setCursorOffset(const Vector2& offset)
	{
		m_cursorOffset = offset;
		updateCursor();
	}

	// 0x472240: a printable character (after the filter, which can reject it) inserted at the caret
	void EditBox::onChar(char c)
	{
		if (m_text.size() < (unsigned int)m_maxLength && _ismbcprint(c))
		{
			if (m_charFilter)
			{
				c = m_charFilter(c);
				if (c == 0)
					return;
			}
			std::string left = m_text.substr(0, m_cursorPos);
			std::string right = m_text.substr(m_cursorPos, m_text.size());
			m_text = left;
			m_text += c;
			m_text += right;
			m_cursorPos++;
			m_cursor->setVisible(true);
			m_blinkTimer = m_blinkTime;
			if (m_textItem)
				m_textItem->setText(m_text);
			updateCursor();
		}
	}

	// 0x4723C0
	void EditBox::deleteForward()
	{
		if ((unsigned int)m_cursorPos < m_text.size())
		{
			std::string left = m_text.substr(0, m_cursorPos);
			std::string right = m_text.substr(m_cursorPos + 1, m_text.size());
			m_text = left;
			m_text += right;
			if (m_textItem)
				m_textItem->setText(m_text);
			updateCursor();
		}
	}

	// 0x4724D0
	void EditBox::deleteBackward()
	{
		if (m_cursorPos > 0)
		{
			std::string left = m_text.substr(0, m_cursorPos - 1);
			std::string right = m_text.substr(m_cursorPos, m_text.size());
			m_text = left;
			m_text += right;
			m_cursorPos--;
			if (m_textItem)
				m_textItem->setText(m_text);
			updateCursor();
		}
	}

	// 0x4725E0: the clipboard text (cut to the free length) inserted at the caret. The copy is never freed, and the
	// clipboard stays open when the box is full.
	void EditBox::paste()
	{
		if (IsClipboardFormatAvailable(CF_TEXT))
		{
			OpenClipboard(getApplication()->getWindowHandle());
			HANDLE data = GetClipboardData(CF_TEXT);
			int length = m_maxLength - m_text.size();
			if (length > 0)
			{
				int dataLength = GlobalSize(data) - 1;
				if (dataLength < length)
					length = dataLength;
				char* text = (char*)malloc(length + 1);
				memcpy(text, GlobalLock(data), length);
				text[length] = 0;
				std::string left = m_text.substr(0, m_cursorPos);
				std::string right = m_text.substr(m_cursorPos, m_text.size());
				std::string pasted(text);
				m_text = left;
				m_text += pasted;
				m_cursorPos = m_text.size();
				m_text += right;
				GlobalUnlock(data);
				CloseClipboard();
				if (m_textItem)
					m_textItem->setText(m_text);
				updateCursor();
			}
		}
	}

	// 0x4727F0: cut to the maximum length, caret at the end
	void EditBox::setText(const std::string& text)
	{
		m_text = text.substr(0, m_maxLength);
		m_cursorPos = m_text.size();
		if (m_textItem)
			m_textItem->setText(m_text);
		updateCursor();
		if (m_cursorPos > (int)text.size())
			m_cursorPos = text.size();
		updateCursor();
	}

	// 0x4728C0: the children are not deactivated
	void EditBox::deactivate()
	{
		if (m_cursor)
			m_cursor->setVisible(false);
		getApplication()->m_keyDownSignal.disconnect(this);
		getApplication()->m_keyRepeatSignal.disconnect(this);
		getApplication()->m_charSignal.disconnect(this);
		clearFlags(4);
	}

	// 0x472920: any handled key shows the cursor and restarts its blink
	void EditBox::onKeyDown(int key)
	{
		switch (key)
		{
		case VK_BACK:
			deleteBackward();
			break;
		case VK_LEFT:
			m_cursorPos = max(m_cursorPos - 1, 0);
			updateCursor();
			break;
		case VK_RIGHT:
			m_cursorPos = min(m_cursorPos + 1, (int)m_text.size());
			updateCursor();
			break;
		case VK_DELETE:
			deleteForward();
			break;
		case VK_END:
			m_cursorPos = m_text.size();
			updateCursor();
			break;
		case VK_HOME:
			m_cursorPos = 0;
			updateCursor();
			break;
		case 'V':
			if (GetAsyncKeyState(VK_CONTROL))
				paste();
			break;
		default:
			return;
		}
		m_cursor->setVisible(true);
		m_blinkTimer = m_blinkTime;
	}

	// 0x472A70
	void EditBox::onKeyRepeat(int key, int count)
	{
		for (int i = 0; i < count; i++)
			onKeyDown(key);
	}

	// 0x472AA0
	EditBox::~EditBox()
	{
		deactivate();
		m_cursor = 0;
		m_textItem = 0;
		m_font = 0;
	}

	// 0x472C90
	std::string EditBox::getTypeName() const
	{
		return "EditBox";
	}

	// 0x472EA0: the maximum length, the cursor offset and the blink time and timer are left uninitialised
	EditBox::EditBox()
	{
		m_cursor = 0;
		m_textItem = 0;
		m_font = 0;
		m_cursorPos = 0;
		m_textOffset = Vector2(0.0f, 0.0f);
		m_charFilter = 0;
	}

	// 0x473060: the children are not activated
	void EditBox::activate()
	{
		if (m_cursor)
		{
			m_cursor->setVisible(true);
			m_blinkTimer = m_blinkTime;
		}
		getApplication()->m_keyDownSignal.disconnect(this);
		getApplication()->m_keyRepeatSignal.disconnect(this);
		getApplication()->m_charSignal.disconnect(this);
		getApplication()->m_keyDownSignal.connect(this, &EditBox::onKeyDown);
		getApplication()->m_keyRepeatSignal.connect(this, &EditBox::onKeyRepeat);
		getApplication()->m_charSignal.connect(this, &EditBox::onChar);
		setFlags(4);
	}

	// 0x471C80 (folded)
	void EditBox::setCharFilter(char (*filter)(char))
	{
		m_charFilter = filter;
	}

	// 0x48F080 (folded)
	const std::string& EditBox::getText() const
	{
		return m_text;
	}
}

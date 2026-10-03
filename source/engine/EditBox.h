// engine::EditBox: a text entry box (background image, text, blinking cursor) edited with the keyboard while active.
#pragma once

#include <string>

#include "Container.h"
#include "Point.h"
#include "RefPtr.h"
#include "sigslot.h"

namespace engine
{
	class Font;
	class Image;
	class TextItem;
	struct UpdateContext;

	// Container +0x00, sigslot::has_slots<> +0x128, members from +0x138, then the vtordisp (+0x1A0) and the
	// Interface subobject (+0x1A4): 0x1A8 bytes. Created by the layout parser (<editBox>). While active it is
	// connected to the application's key-down, key-repeat and character signals (arrows, Home/End, Backspace,
	// Delete, Ctrl+V paste, typed characters).
	class EditBox : public Container, public sigslot::has_slots<>
	{
	public:
		EditBox();
		virtual ~EditBox();

		virtual std::string getTypeName() const;	// slot 24 (engine::Component): "EditBox"
		virtual void activate();					// slot 34 (engine::Component)
		virtual void deactivate();					// slot 35 (engine::Component)
		virtual void update(UpdateContext& context);	// slot 37 (engine::Component): cursor blink only

		const std::string& getText() const;			// body folded with Table::getBackgroundColor (0x48F080)
		void setMaxLength(int length);
		void setCursorBlinkTime(float seconds);
		void setCharFilter(char (*filter)(char));	// body folded with ListBox::setSelectedIndex (0x471C80)
		void init(const std::string& backgroundImage, const std::string& cursorImage, const std::string& fontName);
		void setTextOffset(const Vector2& offset);
		void setAction(const std::string& action);
		void updateCursor();
		void setCursorOffset(const Vector2& offset);
		void onChar(char c);
		void deleteForward();
		void deleteBackward();
		void paste();
		void setText(const std::string& text);
		void onKeyDown(int key);
		void onKeyRepeat(int key, int count);

		std::string m_text;						// +0x138 edited text
		int m_maxLength;						// +0x154 strLen
		RefPtr<Image> m_cursor;					// +0x158 cursorImg
		RefPtr<TextItem> m_textItem;			// +0x15C shows m_text
		RefPtr<Font> m_font;					// +0x160 measures the cursor position
		int m_cursorPos;						// +0x164 caret index
		Vector2 m_cursorOffset;					// +0x168 cursorOffset
		float m_blinkTime;						// +0x170 cursorBlinkTime; 0 = always visible
		float m_blinkTimer;						// +0x174 countdown to the next toggle
		Vector2 m_textOffset;					// +0x178 set to (0, 0), never used (setTextOffset moves the TextItem)
		std::string m_action;					// +0x180 action attribute, stored only
		char (*m_charFilter)(char);				// +0x19C maps a typed character; 0 rejects it (e.g. HighScoreManager::filterNameChar)
	};
}

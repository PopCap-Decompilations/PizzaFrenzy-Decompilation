// engine::TextTyper: a text item that types itself out character by character.
#pragma once

#include <string>

#include "TextItem.h"

namespace engine
{
	class Graphics;

	// Pauses after a '.' and at page ends; updates only while Component flag 4 is not suppressed (start/pause).
	// TextItem +0x00, members from +0x158, then the vtordisp (+0x174) and the Interface subobject (+0x178): 0x17C
	// bytes. Implicit destructor (its copy 0x482420 is a bare jump to ~TextItem without vfptr stores).
	class TextTyper : public TextItem
	{
	public:
		TextTyper();

		virtual std::string getTypeName() const;								// slot 24 (engine::Component)
		virtual void update(UpdateContext& context);							// slot 37 (engine::Component)
		virtual void drawText(Graphics& g);										// slot 66 (engine::TextItem): only the typed part

		virtual void setup(float wrapWidth, float charsPerSecond, float punctuationDelay);	// slot 67: wrapWidth to TextItem's int
		virtual void setPaging(int linesPerPage, float pageDelay);				// slot 68
		virtual void setText(const std::string& text);							// slot 69: TextItem::setText, then reset()
		virtual void reset();													// slot 70
		virtual void start();													// slot 71
		virtual void pause();													// slot 72
		virtual void finish();													// slot 73
		virtual bool isFinished() const;										// slot 74
		virtual bool findPunctuation(int fromLine, int fromColumn, int toLine, int toColumn);	// slot 75

		int m_line;								// +0x158 line being typed
		int m_column;							// +0x15C last typed character of m_line
		float m_charsPerSecond;					// +0x160
		float m_punctuationDelay;				// +0x164 pause after a '.'
		float m_timer;							// +0x168 time to the next character
		int m_linesPerPage;						// +0x16C 0 = no paging
		float m_pageDelay;						// +0x170 pause at a page end
	};
}

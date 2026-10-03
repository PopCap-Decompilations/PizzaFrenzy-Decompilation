// engine::HighScoreListHandler: the SAX handler that reads a high score list into a HighScoreTable.
#pragma once

#include <string>

#include "Properties.h"
#include "XmlDefaultHandler.h"

namespace engine
{
	class HighScoreTable;

	// <highscores><entry><name/><score/><level/><rank/></entry>...<highlight/></highscores>: every <entry> becomes an
	// entry of the table. Used by HighScoreTable::run (0x482E60) on the global score response. Implicit destructor
	// (0x494C20). Layout: XmlDefaultHandler +0x00, members +0x14, then the vtordisp and Interface (0x60 bytes).
	class HighScoreListHandler : public XmlDefaultHandler
	{
	public:
		HighScoreListHandler(HighScoreTable* table);

		// XmlHandler
		// <highscores>: m_highlight = 0; <entry>: clears m_entryAttributes; always clears m_text
		virtual void startElement(const std::string& name, const Properties& attributes);
		// name/score/level/rank -> attributes player/points/level/rank; highlight -> toInt; </entry>: a new table
		// entry with the attributes, inserted by rank (rank > 0) or added; </highscores>: selects m_highlight
		virtual void endElement(const std::string& name);
		// m_text += text
		virtual void characters(const std::string& text);

		HighScoreTable* m_table;				// +0x14 the table filled
		Properties m_entryAttributes;			// +0x18 player/points/level/rank of the current <entry>
		std::string m_text;						// +0x38 character data of the current element
		int m_highlight;						// +0x54 <highlight> value (the rank to highlight)
	};
}

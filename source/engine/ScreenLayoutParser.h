// engine::ScreenLayoutParser: XML handler that builds a ScreenLayout from res/screenLayouts/*.xml.
#pragma once

#include <string>
#include <vector>

#include "RefPtr.h"
#include "XmlHandlerBase.h"

namespace engine
{
	class Container;
	class Properties;
	class RadioGroup;
	class ScreenLayout;
	class Table;

	// ScreenLayout::load runs it through the application's loadXml; <include> runs a nested one. One parseX method
	// per element; group, selector and radioGroup nest on m_containers. Layout: XmlHandlerBase +0x00 (XmlHandler
	// at +0x0C), members +0x14, then the vtordisp and Interface (0x54 bytes).
	class ScreenLayoutParser : public XmlHandlerBase
	{
	public:
		// m_layout is left to setLayout
		ScreenLayoutParser();
		virtual ~ScreenLayoutParser();

		// slot 1: m_layout = layout (body folded with the other "store at +0x14" setters)
		virtual void setLayout(ScreenLayout* layout);

		// XmlHandler
		virtual void startElement(const std::string& name, const Properties& attrs);
		virtual void endElement(const std::string& name);
		virtual void startDocument();
		virtual void endDocument();

		bool parseImage(const Properties& attrs);
		bool parseImageButton(const Properties& attrs);
		bool parseCheckBox(const Properties& attrs);
		bool parseTextButton(const Properties& attrs);
		bool parseText(const Properties& attrs);
		bool parseParticleSystem(const Properties& attrs);
		bool parseTextSub(const Properties& attrs);
		bool parseEditBox(const Properties& attrs);
		bool parseListBox(const Properties& attrs);
		bool parseTable(const Properties& attrs);
		bool parseColumn(const Properties& attrs);
		bool parseKeyAction(const Properties& attrs);
		bool parseInclude(const Properties& attrs);
		bool parseGroup(const Properties& attrs);
		bool parseSelector(const Properties& attrs);
		bool parseRadioGroup(const Properties& attrs);
		// </radioGroup>
		void endRadioGroup();

		ScreenLayout* m_layout;					// +0x14 the screen being built (not reference counted)
		std::vector<Container*> m_containers;	// +0x18 open containers: [0] the layout (startDocument), back() receives new components
		std::string m_initialSelect;			// +0x28 initialSelect attribute of the open radioGroup
		RefPtr<RadioGroup> m_radioGroup;		// +0x44 open radioGroup; checkBoxes inside join it
		RefPtr<Table> m_table;					// +0x48 open table; column elements add to it
	};
}

// engine::RadioGroup: a group of Checkboxes of which one is checked.
#pragma once

#include <string>
#include <vector>

#include "ButtonListener.h"
#include "Container.h"
#include "RefPtr.h"
#include "sigslot.h"

namespace engine
{
	class Checkbox;

	// Layout element <radioGroup>: the checkboxes inside it join the group (addButton makes it their listener); a
	// click checks the clicked one, unchecks the others and emits m_selectionChanged with its name. Layout:
	// Container +0x00, ButtonListener +0x128, members +0x12C, then the vtordisp and Interface (0x170 bytes).
	class RadioGroup : public Container, public ButtonListener
	{
	public:
		RadioGroup();
		// m_buttons.clear(), then the members
		virtual ~RadioGroup();

		// Component
		// "RadioGroup"
		virtual std::string getTypeName() const;

		// slot 75: m_buttons.push_back(button); button->addListener(this, button->getName())
		virtual void addButton(Checkbox* button);
		// slot 76: checks the named button, unchecks the others, emits m_selectionChanged(name); an unknown name logs
		// "WARNING: unable to find selection '%s' for radio box\n" and checks the first button
		virtual void setSelection(const std::string& name);

		// ButtonListener
		// onMouseEnter to onMouseUp ignore the command (body folded, 0x496410)
		virtual void onMouseEnter(std::string command);
		virtual void onMouseLeave(std::string command);
		virtual void onMouseDown(std::string command);
		virtual void onMouseUp(std::string command);
		// the clicked checkbox's name: setSelection without the warning
		virtual void onClick(std::string command);

		sigslot::signal1<const std::string&> m_selectionChanged;	// +0x12C emitted with the selected name
		std::vector<RefPtr<Checkbox> > m_buttons;	// +0x13C
		std::string m_selection;				// +0x14C never read
	};
}

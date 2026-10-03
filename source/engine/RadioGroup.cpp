#include "RadioGroup.h"

#include "Application.h"
#include "Checkbox.h"

namespace engine
{
	// 0x495D20
	void RadioGroup::setSelection(const std::string& name)
	{
		if (m_buttons.empty())
			return;
		bool found = false;
		for (std::vector<RefPtr<Checkbox> >::iterator it = m_buttons.begin(); it != m_buttons.end(); ++it)
		{
			if ((*it)->getName() == name)
			{
				(*it)->setChecked(true);
				m_selectionChanged.emit(name);
				found = true;
			}
			else
			{
				(*it)->setChecked(false);
			}
		}
		if (!found)
		{
			getApplication()->log("WARNING: unable to find selection '%s' for radio box\n", name.c_str());
			m_buttons[0]->setChecked(true);
		}
	}

	// 0x495E40
	void RadioGroup::onClick(std::string command)
	{
		for (std::vector<RefPtr<Checkbox> >::iterator it = m_buttons.begin(); it != m_buttons.end(); ++it)
		{
			if ((*it)->getName() == command)
			{
				(*it)->setChecked(true);
				m_selectionChanged.emit(command);
			}
			else
			{
				(*it)->setChecked(false);
			}
		}
	}

	// 0x4962C0
	RadioGroup::~RadioGroup()
	{
		m_buttons.clear();
	}

	// 0x496410 (folded)
	void RadioGroup::onMouseEnter(std::string command)
	{
	}

	// 0x496410 (folded)
	void RadioGroup::onMouseLeave(std::string command)
	{
	}

	// 0x496410 (folded)
	void RadioGroup::onMouseDown(std::string command)
	{
	}

	// 0x496410 (folded)
	void RadioGroup::onMouseUp(std::string command)
	{
	}

	// 0x496430
	std::string RadioGroup::getTypeName() const
	{
		return "RadioGroup";
	}

	// 0x4964D0
	RadioGroup::RadioGroup()
	{
	}

	// 0x4965E0
	void RadioGroup::addButton(Checkbox* button)
	{
		m_buttons.push_back(button);
		button->addListener(this, button->getName());
	}
}

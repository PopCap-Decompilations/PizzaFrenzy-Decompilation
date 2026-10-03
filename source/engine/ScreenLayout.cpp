#include "ScreenLayout.h"

#include <map>
#include <string>
#include <utility>

#include <windows.h>

#include "ActionListener.h"
#include "Application.h"
#include "Button.h"
#include "RadioGroup.h"
#include "ScreenLayoutParser.h"

namespace engine
{
	// 0x4646B0
	void ScreenLayout::addChild(Component* child)
	{
		Container::addChild(child);
	}

	// 0x46498B (chunk of load: the catch (...) continuation, destroys the parser and the path, returns false)
	// 0x4649A1 (chunk of load: the epilogue)
	// 0x4648F0
	bool ScreenLayout::load(std::string path)
	{
		ScreenLayoutParser parser;
		clearKeyActions();
		parser.setLayout(this);
		setName(path);
		try
		{
			getApplication()->loadXml(path, &parser);
		}
		catch (...)
		{
			return false;
		}
		return true;
	}

	// 0x4649F0
	void ScreenLayout::addButton(Button* button, std::string name, std::string action)
	{
		button->addListener(this, action);
	}

	// 0x464A90
	void ScreenLayout::onClick(std::string command)
	{
		if (m_actionListener)
			m_actionListener->onAction(command);
	}

	// 0x464B10
	void ScreenLayout::doAction(const std::string& action)
	{
		if (m_actionListener)
			m_actionListener->onAction(action);
	}

	// 0x464B60
	void ScreenLayout::onKeyDown(int keyCode)
	{
		if (m_actionListener && keyCode == VK_RETURN && !m_defaultAction.empty())
			m_actionListener->onAction(m_defaultAction);
		else if (m_actionListener && keyCode == VK_ESCAPE && !m_cancelAction.empty())
			m_actionListener->onAction(m_cancelAction);
		else
		{
			// the original does not test m_actionListener here
			std::map<int, std::string>::iterator it = m_keyActions.find(keyCode);
			if (it != m_keyActions.end())
				m_actionListener->onAction(it->second);
		}
	}

	// 0x464C00
	Component* ScreenLayout::getComponent(const std::string& name)
	{
		std::map<std::string, Component*>::iterator it = m_components.find(name);
		if (it == m_components.end())
			return 0;
		return it->second;
	}

	// 0x464C30
	void ScreenLayout::setDefaultAction(const std::string& action)
	{
		m_defaultAction = action;
	}

	// 0x464C50
	void ScreenLayout::setCancelAction(const std::string& action)
	{
		m_cancelAction = action;
	}

	// 0x465430
	void ScreenLayout::deactivate()
	{
		getApplication()->m_keyDownSignal.disconnect(this);
		maskFlags(2);
		Container::deactivate();
		refreshMouseOver();
	}

	// 0x465890
	void ScreenLayout::clearKeyActions()
	{
		m_keyActions.clear();
	}

	// 0x465D10
	void ScreenLayout::activate()
	{
		getApplication()->m_keyDownSignal.connect(this, &ScreenLayout::onKeyDown);
		unmaskFlags(2);
		Container::activate();
		refreshMouseOver();
	}

	// 0x465D60
	void ScreenLayout::connectRadioGroup(RadioGroup* group)
	{
		group->m_selectionChanged.connect(this, &ScreenLayout::doAction);
	}

	// 0x465D90
	bool ScreenLayout::addKeyAction(int keyCode, const std::string& action)
	{
		return m_keyActions.insert(std::pair<int, std::string>(keyCode, action)).second;
	}

	// 0x466160
	ScreenLayout::~ScreenLayout()
	{
		m_keyActions.clear();
	}

	// 0x466320
	void ScreenLayout::addComponent(Component* component, std::string name)
	{
		m_components[name] = component;
	}

	// 0x466380
	ScreenLayout::ScreenLayout()
	{
		m_actionListener = 0;
		setFlags(4);
	}

	// 0x47A140 (folded)
	void ScreenLayout::setActionListener(ActionListener* listener)
	{
		m_actionListener = listener;
	}

	// 0x496410 (folded)
	void ScreenLayout::onMouseEnter(std::string command)
	{
	}

	// 0x496410 (folded)
	void ScreenLayout::onMouseLeave(std::string command)
	{
	}

	// 0x496410 (folded)
	void ScreenLayout::onMouseDown(std::string command)
	{
	}

	// 0x496410 (folded)
	void ScreenLayout::onMouseUp(std::string command)
	{
	}
}

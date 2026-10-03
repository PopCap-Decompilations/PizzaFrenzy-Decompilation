#include "UserSelectScreen.h"

#include <string>

#include "Component.h"
#include "ListBox.h"
#include "User.h"
#include "UserManager.h"

namespace engine
{
	// 0x473140
	UserSelectScreen::~UserSelectScreen()
	{
	}

	// 0x473190: the current user is selected in the list
	void UserSelectScreen::activate()
	{
		Screen::activate();
		m_userList->setSelectedIndex(UserManager::getInstance()->getCurrentUserIndex());
		updateButtons();
	}

	// 0x4731C0
	void UserSelectScreen::selectUser()
	{
		UserManager::getInstance()->setCurrentUser(m_userList->getSelectedIndex());
	}

	// 0x4731E0
	void UserSelectScreen::deleteSelectedUser()
	{
		UserManager::getInstance()->removeUser(m_userList->getSelectedIndex());
		updateButtons();
	}

	// 0x473210
	void UserSelectScreen::onSelectionChanged(int index)
	{
		updateButtons();
	}

	// 0x473220
	bool UserSelectScreen::isFull()
	{
		return UserManager::getInstance()->getUserCount() >= m_userList->getVisibleRowCount();
	}

	// 0x4732A0: the members are left uninitialised
	UserSelectScreen::UserSelectScreen()
	{
	}

	// 0x473460: a disabled button is deactivated and darkened (colour mode 1, colour -0.2): "new" while the list's
	// rows are all filled, "delete" without users or a selection, "ok" without a selection
	void UserSelectScreen::updateButtons()
	{
		Component* newButton = getComponent("new");
		Component* deleteButton = getComponent("delete");
		Component* okButton = getComponent("ok");
		if (newButton)
		{
			if (m_userList->getItemCount() >= m_userList->getVisibleRowCount())
			{
				newButton->deactivate();
				newButton->setColorMode(1);
				newButton->setColor(-0.2f, -0.2f, -0.2f);
			}
			else
			{
				newButton->activate();
				newButton->setColorMode(0);
			}
		}
		if (deleteButton)
		{
			if (m_userList->getItemCount() > 0 && m_userList->getSelectedIndex() >= 0)
			{
				deleteButton->activate();
				deleteButton->setColorMode(0);
			}
			else
			{
				deleteButton->deactivate();
				deleteButton->setColorMode(1);
				deleteButton->setColor(-0.2f, -0.2f, -0.2f);
			}
		}
		if (okButton)
		{
			if (m_userList->getSelectedIndex() >= 0)
			{
				okButton->activate();
				okButton->setColorMode(0);
			}
			else
			{
				okButton->deactivate();
				okButton->setColorMode(1);
				okButton->setColor(-0.2f, -0.2f, -0.2f);
			}
		}
	}

	// 0x4736E0: "" when no user is selected
	std::string UserSelectScreen::getSelectedUserName() const
	{
		User* user = UserManager::getInstance()->getUser(m_userList->getSelectedIndex());
		if (user)
			return user->getName();
		return "";
	}

	// 0x4738A0: the list shows the UserManager's users with the current one selected
	void UserSelectScreen::init(void* param)
	{
		m_userList = static_cast<ListBox*>(getComponent("userList"));
		m_userList->setModel(new UserListModel());
		m_actionSignal.connect(this, &UserSelectScreen::onCommand);
		m_userList->setSelectedIndex(UserManager::getInstance()->getCurrentUserIndex());
		m_maxUsers = m_userList->getVisibleRowCount();
		m_userList->m_selectionChanged.connect(this, &UserSelectScreen::onSelectionChanged);
		updateButtons();
	}

	// 0x492310 (folded)
	void UserSelectScreen::onCommand(const std::string& command)
	{
	}
}

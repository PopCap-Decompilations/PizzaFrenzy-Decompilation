// engine::UserSelectScreen: the user profile selection screen (res/screenLayouts/userSelect.xml).
#pragma once

#include <string>

#include "Screen.h"

namespace engine
{
	class ListBox;

	// Screen +0x00, members from +0x1D4, then the vtordisp (+0x1E0) and the Interface subobject (+0x1E4): 0x1E8
	// bytes. A "userList" list box over the UserManager's profiles with "new", "delete" and "ok" buttons; created
	// by the game with userSelect.xml. The constructor leaves the members uninitialised.
	class UserSelectScreen : public Screen
	{
	public:
		UserSelectScreen();
		virtual ~UserSelectScreen();

		virtual void activate();								// slot 34 (engine::Component)

		virtual void init(void* param);							// slot 95: after the layout is loaded (argument unused)
		virtual void selectUser();								// slot 96: the selected profile becomes the current user
		virtual void deleteSelectedUser();						// slot 97
		virtual bool isFull();									// slot 98: user count >= visible rows of the list
		virtual void onCommand(const std::string& command);		// slot 99: the screen's action signal; empty (body folded: 0x492310)
		virtual void onSelectionChanged(int index);				// slot 100: the list's selection signal
		virtual void updateButtons();							// slot 101

		std::string getSelectedUserName() const;

		int m_unused;							// +0x1D4 never initialised or accessed
		ListBox* m_userList;					// +0x1D8 the "userList" list box (not reference counted)
		int m_maxUsers;							// +0x1DC visible rows of the list, set by init, never read
	};
}

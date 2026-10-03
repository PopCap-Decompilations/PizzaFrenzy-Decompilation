// MainMenuScreen: the main menu (Lorenzo's welcome text and button tips, edit-pizza lock, game link).
#pragma once

#include <string>

#include "engine/RefPtr.h"
#include "engine/Screen.h"

namespace engine
{
	class Component;
	class Container;
	class TextTyper;
}

// res/screenLayouts/mainMenu.xml (game+0x98): Lorenzo types a welcome text, or a tip while the mouse is over one of
// the new speed/memory/concentration game buttons; the edit-pizza lock; the optional cross-promotion "gameLink"
// button. The members end at +0x1E4; the vtordisp (+0x1E4) and the Interface subobject (+0x1E8) follow them
// (size 0x1EC).
class MainMenuScreen : public engine::Screen
{
public:
	MainMenuScreen();
	virtual ~MainMenuScreen();

	virtual void init();											// slot 95
	virtual void refresh();											// slot 96
	virtual void setGameLink(const std::string& imageBase);			// slot 97

	virtual void activate();										// slot 34 (engine::Component)
	virtual void onMouseEnter(std::string command);					// slot 0 of engine::ButtonListener (+0x144)
	virtual void onMouseLeave(std::string command);					// slot 1 of engine::ButtonListener (+0x144)

	void showWelcomeText();
	void updateEditPizzaLock();

	engine::RefPtr<engine::Container> m_userProperties;					// +0x1D4 "userProperties": holds the welcome typer
	engine::RefPtr<engine::Component> m_lorenzoBubble;					// +0x1D8 "lorenzoBubble" (only looked up)
	engine::RefPtr<engine::TextTyper> m_welcomeText;					// +0x1DC replaces layout text "welcomeUser"
	bool m_showingTip;													// +0x1E0 a button tip replaced the welcome text (not set by
																		//        the ctor)
};

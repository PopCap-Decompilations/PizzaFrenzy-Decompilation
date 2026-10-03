// PizzaOptionsScreen: the game's options screen (the engine's options screen plus the story checkbox).
#pragma once

#include <string>

#include "engine/OptionsScreen.h"

class GameSettings;
class PizzaFrenzy;

// res/screenLayouts/optionMenu.xml (game+0x9C): the engine options screen plus the story checkbox
// ("storyOptionCheck", action "toggleStory"). Adds no members (0x1E4 bytes, as engine::OptionsScreen). The
// constructor is inline (its copy was emitted where the game creates the screen, in PizzaFrenzy.cpp); the
// destructor is implicit (0x401710 stores no vtables).
class PizzaOptionsScreen : public engine::OptionsScreen
{
public:
	// 0x4010D0
	PizzaOptionsScreen()
	{
	}

	virtual void setup(PizzaFrenzy* game, GameSettings* settings);	// slot 97: OptionsScreen::init plus the story box

	virtual void handleAction(const std::string& action);			// slot 96 (engine::OptionsScreen)
};

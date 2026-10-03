#include "GameOptionsScreen.h"

#include <string>

#include "engine/Checkbox.h"
#include "GameSettings.h"
#include "PizzaFrenzy.h"

// 0x43C2B0
void PizzaOptionsScreen::setup(PizzaFrenzy* game, GameSettings* settings)
{
	engine::OptionsScreen::init(game, settings);
	engine::Checkbox* storyCheck = (engine::Checkbox*)getComponent("storyOptionCheck");
	if (storyCheck)
		storyCheck->setChecked(settings->getReplayStory());
}

// 0x43C370
void PizzaOptionsScreen::handleAction(const std::string& action)
{
	if (action == "toggleStory")
	{
		engine::Checkbox* storyCheck = (engine::Checkbox*)getComponent("storyOptionCheck");
		if (storyCheck)
		{
			((GameSettings*)m_settings)->setReplayStory(storyCheck->isChecked());
			m_settings->save();
		}
	}
	else
	{
		engine::OptionsScreen::handleAction(action);
	}
}

#include "OptionsScreen.h"

#include <string>

#include "Application.h"
#include "Checkbox.h"
#include "GameBase.h"
#include "Settings.h"
#include "StringUtil.h"
#include "TextItem.h"

namespace engine
{
	// 0x466510
	OptionsScreen::~OptionsScreen()
	{
	}

	// 0x466570
	OptionsScreen::OptionsScreen()
	{
	}

	// 0x466690
	void OptionsScreen::handleAction(const std::string& action)
	{
		if (action == "toggleSound")
		{
			Checkbox* check = static_cast<Checkbox*>(getComponent("soundOptionCheck"));
			if (check)
			{
				m_settings->setSoundEnabled(check->isChecked());
				m_game->setSoundEnabled(m_settings->isSoundEnabled());
				m_settings->save();
			}
		}
		else if (action == "toggleMusic")
		{
			Checkbox* check = static_cast<Checkbox*>(getComponent("musicOptionCheck"));
			if (check)
			{
				m_settings->setMusicEnabled(check->isChecked());
				m_game->setMusicEnabled(m_settings->isMusicEnabled());
				m_settings->save();
			}
		}
		else if (action == "toggleFullScreen")
		{
			Checkbox* check = static_cast<Checkbox*>(getComponent("fullScreenOptionCheck"));
			if (check)
			{
				m_settings->setFullScreen(check->isChecked());
				getApplication()->setFullScreen(m_settings->isFullScreen());
				m_settings->save();
			}
		}
	}

	// 0x4669D0
	void OptionsScreen::init(GameBase* game, Settings* settings)
	{
		Checkbox* soundCheck = static_cast<Checkbox*>(getComponent("soundOptionCheck"));
		if (!game->isSoundAvailable())
		{
			if (soundCheck)
			{
				soundCheck->setEnabled(false);
				soundCheck->setChecked(false);
				soundCheck->setAlpha(0.8f);
			}
		}
		else if (soundCheck)
		{
			soundCheck->setEnabled(true);
			soundCheck->setAlpha(1.0f);
			soundCheck->setChecked(settings->isSoundEnabled());
		}
		Checkbox* musicCheck = static_cast<Checkbox*>(getComponent("musicOptionCheck"));
		if (!game->isMusicAvailable())
		{
			if (musicCheck)
			{
				musicCheck->setEnabled(false);
				musicCheck->setChecked(false);
				musicCheck->setAlpha(0.8f);
			}
		}
		else if (musicCheck)
		{
			musicCheck->setEnabled(true);
			musicCheck->setAlpha(1.0f);
			musicCheck->setChecked(settings->isMusicEnabled());
		}
		Checkbox* fullScreenCheck = static_cast<Checkbox*>(getComponent("fullScreenOptionCheck"));
		if (fullScreenCheck)
		{
			fullScreenCheck->setChecked(getApplication()->isFullScreen());
			settings->setFullScreen(getApplication()->isFullScreen());
		}
		m_actionSignal.connect(this, &OptionsScreen::handleAction);
		m_settings = settings;
		m_game = game;
		TextItem* versionText = static_cast<TextItem*>(getComponent("VersionText"));
		if (versionText)
		{
			std::string text;
			format(text, "Version %s", getApplication()->getVersion().c_str());
			versionText->setText(text);
		}
	}
}

#include "MainMenuScreen.h"

#include <string>

#include "engine/Application.h"
#include "engine/Container.h"
#include "engine/ImageButton.h"
#include "engine/StringUtil.h"
#include "engine/Surface.h"
#include "engine/TextTyper.h"
#include "engine/User.h"
#include "engine/UserManager.h"
#include "Constants.h"
#include "PizzaFrenzy.h"
#include "UserProgress.h"

// 0x43A5F0
void MainMenuScreen::activate()
{
	engine::Screen::activate();
	refresh();
}

// 0x43A610
MainMenuScreen::~MainMenuScreen()
{
}

// 0x43A730
MainMenuScreen::MainMenuScreen()
{
}

// 0x43A800
void MainMenuScreen::showWelcomeText()
{
	engine::User* user = engine::UserManager::getInstance()->getCurrentUser();
	if (user)
	{
		engine::Application* app = engine::getApplication();
		m_welcomeText->setText(app->formatString(204, user->getName().c_str()));
	}
	else
	{
		m_welcomeText->setText(engine::getApplication()->loadString(104));
	}
	m_welcomeText->reset();
	m_welcomeText->start();
	m_showingTip = false;
}

// 0x43A940
void MainMenuScreen::onMouseLeave(std::string command)
{
	m_welcomeText->setPaging(0, 0.0f);
	m_welcomeText->setup(320.0f, 2.0f * g_typerSpeed, g_typerDelay);
	if (m_showingTip)
		showWelcomeText();
}

// 0x43A9E0
void MainMenuScreen::init()
{
	m_userProperties = (engine::Container*)getComponent("userProperties");
	m_lorenzoBubble = getComponent("lorenzoBubble");

	engine::TextItem* welcomeUser = (engine::TextItem*)getComponent("welcomeUser");
	m_welcomeText = new engine::TextTyper();
	m_welcomeText->setPosition(welcomeUser->getPosition());
	m_welcomeText->setFont(welcomeUser->getFont());
	m_welcomeText->setup(320.0f, 2.0f * g_typerSpeed, g_typerDelay);
	m_welcomeText->setColor(welcomeUser->getColor());
	m_welcomeText->setColorMode(welcomeUser->getColorMode());
	m_welcomeText->setIndent(0.0f, 0.0f);
	welcomeUser->setText("");
	m_userProperties->addChild(m_welcomeText);

	engine::Component* editButton = getComponent("EditButton");
	editButton->setVisible(false);
	engine::Component* editBtn = getComponent("EditBtn");
	editBtn->setEnabled(false);
	m_showingTip = false;
}

// 0x43ADB0
void MainMenuScreen::onMouseEnter(std::string command)
{
	std::string tip;
	if (command == "newSpeedGame")
	{
		tip = engine::getApplication()->loadString(220);
	}
	else if (command == "newMemoryGame")
	{
		tip = engine::getApplication()->loadString(221);
	}
	else if (command == "newConcentrationGame")
	{
		tip = engine::getApplication()->loadString(222);
	}
	else if (command == "sproutLogo")
	{
		tip = engine::getApplication()->loadString(232);
		m_welcomeText->setPaging(3, 0.0f);
		m_welcomeText->setup(320.0f, g_typerSpeed, 2.0f);
	}
	if (!tip.empty())
	{
		m_welcomeText->setText(tip);
		m_welcomeText->reset();
		m_welcomeText->start();
		m_showingTip = true;
	}
}

// 0x43B000
void MainMenuScreen::updateEditPizzaLock()
{
	int toppings = PizzaFrenzy::getProfile()->m_toppingLevels.size();
	engine::Component* locked = getComponent("editPizzaLocked");
	locked->setVisible(toppings < g_pizzaEditorUnlock && !g_unlockAll);
}

// 0x43B0C0
void MainMenuScreen::setGameLink(const std::string& imageBase)
{
	engine::Component* linkGroup = getComponent("gameLinkGrp");
	engine::Component* noLinkGroup = getComponent("noGameLinkGrp");
	engine::ImageButton* linkButton = (engine::ImageButton*)getComponent("gameLinkBtn");
	if (imageBase.empty())
	{
		linkGroup->setVisible(false);
		noLinkGroup->setVisible(true);
		if (linkButton)
			linkButton->setEnabled(false);
	}
	else
	{
		linkGroup->setVisible(true);
		noLinkGroup->setVisible(false);
		linkButton->setEnabled(true);

		std::string path;
		engine::Bitmap* normal = 0;
		engine::Bitmap* high = 0;
		engine::format(path, "%sNormal.png", imageBase.c_str());
		if (engine::getApplication()->nativeFileExists(path))
			normal = engine::getApplication()->getDiskImage(path.c_str());
		engine::format(path, "%sHigh.png", imageBase.c_str());
		if (engine::getApplication()->nativeFileExists(path))
			high = engine::getApplication()->getDiskImage(path.c_str());
		if (!normal)
		{
			linkGroup->setVisible(false);
			noLinkGroup->setVisible(true);
			linkButton->setEnabled(false);
		}
		else
		{
			linkButton->setImages(normal, high, 0);
			linkButton->activate();
		}
	}
}

// 0x43B3D0
void MainMenuScreen::refresh()
{
	showWelcomeText();
	updateEditPizzaLock();
}

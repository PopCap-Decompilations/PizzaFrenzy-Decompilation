// CitySelectScreen: the city select screen (a postcard button per visited city, locks, Lisa's hint).
#include <stdio.h>
#include <string>
#include <vector>

#include "CitySelectScreen.h"
#include "engine/Application.h"
#include "engine/Color.h"
#include "engine/Container.h"
#include "engine/Graphics.h"
#include "engine/Image.h"
#include "engine/ImageButton.h"
#include "engine/Point.h"
#include "engine/StringUtil.h"
#include "engine/Surface.h"
#include "engine/TextItem.h"
#include "engine/TextTyper.h"
#include "Constants.h"
#include "GameScreen.h"
#include "HudScreen.h"
#include "Level.h"
#include "PizzaFrenzy.h"
#include "UserProgress.h"

// 0x4332D0
void CitySelectScreen::onMouseEnter(std::string command)
{
	// The original leaves index uninitialised: push ecx (0x4332E5) reserves its slot, so when the name doesn't
	// parse (the Cancel button, "citySelectCancel") it still holds `this`, a pointer out of range, and nothing
	// happens. -1 behaves the same without reading an uninitialised variable (0 raised postcard 0).
	int index = -1;
	sscanf(command.c_str(), "citySelect%d", &index);
	if (index >= 0 && index < (int)m_postcardImages.size())
	{
		engine::ImageButton* button = m_postcardButtons[index];
		m_postcardLayer->removeChild(button);
		m_topLayer->addChild(button);
		m_hoveredButton = button;
	}
}

// 0x4333A0
void CitySelectScreen::onMouseLeave(std::string command)
{
	int index = -1;		// uninitialised in the original, `this` in practice (0x4333B5): see onMouseEnter
	sscanf(command.c_str(), "citySelect%d", &index);
	if (index >= 0 && index < (int)m_postcardImages.size())
	{
		engine::ImageButton* button = m_postcardButtons[index];
		m_topLayer->removeChild(button);
		m_postcardLayer->addChild(button);
		m_hoveredButton = NULL;
	}
}

// 0x433480
void CitySelectScreen::updateLocks()
{
	if (m_hoveredButton)
	{
		m_postcardLayer->addChild(m_hoveredButton);
		m_hoveredButton = NULL;
	}
	m_topLayer->removeAllChildren();
	int maxCity = getGame()->getProfile()->getMaxCity();
	engine::Bitmap* lockImage = engine::getApplication()->getImage("res\\menuAssets\\lockedOverlay.jpg");
	for (int i = 0; i < (int)m_postcardButtons.size(); ++i)
	{
		engine::ImageButton* button = m_postcardButtons[i];
		if (i > maxCity && !g_unlockAll)
		{
			engine::Image* lock = new engine::Image(lockImage);
			lock->setPosition(button->getPosition().x - 52.0f, button->getPosition().y - 47.0f);
			m_topLayer->addChild(lock);
			button->setEnabled(false);
		}
		else
		{
			button->setEnabled(true);
		}
	}
}

// 0x433620
engine::Bitmap* CitySelectScreen::getPostcard(int index) const
{
	if (index >= 0 && index < (int)m_postcardImages.size())
		return m_postcardImages[index];
	return NULL;
}

// 0x433690: the returned image carries one reference for the caller
engine::Bitmap* CitySelectScreen::createPostcard(City* city)
{
	// the city's first level that is not a bonus level (or its last level)
	Level* level = NULL;
	if (city != NULL && !city->m_levels.empty())
	{
		std::vector<engine::RefPtr<Level> >::iterator it = city->m_levels.begin();
		level = *it;
		while (level->m_bonus && ++it != city->m_levels.end())
			level = *it;
	}

	engine::RefPtr<engine::Bitmap> snapshot = engine::getApplication()->createBlankImage(700, 600);
	engine::RefPtr<engine::Graphics> snapshotGraphics = engine::getApplication()->createGraphics(snapshot);
	if (level != NULL)
	{
		GameScreen* gameScreen = PizzaFrenzy::getGameScreen();
		getGame()->loadCity(level->getCityFile(), gameScreen);
		gameScreen->setAlpha(1.0f);
		gameScreen->setupGraphics(*snapshotGraphics);
		gameScreen->draw(*snapshotGraphics);
	}
	else
	{
		snapshotGraphics->setColor(0.24f, 0.33f, 0.34f);
		snapshotGraphics->drawRect(700.0f, 600.0f);
	}

	engine::Bitmap* frame = engine::getApplication()->getImage("res\\menuAssets\\postcardFrame.jpg");
	engine::Bitmap* postcard = engine::getApplication()->createBlankImage(frame->getWidth(), frame->getHeight());
	postcard->addRef();
	engine::RefPtr<engine::Graphics> g = engine::getApplication()->createGraphics(postcard);
	postcard->fill(engine::Color(0.0f, 0.0f, 0.0f, 0.0f));
	postcard->setAlphaType(2, 255);

	engine::RefPtr<engine::Image> snapshotImage = new engine::Image(snapshot);
	snapshotImage->setBlendMode(1);
	snapshotImage->setScale(2.0f / 7.0f);
	snapshotImage->setPosition(7.0f, 8.0f);
	g->pushState();
	snapshotImage->setupGraphics(*g);
	snapshotImage->draw(*g);
	g->popState();

	engine::Image frameImage(frame);
	g->pushState();
	frameImage.setupGraphics(*g);
	frameImage.draw(*g);
	g->popState();

	engine::Bitmap* caption = engine::getApplication()->getImage("res\\menuAssets\\toppingUpgradeCaption.jpg");
	caption->setPivotType(1);
	engine::Image captionImage(caption);
	captionImage.setPosition(109.0f, 136.0f);
	captionImage.setScale(0.9f);
	g->pushState();
	captionImage.setupGraphics(*g);
	captionImage.draw(*g);
	g->popState();

	engine::TextItem nameText;
	nameText.setXAlign(1);
	nameText.setYAlign(1);
	nameText.setFont(engine::getApplication()->getFont("res\\fonts\\buttonFont.xml"));
	nameText.setColor(0.0f, 0.0f, 0.0f);
	nameText.setColorMode(2);
	if (city != NULL)
		nameText.setText(city->getName());
	else
		nameText.setText("???");
	nameText.setPosition(109.0f, 136.0f);
	g->pushState();
	nameText.setupGraphics(*g);
	nameText.draw(*g);
	g->popState();
	return postcard;
}

// 0x433C30
void CitySelectScreen::activate()
{
	engine::Screen::activate();
	m_lisaText->setText("These are postcards from the cities you've already visited. Click the city where you'd like to start.");
	m_lisaText->start();
}

// 0x433D50
CitySelectScreen::~CitySelectScreen()
{
}

// 0x434210
CitySelectScreen::CitySelectScreen()
{
}

// 0x434300
void CitySelectScreen::init(const std::vector<engine::RefPtr<City> >& cities)
{
	engine::Container* group = (engine::Container*)getComponent("postcardGroup");
	m_postcardLayer = new engine::Container();
	group->addChild(m_postcardLayer);
	m_lisa = (engine::Container*)getComponent("Lisa");
	m_lisaBubble = getComponent("LisaBubble");
	engine::TextItem* lisaText = (engine::TextItem*)getComponent("lisaText");
	m_lisaText = new engine::TextTyper();
	m_lisaText->setPosition(lisaText->getPosition());
	m_lisaText->setFont(lisaText->getFont());
	m_lisaText->setup(g_typerWidth, g_typerSpeed, g_typerDelay);
	m_lisaText->setColor(lisaText->getColor());
	m_lisaText->setColorMode(lisaText->getColorMode());
	m_lisaText->setIndent(0.0f, 0.0f);
	lisaText->setText("");
	m_lisa->addChild(m_lisaText);
	setCities(cities);
	m_topLayer = new engine::Container();
	group->addChild(m_topLayer);
	m_hoveredButton = NULL;
}

// 0x434790
void CitySelectScreen::setCities(const std::vector<engine::RefPtr<City> >& cities)
{
	// the snapshots are rendered through the game screen without the HUD
	PizzaFrenzy::getHud()->setVisible(false);
	PizzaFrenzy::getGameScreen()->m_layout->setVisible(false);
	for (std::vector<engine::RefPtr<City> >::const_iterator it = cities.begin(); it != cities.end(); ++it)
	{
		engine::Bitmap* postcard = createPostcard(*it);
		m_postcardImages.push_back(postcard);
		postcard->release();
	}
	m_unknownPostcard = createPostcard(NULL);
	PizzaFrenzy::getHud()->setVisible(true);
	PizzaFrenzy::getGameScreen()->m_layout->setVisible(true);

	for (int i = 0; i < (int)m_postcardImages.size(); ++i)
	{
		engine::Bitmap* image = m_postcardImages[i];
		int row = i / 4;
		int column = i % 4;
		engine::ImageButton* button = new engine::ImageButton();
		button->setHotspotMode(1);
		button->setImages(image, image, image);
		button->setPosition(column * 122.0f, row * 105.0f);
		button->setHoverScale(engine::Vector2(1.0f, 1.0f));
		button->setNormalScale(engine::Vector2(0.5f, 0.5f));
		button->setScaleDuration(0.2f);
		std::string command;
		engine::format(command, "citySelect%d", i);
		addButton(button, "", command);
		m_postcardButtons.push_back(button);
		m_postcardLayer->addChild(button);
	}
}

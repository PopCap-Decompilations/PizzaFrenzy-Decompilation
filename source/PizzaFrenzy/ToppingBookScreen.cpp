#include "ToppingBookScreen.h"

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include "engine/Application.h"
#include "engine/Color.h"
#include "engine/Container.h"
#include "engine/Font.h"
#include "engine/Graphics.h"
#include "engine/HsvFilter.h"
#include "engine/Image.h"
#include "engine/StringUtil.h"
#include "engine/Surface.h"
#include "engine/TextItem.h"
#include "Constants.h"
#include "GameLogic.h"
#include "PizzaFrenzy.h"
#include "TileManager.h"
#include "UserProgress.h"

// 0x443BB0 (folded)
void ToppingBookScreen::activate()
{
	engine::Screen::activate();
}

// 0x443D80
void ToppingBookScreen::onButton(const std::string& name)
{
	if (name == "bookLeft")
	{
		int page = m_page - 2;
		if (page >= 0)
			m_page = page;
		showPages();
	}
	else if (name == "bookRight")
	{
		int count = (int)PizzaFrenzy::getTileManifest()->m_toppingList.size();
		int page = m_page + 2;
		if (page <= count)
			m_page = page;
		showPages();
	}
	else
	{
		getGame()->onScreenEvent(name);
	}
}

// 0x443E30
void ToppingBookScreen::showTitles()
{
	m_leftTitles->removeAllChildren();
	engine::Font* font = engine::getApplication()->getFont("res\\fonts\\bookFont.xml");
	int y = 0;
	Title* last = 0;
	int points = 0;
	for (std::map<Topping*, int>::iterator it = PizzaFrenzy::getProfile()->m_toppingLevels.begin();
		it != PizzaFrenzy::getProfile()->m_toppingLevels.end(); ++it)
	{
		if (!it->first->m_isPizza)
			points += it->second + 1;
	}
	for (std::vector<engine::RefPtr<Title> >::iterator it = PizzaFrenzy::getTileManifest()->m_titles.begin();
		it != PizzaFrenzy::getTileManifest()->m_titles.end(); ++it)
	{
		Title* title = *it;
		if (title->m_points > points)
			break;
		if (last)
		{
			// the titles already passed, struck through
			engine::TextItem* text = new engine::TextItem();
			text->setFont(font);
			text->setXAlign(1);
			text->setText(last->m_text);
			text->m_style |= 2;
			text->setColor(0.0f, 0.0f, 0.0f);
			text->setColorMode(2);
			text->setPosition(0.0f, (float)y);
			m_leftTitles->addChild(text);
			y += font->getLineHeight() - 2;
		}
		last = title;
	}
	if (last)
	{
		// the current title, in red
		engine::TextItem* text = new engine::TextItem();
		text->setFont(font);
		text->setXAlign(1);
		text->setText(last->m_text);
		text->setColor(1.0f, 0.0f, 0.0f);
		text->setColorMode(2);
		text->setPosition(0.0f, (float)y);
		m_leftTitles->addChild(text);
	}
}

// 0x4446C0
void ToppingBookScreen::showPages()
{
	std::vector<engine::RefPtr<Topping> >& toppings = PizzaFrenzy::getTileManifest()->m_toppingList;
	UserProgress* profile = PizzaFrenzy::getProfile();
	int left = m_page;
	if (left < 1)
	{
		m_pages[0]->setVisible(false);
		showTitles();
	}
	else
	{
		m_leftTitles->removeAllChildren();
		Topping* topping = toppings.at(left - 1);
		showTopping(topping, profile->getToppingLevel(topping), 0, left);
	}
	int right = m_page + 1;
	if (right > (int)toppings.size())
	{
		m_pages[1]->setVisible(false);
	}
	else
	{
		Topping* topping = toppings.at(right - 1);
		showTopping(topping, profile->getToppingLevel(topping), 1, right);
	}
}

// 0x444AD0
ToppingBookScreen::~ToppingBookScreen()
{
}

// 0x444DB0
void ToppingBookScreen::init()
{
	m_leftTitles = static_cast<engine::Container*>(getComponent("leftTitles"));
	for (int i = 0; i < 2; i++)
	{
		std::string name;
		engine::format(name, "%sPage", i == 0 ? "left" : "right");
		m_pages[i] = getComponent(name);
		engine::format(name, "%sToppingBg", i == 0 ? "left" : "right");
		m_toppingBgs[i] = static_cast<engine::Image*>(getComponent(name));
		engine::format(name, "%sLocked", i == 0 ? "left" : "right");
		m_lockedStamps[i] = getComponent(name);
		engine::format(name, "%sTopping", i == 0 ? "left" : "right");
		m_toppingImages[i] = static_cast<engine::Image*>(getComponent(name));
		engine::format(name, "%sToppingCaptionText", i == 0 ? "left" : "right");
		m_captionTexts[i] = static_cast<engine::TextItem*>(getComponent(name));
		engine::format(name, "%sToppingLevel", i == 0 ? "left" : "right");
		m_levelTexts[i] = static_cast<engine::TextItem*>(getComponent(name));
		engine::format(name, "%sToppingValue", i == 0 ? "left" : "right");
		m_valueTexts[i] = static_cast<engine::TextItem*>(getComponent(name));
		engine::format(name, "%sToppingNext", i == 0 ? "left" : "right");
		m_nextTexts[i] = static_cast<engine::TextItem*>(getComponent(name));
		engine::format(name, "%sToppingDesc", i == 0 ? "left" : "right");
		m_descTexts[i] = static_cast<engine::TextItem*>(getComponent(name));
		engine::format(name, "%sToppingPage", i == 0 ? "left" : "right");
		m_pageTexts[i] = static_cast<engine::TextItem*>(getComponent(name));
		engine::format(name, "%sButton", i == 0 ? "left" : "right");
		m_pageButtons[i] = getComponent(name);
	}
	m_levelFormat = m_levelTexts[0]->getText();
	m_valueFormat = m_valueTexts[0]->getText();
	m_pageFormat = m_pageTexts[0]->getText();
	m_page = 0;
	showPages();
	m_actionSignal.connect(this, &ToppingBookScreen::onButton);

	// a black copy of every topping's image, shown for the toppings the player has not found
	m_silhouetteFilter = new engine::HsvFilter();
	m_silhouetteFilter->set(0.0f, -1.0f, -1.0f);
	TileManager* manifest = PizzaFrenzy::getTileManifest();
	for (std::vector<engine::RefPtr<Topping> >::iterator it = manifest->m_toppingList.begin();
		it != manifest->m_toppingList.end(); ++it)
	{
		Topping* topping = *it;
		engine::Bitmap* image = topping->m_image;
		image->setPivotType(0);
		engine::Bitmap* silhouette = engine::getApplication()->createBlankImage(image->getWidth(), image->getHeight());
		m_silhouettes[topping] = silhouette;
		engine::RefPtr<engine::Graphics> graphics = engine::getApplication()->createGraphics(silhouette);
		silhouette->fill(engine::Color(0.0f, 0.0f, 0.0f, 0.0f));
		silhouette->setAlphaType(2, 255);
		graphics->drawImage(image);
		silhouette->applyFilter(m_silhouetteFilter);
		image->setPivotType(1);
		silhouette->setPivotType(1);
	}
}

// 0x4455D0
void ToppingBookScreen::showTopping(Topping* topping, int level, int side, int pageNumber)
{
	m_pages[side]->setVisible(true);
	engine::Bitmap* background = m_toppingBgs[side]->getImage();
	m_toppingBgs[side]->setImage(topping->m_couponImage->copyScaledUniform(
		(float)(background->getWidth() / topping->m_couponImage->getWidth())));
	std::string text;
	int count = (int)PizzaFrenzy::getTileManifest()->m_toppingList.size();
	engine::format(text, m_pageFormat.c_str(), pageNumber, count);
	m_pageTexts[side]->setText(text);
	if ((pageNumber <= 0 && side == 0) || (pageNumber >= count && side == 1))
	{
		m_pageButtons[side]->setVisible(false);
		m_pageButtons[side]->setEnabled(false);
	}
	else
	{
		m_pageButtons[side]->setVisible(true);
		m_pageButtons[side]->setEnabled(true);
	}
	UserProgress* profile = PizzaFrenzy::getProfile();
	bool isNew = false;
	bool known = true;
	if (level < 0)
	{
		// not owned: a newly unlocked topping is shown at its first level with the "locked" stamp, any other one
		// as a silhouette
		isNew = std::find(profile->getNewToppings().begin(), profile->getNewToppings().end(), topping)
			!= profile->getNewToppings().end();
		if (isNew)
			level = 0;
		else
			known = false;
	}
	if (known && level < (int)topping->m_upgrades.size())
	{
		m_toppingImages[side]->setImage(topping->m_image);
		ToppingUpgrade* upgrade = topping->m_upgrades.at(level);
		m_captionTexts[side]->setText(upgrade->m_name);
		engine::format(text, m_levelFormat.c_str(), level + 1);
		m_levelTexts[side]->setText(text);
		engine::format(text, m_valueFormat.c_str(),
			upgrade->m_bonusMultiplier * (topping->m_isPizza ? g_premiumToppingPrice : g_toppingPrice));
		m_valueTexts[side]->setText(text);
		m_nextTexts[side]->setText((int)topping->m_upgrades.size() <= level + 1 ? "N/A"
			: getGame()->getGameLogic()->getFrenzyText(topping->m_upgrades[level + 1]->m_combo));
		m_descTexts[side]->setText(upgrade->m_description);
		engine::format(text, m_pageFormat.c_str(), pageNumber, PizzaFrenzy::getTileManifest()->m_toppingList.size());
		m_pageTexts[side]->setText(text);
	}
	else
	{
		m_toppingImages[side]->setImage(m_silhouettes[topping]);
		m_captionTexts[side]->setText(engine::getApplication()->loadString(233));
		engine::format(text, m_levelFormat.c_str(), 0);
		m_levelTexts[side]->setText(text);
		engine::format(text, m_valueFormat.c_str(), 0);
		m_valueTexts[side]->setText(text);
		m_nextTexts[side]->setText("N/A");
		m_descTexts[side]->setText("");
	}
	m_lockedStamps[side]->setVisible(isNew);
}

// 0x445B20
ToppingBookScreen::ToppingBookScreen()
{
}

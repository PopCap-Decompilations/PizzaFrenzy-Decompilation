#include "ToppingSelectionScreen.h"

#include <algorithm>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>

#include "engine/Application.h"
#include "engine/Container.h"
#include "engine/FadeContainer.h"
#include "engine/Image.h"
#include "engine/ImageButton.h"
#include "engine/StringUtil.h"
#include "engine/TextItem.h"
#include "GameProgress.h"
#include "Level.h"
#include "OrderButton.h"
#include "PizzaFrenzy.h"
#include "TileManager.h"
#include "UserProgress.h"

// 0x443BB0 (folded)
void ToppingSelectionScreen::activate()
{
	engine::Screen::activate();
}

// 0x445DB0
void ToppingSelectionScreen::onRightClick(const engine::Point& pos)
{
	dropTopping();
}

// 0x445ED0
void ToppingSelectionScreen::onMouseMove(const engine::Point& pos)
{
	m_cursorImage->setPosition(engine::Vector2(pos));
}

// 0x446010
void ToppingSelectionScreen::pickUpTopping(Topping* topping)
{
	if (m_heldTopping)
		enableToppingButton(m_heldTopping, true);
	enableToppingButton(topping, false);
	m_heldTopping = topping;
	m_cursorImage->setImage(topping->m_smallImage);
	for (std::vector<engine::RefPtr<engine::ImageButton> >::iterator it = m_vanButtons.begin();
		it != m_vanButtons.end(); ++it)
	{
		(*it)->setEnabled(true);
	}
}

// 0x4460B0
void ToppingSelectionScreen::dropTopping()
{
	if (m_heldTopping)
	{
		enableToppingButton(m_heldTopping, true);
		m_heldTopping = 0;
		m_cursorImage->setImage(0);
		for (std::vector<engine::RefPtr<engine::ImageButton> >::iterator it = m_vanButtons.begin();
			it != m_vanButtons.end(); ++it)
		{
			(*it)->setEnabled(false);
		}
	}
}

// 0x446130
void ToppingSelectionScreen::updateDoneButton()
{
	bool complete = true;
	for (std::vector<engine::RefPtr<Topping> >::iterator it = m_vanToppings.begin(); it != m_vanToppings.end(); ++it)
	{
		if (*it == 0)
		{
			complete = false;
			break;
		}
	}
	if (complete)
	{
		m_doneButton->setEnabled(true);
		m_doneButton->setColor(0.0f, 0.0f, 0.0f);
	}
	else
	{
		m_doneButton->setEnabled(false);
		m_doneButton->setColor(-0.2f, -0.2f, -0.2f);
		m_doneButton->setColorMode(1);
	}
}

// 0x4461B0
void ToppingSelectionScreen::onMouseLeave(std::string command)
{
	m_caption->fade(false, 0.1f, 1.0f, false);
}

// 0x446210
void ToppingSelectionScreen::showPage(int side, int page)
{
	GameProgress* stats = PizzaFrenzy::getGameStats();
	int count = (int)stats->m_toppingLevels.size();
	m_pages[side]->setVisible(true);
	std::string text;
	int pages = count / 12;
	if (count % 12 > 0)
		pages++;
	engine::format(text, m_pageFormat.c_str(), page + 1, pages);
	m_pageTexts[side]->setText(text);
	if ((page <= 0 && side == 0) || (page >= pages && side == 1))
	{
		m_pageButtons[side]->setVisible(false);
		m_pageButtons[side]->setEnabled(false);
	}
	else
	{
		m_pageButtons[side]->setVisible(true);
		m_pageButtons[side]->setEnabled(true);
	}
	m_buttonPanels[side]->removeAllChildren();

	// twelve toppings per page, in a grid of three columns
	std::map<Topping*, int>::iterator it = stats->m_toppingLevels.begin();
	int first = page * 12;
	for (int i = 0; i < first && first < count; i++)
		++it;
	int shown = 0;
	int index = first;
	while (shown < 12 && index < count)
	{
		Topping* topping = it->first;
		if (topping)
		{
			OrderButton* button = createToppingButton(topping, it->second);
			button->setPosition((float)(shown % 3 * 67), (float)(shown / 3 * 66));
			m_buttonPanels[side]->addChild(button);
			if (std::find(m_vanToppings.begin(), m_vanToppings.end(), topping) != m_vanToppings.end())
				enableToppingButton(topping, false);
			shown++;
			index++;
		}
		++it;
	}
}

// 0x4464D0
void ToppingSelectionScreen::showPages()
{
	dropTopping();
	m_caption->setVisible(false);
	GameProgress* stats = PizzaFrenzy::getGameStats();
	m_toppingButtons.clear();
	showPage(0, m_page);
	if ((int)stats->m_toppingLevels.size() <= (m_page + 1) * 12)
		m_pages[1]->setVisible(false);
	else
		showPage(1, m_page + 1);
}

// 0x446F30
void ToppingSelectionScreen::init()
{
	for (int i = 0; i < 2; i++)
	{
		std::string name;
		engine::format(name, "%sPage", i == 0 ? "left" : "right");
		m_pages[i] = getComponent(name);
	}
	for (int i = 0; i < 2; i++)
	{
		std::string name;
		engine::format(name, "%sButtons", i == 0 ? "left" : "right");
		m_buttonPanels[i] = static_cast<engine::Container*>(getComponent(name));
	}
	for (int i = 0; i < 2; i++)
	{
		std::string name;
		engine::format(name, "%sToppingPage", i == 0 ? "left" : "right");
		m_pageTexts[i] = static_cast<engine::TextItem*>(getComponent(name));
	}
	m_pageFormat = m_pageTexts[0]->getText();
	for (int i = 0; i < 2; i++)
	{
		std::string name;
		engine::format(name, "%sButton", i == 0 ? "left" : "right");
		m_pageButtons[i] = getComponent(name);
	}
	m_vanButtonPanel = static_cast<engine::Container*>(getComponent("vanButtons"));
	m_page = 0;
	m_cursorImage = new engine::Image(0);
	addChild(m_cursorImage);
	m_actionSignal.connect(this, &ToppingSelectionScreen::onButton);
	engine::getApplication()->m_mouseMoveSignal.connect(this, &ToppingSelectionScreen::onMouseMove);
	engine::getApplication()->m_rightMouseDownSignal.connect(this, &ToppingSelectionScreen::onRightClick);
	m_doneButton = getComponent("doneButton");
	updateDoneButton();
	m_caption = static_cast<engine::FadeContainer*>(getComponent("caption"));
	m_captionText = static_cast<engine::TextItem*>(getComponent("captionText"));
	m_caption->setVisible(false);
}

// 0x447690
ToppingSelectionScreen::~ToppingSelectionScreen()
{
}

// 0x447990
void ToppingSelectionScreen::onButton(const std::string& name)
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
		int count = (int)PizzaFrenzy::getGameStats()->m_toppingLevels.size();
		int pages = count / 12;
		if (count % 12 > 0)
			pages++;
		int page = m_page + 2;
		if (page < pages)
			m_page = page;
		showPages();
	}
	else if (name == "selectionDone")
	{
		commitSelection();
		getGame()->onScreenEvent(name);
	}
	else if (name == "menu")
	{
		getGame()->onScreenEvent(name);
	}
	else
	{
		Topping* topping = PizzaFrenzy::getTileManifest()->getTopping(name);
		if (topping)
		{
			pickUpTopping(topping);
		}
		else if (name.substr(0, 5) == "pizza")
		{
			std::string id = name.substr(5);
			pickUpTopping(getGame()->getPizzaTopping(atoi(id.c_str())));
		}
		else if (name.substr(0, 9) == "vanButton")
		{
			Topping* held = m_heldTopping;
			if (held)
			{
				std::string number = name.substr(9);
				int slot = atoi(number.c_str());
				if (slot >= 0 && slot < (int)m_vanButtons.size())
				{
					if (m_vanToppings[slot])
						enableToppingButton(m_vanToppings[slot], true);
					m_vanToppings[slot] = held;
					m_vanImages[slot]->setImage(m_heldTopping->m_smallImage);
					dropTopping();
					enableToppingButton(held, false);
					updateDoneButton();
				}
			}
		}
	}
}

// 0x447C60
void ToppingSelectionScreen::setLevel(Level* level)
{
	m_level = level;
	int slots = (int)level->getKitchens().size();
	m_vanButtonPanel->removeAllChildren();
	m_vanButtons.clear();
	m_vanImages.clear();
	m_vanToppings.clear();
	UserProgress* profile = PizzaFrenzy::getProfile();
	for (std::vector<engine::RefPtr<Topping> >::iterator it = profile->getPickedToppings().begin();
		it != profile->getPickedToppings().end(); ++it)
	{
		Topping* topping = *it;
		m_vanToppings.push_back(topping);
	}
	m_vanToppings.resize(slots);

	// one van button per kitchen, 90 pixels apart and centred, with the icon of its topping
	float x = (1 - slots) * 45.0f;
	for (int i = 0; i < slots; i++)
	{
		engine::ImageButton* button = new engine::ImageButton();
		button->setHotspotMode(1);
		button->setImages(engine::getApplication()->getImage("res\\pizza\\toppingButton.jpg"), 0, 0);
		button->setEnabled(false);
		button->setPosition(x, 0.0f);
		std::string name;
		engine::format(name, "vanButton%d", i);
		addButton(button, name, name);
		addComponent(button, name);
		button->setSounds("res\\sounds\\menu_mouseover.ogg", "res\\sounds\\menu_clicked.ogg");
		m_vanButtonPanel->addChild(button);
		m_vanButtons.push_back(button);
		engine::Image* icon = new engine::Image(0);
		if (m_vanToppings[i])
			icon->setImage(m_vanToppings[i]->m_smallImage);
		icon->setPosition(x - 9.0f, -12.0f);
		m_vanButtonPanel->addChild(icon);
		m_vanImages.push_back(icon);
		x += 90.0f;
	}
	updateDoneButton();
	m_page = 0;
	showPages();
}

// 0x448300
void ToppingSelectionScreen::commitSelection()
{
	std::vector<KitchenInfo*>::iterator it;
	for (it = m_level->getKitchens().begin(); it != m_level->getKitchens().end(); ++it)
		(*it)->topping = 0;
	it = m_level->getKitchens().begin();
	std::vector<engine::RefPtr<Topping> >::iterator slot = m_vanToppings.begin();
	for (; it != m_level->getKitchens().end(); ++it, ++slot)
		(*it)->topping = *slot;

	UserProgress* profile = PizzaFrenzy::getProfile();
	profile->getPickedToppings().clear();
	for (std::vector<engine::RefPtr<Topping> >::iterator picked = m_vanToppings.begin();
		picked != m_vanToppings.end(); ++picked)
	{
		Topping* topping = *picked;
		if (topping)
			profile->getPickedToppings().push_back(topping);
	}
	profile->save();
}

// 0x4484F0
void ToppingSelectionScreen::enableToppingButton(Topping* topping, bool enable)
{
	OrderButton* button = m_toppingButtons[topping];
	if (button)
	{
		if (!enable)
			button->disable();
		else
			button->enable();
	}
}

// 0x448530
OrderButton* ToppingSelectionScreen::createToppingButton(Topping* topping, int level)
{
	OrderButton* button = new OrderButton(topping, level + 1, this);
	PizzaFrenzy::getGameStats()->getToppingUpgrade(topping);	// result unused
	std::string text;
	engine::format(text, "%d/4", level + 1);
	button->m_countText->setText(text);
	button->setScale(0.9f);
	m_toppingButtons[topping] = button;
	return button;
}

// 0x448660
void ToppingSelectionScreen::onMouseEnter(std::string command)
{
	Topping* topping = PizzaFrenzy::getTileManifest()->getTopping(command);
	OrderButton* button = 0;
	if (topping)
	{
		button = m_toppingButtons[topping];
	}
	else if (command.substr(0, 5) == "pizza")
	{
		std::string id = command.substr(5);
		topping = getGame()->m_userPizzaCombos[atoi(id.c_str())];
		button = m_toppingButtons[topping];
	}
	if (button && topping)
	{
		m_captionText->setText(topping->m_display);
		m_caption->fade(true, 0.1f, 1.0f, false);
		m_caption->setPosition(button->getScreenPosition().x + 35.0f, button->getScreenPosition().y + 50.0f);
	}
}

// 0x448860
ToppingSelectionScreen::ToppingSelectionScreen()
{
}

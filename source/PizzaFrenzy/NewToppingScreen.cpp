#include "NewToppingScreen.h"

#include <string>
#include <vector>

#include "engine/Container.h"
#include "engine/Image.h"
#include "engine/StringUtil.h"
#include "engine/Surface.h"
#include "engine/TextItem.h"
#include "Constants.h"
#include "GameProgress.h"
#include "OrderButton.h"
#include "PizzaFrenzy.h"
#include "TileManager.h"
#include "UserProgress.h"

// 0x43B3E0
OrderButton* NewToppingScreen::createToppingButton(Topping* topping)
{
	OrderButton* button = new OrderButton(topping, -1, this);
	button->setScale(0.9f);
	return button;
}

// 0x43B500
void NewToppingScreen::populateToppings()
{
	std::vector<engine::RefPtr<Topping> >& toppings = PizzaFrenzy::getProfile()->getNewToppings();
	m_leftButtons->removeAllChildren();
	int i = 0;
	for (std::vector<engine::RefPtr<Topping> >::iterator it = toppings.begin(); it != toppings.end() && i < 15;
		++it, ++i)
	{
		OrderButton* button = createToppingButton(*it);
		button->setPosition((float)(67 * (i % 3)), (float)(64 * (i / 3)));
		m_leftButtons->addChild(button);
	}
	showTopping(0, 0);
}

// 0x43B5B0
NewToppingScreen::NewToppingScreen()
{
}

// 0x43B6F0
NewToppingScreen::~NewToppingScreen()
{
}

// 0x43B8E0
void NewToppingScreen::onMouseEnter(std::string command)
{
	Topping* topping = PizzaFrenzy::getTileManifest()->getTopping(command);
	if (topping)
		showTopping(topping, 0);
}

// 0x43B970
void NewToppingScreen::onButtonClicked(const std::string& name)
{
	Topping* topping = PizzaFrenzy::getTileManifest()->getTopping(name);
	if (topping)
	{
		PizzaFrenzy::getGameStats()->setToppingLevel(topping, 0);
		PizzaFrenzy::getProfile()->removeNewTopping(topping);
		PizzaFrenzy::getProfile()->setUnlockTopping(false);
		PizzaFrenzy::getProfile()->save();
		getGame()->onScreenEvent("toppingSelected");
	}
	else
	{
		getGame()->onScreenEvent(name);
	}
}

// 0x43BAD0
void NewToppingScreen::showTopping(Topping* topping, int level)
{
	if (!topping)
	{
		m_rightTopping->setImage(0);
		m_rightToppingCaptionText->setText("");
		m_rightToppingLevel->setText("");
		m_rightToppingValue->setText("");
		m_rightToppingDesc->setText("");
	}
	else
	{
		m_rightToppingBg->setImage(topping->m_couponImage->copyScaledUniform(2.0f));
		m_rightTopping->setImage(topping->m_image);
		ToppingUpgrade* upgrade = topping->m_upgrades.at(level);
		m_rightToppingCaptionText->setText(upgrade->m_name);
		std::string text;
		engine::format(text, m_levelFormat.c_str(), level + 1);
		m_rightToppingLevel->setText(text);
		engine::format(text, m_valueFormat.c_str(),
			(topping->m_isPizza ? g_premiumToppingPrice : g_toppingPrice) * upgrade->m_bonusMultiplier);
		m_rightToppingValue->setText(text);
		m_rightToppingDesc->setText(upgrade->m_description);
	}
}

// 0x43BE30
void NewToppingScreen::init()
{
	m_leftButtons = (engine::Container*)getComponent("leftButtons");
	m_rightToppingBg = (engine::Image*)getComponent("rightToppingBg");
	m_rightTopping = (engine::Image*)getComponent("rightTopping");
	m_rightToppingCaptionText = (engine::TextItem*)getComponent("rightToppingCaptionText");
	m_rightToppingLevel = (engine::TextItem*)getComponent("rightToppingLevel");
	m_levelFormat = m_rightToppingLevel->getText();
	m_rightToppingValue = (engine::TextItem*)getComponent("rightToppingValue");
	m_valueFormat = m_rightToppingValue->getText();
	m_rightToppingDesc = (engine::TextItem*)getComponent("rightToppingDesc");
	m_actionSignal.connect(this, &NewToppingScreen::onButtonClicked);
}

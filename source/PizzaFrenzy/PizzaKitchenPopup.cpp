// PizzaKitchenPopup and PizzaPopupState.
#include "PizzaKitchenPopup.h"

#include "engine/Application.h"
#include "engine/Container.h"
#include "engine/Image.h"
#include "engine/TextItem.h"
#include "Constants.h"
#include "GameLogic.h"
#include "GameProgress.h"
#include "PizzaFrenzy.h"
#include "PopupResetState.h"
#include "TileManager.h"

// 0x451330
PizzaKitchenPopup::PizzaKitchenPopup()
{
}

// 0x4513D0
int PizzaKitchenPopup::getPopupType() const
{
	return 1;
}

// 0x4513E0
PizzaKitchenPopup::~PizzaKitchenPopup()
{
	m_toppingIcon = 0;
	m_button = 0;
	m_priceTag = 0;
	m_priceText = 0;
}

// ---- PizzaPopupState --------------------------------------------------------------------------------------------

// 0x451580
PizzaPopupState::PizzaPopupState(PizzaPopup* popup)
	: engine::State<PizzaPopup>(popup)
{
}

// 0x451610 (folded)
bool PizzaPopupState::acceptsInput() const
{
	return true;
}

// 0x451610 (folded)
bool PizzaPopupState::isActive() const
{
	return true;
}

// ---- PizzaKitchenPopup ------------------------------------------------------------------------------------------

// 0x451650
void PizzaKitchenPopup::highlight()
{
	if (PizzaFrenzy::getGameLogic()->getSelectedCount() > 0)
	{
		PizzaPopup::highlight();
		m_state.switchState(new PopupActiveState(this));
	}
}

// 0x451710
void PizzaKitchenPopup::select()
{
	PizzaPopup::select();
	m_state.switchState(new PopupActiveState(this));
}

// 0x4517B0 (folded)
void PizzaKitchenPopup::onDispatched()
{
	PizzaPopup::onDispatched();
	m_state.switchState(new PopupResetState(this));
}

// 0x4517B0 (folded)
void PizzaKitchenPopup::deselect()
{
	PizzaPopup::deselect();
	m_state.switchState(new PopupResetState(this));
}

// 0x451850
void PizzaKitchenPopup::onShowFinished()
{
	m_state.switchState(new PopupResetState(this));
}

// 0x452050 (folded)
void PizzaKitchenPopup::onCloseFinished()
{
	remove();
}

// 0x452890 (folded)
Topping* PizzaKitchenPopup::getTopping() const
{
	return m_topping;
}

// 0x4518F0
std::string PizzaKitchenPopup::getTypeName() const
{
	return "PizzaKitchenPopup";
}

// 0x451920
void PizzaKitchenPopup::setPrice(int price)
{
	m_priceText->setNumber(price, "$");
}

// 0x4519B0
void PizzaKitchenPopup::setTopping(Topping* topping)
{
	m_topping = topping;
	m_toppingIcon->removeAllChildren();
	if (m_topping)
	{
		m_toppingIcon->addChild(new engine::Image(m_topping->m_smallImage));
		setPrice(PizzaFrenzy::getGameStats()->getToppingUpgrade(topping)->m_bonusMultiplier
			* (topping->m_isPizza ? g_premiumToppingPrice : g_toppingPrice));
	}
}

// 0x451AB0
void PizzaKitchenPopup::init(BuildingTile* owner)
{
	PizzaPopup::init(owner);

	engine::Bitmap* image = engine::getApplication()->getImage("res/pizza/toppingButton.jpg");
	image->setPivotType(1);
	m_button = new engine::Image(image);
	m_button->setPosition(-2.0f, 2.0f);
	addChild(m_button);

	m_toppingIcon = new engine::Container();
	addChild(m_toppingIcon);
	m_toppingIcon->setPosition(-10.0f, -12.0f);

	m_priceTag = new engine::Container();
	m_priceTag->setPosition(24.0f, -26.0f);
	addChild(m_priceTag);

	image = engine::getApplication()->getImage("res/pizza/pizzaMultiplierBkg.jpg");
	image->setPivotType(1);
	m_priceTag->addChild(new engine::Image(image));

	m_priceText = new engine::TextItem();
	m_priceText->setFont(engine::getApplication()->getFont("res\\fonts\\hudFont.xml"));
	m_priceText->setColor(0.0f, 0.0f, 0.0f);
	m_priceText->setColorMode(2);
	m_priceText->setXAlign(1);
	m_priceText->setYAlign(1);
	m_priceText->setBlendMode(1);
	m_priceTag->addChild(m_priceText);

	m_state.switchState(new PopupShowState(this, true));
}

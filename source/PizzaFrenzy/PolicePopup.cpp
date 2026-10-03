// PolicePopup.
#include "PolicePopup.h"

#include "engine/Application.h"
#include "engine/Image.h"
#include "BuildingTile.h"
#include "CustomerTile.h"
#include "GameLogic.h"
#include "PizzaFrenzy.h"
#include "PoliceCar.h"
#include "PopupResetState.h"

// 0x455410
void PolicePopup::sendPoliceCar(CustomerTile* criminal)
{
	PoliceCar* car = new PoliceCar();
	car->init();
	m_owner->addVehicle(m_owner->getParkingCell(), car);
	car->dispatchTo(criminal);
	criminal->onVehicleDispatched();
}

// 0x4554C0
PolicePopup::PolicePopup()
{
}

// 0x455540
int PolicePopup::getPopupType() const
{
	return 2;
}

// 0x455580
PolicePopup::~PolicePopup()
{
	m_button = 0;
}

// 0x455650
void PolicePopup::init(BuildingTile* owner)
{
	PizzaPopup::init(owner);
	engine::Bitmap* image = engine::getApplication()->getImage("res/pizza/policeButton.jpg");
	image->setPivotType(1);
	m_button = new engine::Image(image);
	m_button->setPosition(-2.0f, 2.0f);
	addChild(m_button);
	m_state.switchState(new PopupShowState(this, true));
}

// 0x4557A0
void PolicePopup::highlight()
{
	if (PizzaFrenzy::getGameLogic()->getSelectedCount() > 0)
	{
		PizzaPopup::highlight();
		m_state.switchState(new PopupActiveState(this));
	}
}

// 0x455860
void PolicePopup::select()
{
	PizzaPopup::select();
	m_state.switchState(new PopupActiveState(this));
}

// 0x455900
void PolicePopup::onDispatched()
{
	PizzaPopup::onDispatched();
	m_state.switchState(new PopupCloseState(this));
}

// 0x4559A0
void PolicePopup::deselect()
{
	PizzaPopup::deselect();
	m_state.switchState(new PopupResetState(this));
}

// 0x455A40
void PolicePopup::onShowFinished()
{
	m_state.switchState(new PopupResetState(this));
}

// 0x452050 (folded)
void PolicePopup::onCloseFinished()
{
	remove();
}

// 0x455AE0
std::string PolicePopup::getTypeName() const
{
	return "PolicePopup";
}

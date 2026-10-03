// PolicePopup: the police station's button, which sends a police car after a criminal customer.
#pragma once

#include <string>

#include "engine/RefPtr.h"
#include "PizzaPopup.h"

namespace engine
{
	class Image;
}

class BuildingTile;
class CustomerTile;

// The police station's button (res/pizza/policeButton.jpg), shown while a criminal's order is pending; sends a
// police car after the criminal. Popup type 2. 0x158 bytes: PizzaPopup up to +0x14C, m_button, vtordisp +0x150,
// Interface +0x154.
class PolicePopup : public PizzaPopup
{
public:
	PolicePopup();
	virtual ~PolicePopup();

	// engine::Component
	virtual std::string getTypeName() const;								// slot 24: "PolicePopup"

	// PizzaPopup
	virtual void init(BuildingTile* owner);									// slot 75
	virtual int getPopupType() const;										// slot 78: 2
	virtual void onShowFinished();											// slot 79
	virtual void onCloseFinished();											// slot 80 (0x452050, folded: remove())
	virtual void highlight();												// slot 81
	virtual void select();													// slot 82
	virtual void onDispatched();											// slot 83
	virtual void deselect();												// slot 84

	virtual void sendPoliceCar(CustomerTile* criminal);						// slot 90

	engine::RefPtr<engine::Image> m_button;									// +0x14C res/pizza/policeButton.jpg at (-2,2)
};

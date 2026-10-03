// NewToppingScreen: the new-topping selection screen (grid of topping buttons and a detail panel).
#pragma once

#include <string>

#include "engine/RefPtr.h"
#include "engine/Screen.h"

namespace engine
{
	class Container;
	class Image;
	class TextItem;
}

class OrderButton;
class Topping;

// res/screenLayouts/newToppingScreen.xml (game+0xEC): a 3-column grid of the player's new toppings in "leftButtons"
// and a detail panel on the right; clicking a topping gives it to the player. The members end at +0x244; the
// vtordisp (+0x244) and the Interface subobject (+0x248) follow them (size 0x24C).
class NewToppingScreen : public engine::Screen
{
public:
	NewToppingScreen();
	virtual ~NewToppingScreen();

	virtual void init();											// slot 95
	virtual void onButtonClicked(const std::string& name);			// slot 96
	virtual void populateToppings();								// slot 97
	virtual void showTopping(Topping* topping, int level);			// slot 98
	virtual OrderButton* createToppingButton(Topping* topping);		// slot 99

	virtual void onMouseEnter(std::string command);					// slot 0 of engine::ButtonListener (+0x144)

	engine::RefPtr<engine::Container> m_leftButtons;					// +0x1D4 "leftButtons"
	engine::RefPtr<engine::Image> m_rightToppingBg;						// +0x1D8 "rightToppingBg"
	engine::RefPtr<engine::Image> m_rightTopping;						// +0x1DC "rightTopping"
	engine::RefPtr<engine::TextItem> m_rightToppingCaptionText;			// +0x1E0 "rightToppingCaptionText": level name
	engine::RefPtr<engine::TextItem> m_rightToppingValue;				// +0x1E4 "rightToppingValue"
	engine::RefPtr<engine::TextItem> m_rightToppingLevel;				// +0x1E8 "rightToppingLevel"
	engine::RefPtr<engine::TextItem> m_rightToppingDesc;				// +0x1EC "rightToppingDesc"
	std::string m_valueFormat;											// +0x1F0 original text of rightToppingValue (format of the value)
	std::string m_levelFormat;											// +0x20C original text of rightToppingLevel (format of level+1)
	std::string m_unusedFormat;											// +0x228 only constructed and destroyed
};

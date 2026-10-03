// ToppingSelectionScreen: the pre-level topping picker (the player's toppings and the level's van slots).
#pragma once

#include <map>
#include <string>
#include <vector>

#include "engine/Point.h"
#include "engine/RefPtr.h"
#include "engine/Screen.h"

namespace engine
{
	class Component;
	class Container;
	class FadeContainer;
	class Image;
	class ImageButton;
	class TextItem;
}

class Level;
class OrderButton;
class Topping;

// res/screenLayouts/toppingSelectionScreen.xml, built by the game into game+0xE4. Pages of the player's toppings
// (12 per page, a 3x4 grid of OrderButtons showing "%d/4") and one "vanButton%d" slot per topping slot of the level:
// a click on a topping picks it up (its icon follows the mouse), a click on a slot places it; "selectionDone" writes
// the picks into the level and the profile's PickToppings. The members end at +0x26C; the vtordisp (+0x26C) and the
// Interface subobject (+0x270) follow them (0x274 bytes).
class ToppingSelectionScreen : public engine::Screen
{
public:
	ToppingSelectionScreen();
	virtual ~ToppingSelectionScreen();

	virtual void init();														// slot 95
	virtual void setLevel(Level* level);										// slot 96
	virtual void onMouseMove(const engine::Point& pos);						// slot 97: Application::m_mouseMoveSignal
	virtual void onRightClick(const engine::Point& pos);						// slot 98: Application::m_rightMouseDownSignal
	virtual void showPages();													// slot 99
	virtual void onButton(const std::string& name);							// slot 100
	virtual void showPage(int side, int page);									// slot 101
	virtual void pickUpTopping(Topping* topping);								// slot 102
	virtual void dropTopping();													// slot 103
	virtual void updateDoneButton();											// slot 104
	virtual void commitSelection();												// slot 105
	virtual void enableToppingButton(Topping* topping, bool enable);			// slot 106
	virtual OrderButton* createToppingButton(Topping* topping, int level);	// slot 107

	virtual void activate();						// slot 34 (engine::Component), folded body 0x443BB0
	virtual void onMouseEnter(std::string command);	// slot 0 of engine::ButtonListener (+0x144): shows the tooltip
	virtual void onMouseLeave(std::string command);	// slot 1 of engine::ButtonListener (+0x144): hides the tooltip

	engine::RefPtr<engine::Component> m_pages[2];						// +0x1D4 "%sPage" (left, right)
	engine::RefPtr<engine::Container> m_buttonPanels[2];				// +0x1DC "%sButtons": the page's topping buttons
	engine::RefPtr<engine::TextItem> m_pageTexts[2];					// +0x1E4 "%sToppingPage"
	engine::RefPtr<engine::Component> m_pageButtons[2];				// +0x1EC "%sButton": "bookLeft"/"bookRight"
	engine::RefPtr<engine::Component> m_doneButton;					// +0x1F4 "doneButton"
	engine::RefPtr<engine::Container> m_vanButtonPanel;				// +0x1F8 "vanButtons"
	std::vector<engine::RefPtr<engine::ImageButton> > m_vanButtons;	// +0x1FC "vanButton%d", one per level slot
	std::vector<engine::RefPtr<engine::Image> > m_vanImages;			// +0x20C icon of the topping in each slot
	std::vector<engine::RefPtr<Topping> > m_vanToppings;				// +0x21C topping of each slot (0 = empty)
	std::map<Topping*, engine::RefPtr<OrderButton> > m_toppingButtons;	// +0x22C button of each topping on the pages
	engine::RefPtr<Topping> m_heldTopping;								// +0x238 picked up, not placed yet
	engine::RefPtr<engine::Image> m_cursorImage;						// +0x23C held topping's icon, follows the mouse
	Level* m_level;														// +0x240 set by setLevel (not by the ctor); raw
	engine::RefPtr<engine::TextItem> m_captionText;					// +0x244 "captionText": tooltip text
	engine::RefPtr<engine::FadeContainer> m_caption;					// +0x248 "caption": tooltip panel
	std::string m_pageFormat;											// +0x24C layout text of "leftToppingPage"
	int m_page;															// +0x268 left page; set by init/setLevel, not by the ctor
};

// PizzaEditor: the pizza designer's controller (res\screenLayouts\userPizzaEditor.xml and pizzaNameScreen.xml).
#pragma once

#include <string>
#include <vector>

#include "engine/Object.h"
#include "engine/Point.h"
#include "engine/RefPtr.h"
#include "engine/sigslot.h"

namespace engine
{
	class Container;
	class Image;
	class Screen;
	class TextItem;
	class XmlWriter;
}

class PizzaDesign;
class Topping;
class ToppingButton;

// Browses, adds, deletes and renames the pizzas of a list (the game's user pizzas) and places toppings on them with
// the mouse; saves userPizzas.xml (res\manifests\pizzas.xml when not editing user pizzas). The game's instance is
// game+0xFC. Object +0x00, has_slots +0x0C; the members end at +0x56; the vtordisp (+0x58) and the Interface
// subobject (+0x5C) follow them (0x60 bytes).
class PizzaEditor : public engine::Object, public sigslot::has_slots<>
{
public:
	PizzaEditor(engine::Screen* editorScreen, engine::Screen* nameScreen, std::vector<engine::RefPtr<PizzaDesign> >* pizzas, bool userPizzas);
	virtual ~PizzaEditor();

	virtual void init();									// slot 1: once after construction, finds the screen's items
	virtual void onCommand(const std::string& command);	// slot 2: the command signal of both screens
	virtual void open();									// slot 3: entering the designer (topping palette, current pizza)

	void onMouseMove(const engine::Point& pos);
	void savePizza();
	void removeToppingAt(const engine::Vector2& pos);
	void updateCaption();
	void onRightMouseDown(const engine::Point& pos);
	void writePizzas(engine::XmlWriter* writer);
	bool savePizzas(const std::string& fileName);
	void addTopping(const engine::Vector2& pos, Topping* topping);
	void showPizza();
	void nextPizza();
	void addPizza();
	void prevPizza();
	void onMouseDown(const engine::Point& pos);
	void selectTopping(Topping* topping);
	void deletePizza();

	std::vector<engine::RefPtr<PizzaDesign> >* m_pizzas;			// +0x1C the list being edited; not owned
	engine::RefPtr<PizzaDesign> m_pizza;							// +0x20 (*m_pizzas)[m_pizzaIndex]
	engine::RefPtr<Topping> m_topping;								// +0x24 picked from the palette, placed by left clicks
	engine::RefPtr<engine::Screen> m_editorScreen;					// +0x28 userPizzaEditor.xml
	engine::RefPtr<engine::Screen> m_nameScreen;					// +0x2C pizzaNameScreen.xml (edit box "nameEntry")
	engine::RefPtr<engine::Container> m_toppingButtons;				// +0x30 "toppingButtons": the palette
	engine::RefPtr<engine::Container> m_targetToppings;				// +0x34 "targetToppings": the pizza (radius 128)
	engine::RefPtr<engine::TextItem> m_caption;						// +0x38 "caption": "%d. %s"
	std::vector<engine::RefPtr<ToppingButton> > m_toppingImages;	// +0x3C the pieces placed on the pizza
	engine::RefPtr<engine::Image> m_cursorImage;					// +0x4C m_topping's image following the mouse
	int m_pizzaIndex;												// +0x50 -1 when the list is empty
	bool m_modified;												// +0x54 cleared by the constructor, never used
	bool m_userPizzas;												// +0x55 user pizzas: userPizzas.xml, unlocked toppings only
};

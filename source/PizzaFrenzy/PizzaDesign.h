// PizzaDesign: a named pizza (built-in or made in the pizza editor) and its topping placements.
#pragma once

#include <string>
#include <vector>

#include "engine/Object.h"
#include "engine/Point.h"

namespace engine
{
	class Bitmap;
	class XmlWriter;
}

class Topping;

// A topping placed on a PizzaDesign, at a position relative to the pizza's centre (new'd by addTopping, deleted
// by clearToppings; implicit constructor).
struct ToppingPlacement
{
	Topping* topping;									// +0x00
	engine::Point position;								// +0x04
};

// A <pz n ID> of res\manifests\pizzas.xml or userPizzas.xml with one <t n p> per placement; the game turns each
// design into a "pizza%d" topping.
class PizzaDesign : public engine::Object
{
public:
	PizzaDesign(const std::string& name);
	virtual ~PizzaDesign();

	engine::Bitmap* createThumbnail() const;
	void clearToppings();
	void write(engine::XmlWriter& writer) const;
	void addTopping(Topping* topping, const engine::Point& position);

	// 0x4041E0 (folded)
	std::string getName() const
	{
		return m_name;
	}

	// Inlined everywhere: PizzaFrenzy::addPizza copies the id to a new temporary for each map lookup.
	int getId() const
	{
		return m_id;
	}

	std::string m_name;									// +0x0C "n" (default "Untitled")
	int m_id;											// +0x28 "ID"
	std::vector<ToppingPlacement*> m_toppings;			// +0x2C owned placements
};

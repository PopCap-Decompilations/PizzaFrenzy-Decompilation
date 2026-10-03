// PizzaDesign: a named pizza and its topping placements.
#include "engine/Application.h"
#include "engine/Graphics.h"
#include "engine/RefPtr.h"
#include "engine/Surface.h"
#include "engine/XmlWriter.h"

#include "PizzaDesign.h"
#include "TileManager.h"

// 0x418DB0
engine::Bitmap* PizzaDesign::createThumbnail() const
{
	engine::Bitmap* background = engine::getApplication()->getImage("res\\pizza\\decoratePizza.jpg");
	engine::RefPtr<engine::Bitmap> image = background->copy();
	engine::RefPtr<engine::Graphics> graphics = engine::getApplication()->createGraphics(image);
	for (std::vector<ToppingPlacement*>::const_iterator it = m_toppings.begin(); it != m_toppings.end(); ++it)
	{
		ToppingPlacement* placement = *it;
		graphics->setScale(1.2f, 1.2f);
		graphics->setSmoothing(1);
		graphics->drawImage(placement->topping->m_image, (float)(placement->position.x + background->getWidth() / 2),
			(float)(placement->position.y + background->getHeight() / 2));
	}
	return image->copyScaledUniform(50.0f / image->getWidth());
}

// 0x418F40
void PizzaDesign::clearToppings()
{
	for (std::vector<ToppingPlacement*>::iterator it = m_toppings.begin(); it != m_toppings.end(); ++it)
		delete *it;
	m_toppings.clear();
}

// 0x418F90
PizzaDesign::~PizzaDesign()
{
	clearToppings();
}

// 0x419020
void PizzaDesign::write(engine::XmlWriter& writer) const
{
	writer.startElement("pz");
	writer.writeAttribute("n", m_name);
	writer.writeAttribute("ID", m_id);
	for (std::vector<ToppingPlacement*>::const_iterator it = m_toppings.begin(); it != m_toppings.end(); ++it)
	{
		writer.startElement("t");
		writer.writeAttribute("n", (*it)->topping->m_name);
		writer.writeAttribute("p", (*it)->position);
		writer.endElement();
	}
	writer.endElement();
}

// 0x4194F0
PizzaDesign::PizzaDesign(const std::string& name)
	: m_name(name)
{
}

// 0x419590
void PizzaDesign::addTopping(Topping* topping, const engine::Point& position)
{
	ToppingPlacement* placement = new ToppingPlacement;
	placement->topping = topping;
	placement->position = position;
	m_toppings.push_back(placement);
}

#include "PizzaEditor.h"

#include <windows.h>

#include "engine/Application.h"
#include "engine/Container.h"
#include "engine/EditBox.h"
#include "engine/Exception.h"
#include "engine/Image.h"
#include "engine/Screen.h"
#include "engine/StringUtil.h"
#include "engine/TextItem.h"
#include "engine/XmlWriter.h"

#include "OrderButton.h"
#include "PizzaDesign.h"
#include "PizzaFrenzy.h"
#include "TileManager.h"
#include "ToppingButton.h"
#include "UserProgress.h"

// 0x40E170
void PizzaEditor::onMouseMove(const engine::Point& pos)
{
	engine::Vector2 offset = pos - engine::Point(m_targetToppings->getScreenPosition());
	if (offset.length() < 128.0f)
		m_cursorImage->setAlpha(1.0f);
	else
		m_cursorImage->setAlpha(0.5f);
	m_cursorImage->setPosition(engine::Vector2(pos));
}

// 0x40E310
void PizzaEditor::savePizza()
{
	if (m_pizza)
	{
		m_pizza->clearToppings();
		for (std::vector<engine::RefPtr<ToppingButton> >::iterator it = m_toppingImages.begin(); it != m_toppingImages.end(); ++it)
		{
			ToppingButton* button = *it;
			m_pizza->addTopping(button->getTopping(), engine::Point(button->getPosition()));
		}
		getGame()->addPizza(m_pizza, m_userPizzas);
	}
}

// 0x40E560
void PizzaEditor::init()
{
	m_toppingButtons = static_cast<engine::Container*>(m_editorScreen->getComponent("toppingButtons"));
	m_targetToppings = static_cast<engine::Container*>(m_editorScreen->getComponent("targetToppings"));
	m_caption = static_cast<engine::TextItem*>(m_editorScreen->getComponent("caption"));
	m_cursorImage = new engine::Image();
	m_editorScreen->addChild(m_cursorImage);
	m_pizzaIndex = m_pizzas->size() - 1;
}

// 0x40E7B0
void PizzaEditor::removeToppingAt(const engine::Vector2& pos)
{
	for (std::vector<engine::RefPtr<ToppingButton> >::iterator it = m_toppingImages.begin(); it != m_toppingImages.end(); ++it)
	{
		if (((*it)->getPosition() - pos).length() < 15.0f)
		{
			m_targetToppings->removeChild(*it);
			m_toppingImages.erase(it);
			return;
		}
	}
}

// 0x40E870
void PizzaEditor::updateCaption()
{
	std::string caption;
	engine::format(caption, "%d. %s", m_pizzaIndex + 1, m_pizza->getName().c_str());
	m_caption->setText(caption);
}

// 0x40E940
void PizzaEditor::onRightMouseDown(const engine::Point& pos)
{
	removeToppingAt(engine::Vector2(pos - engine::Point(m_targetToppings->getScreenPosition())));
}

// 0x40E990
void PizzaEditor::writePizzas(engine::XmlWriter* writer)
{
	writer->startElement("pm");
	writer->writeAttribute("maxID", getGame()->getNextPizzaId(m_userPizzas));
	for (std::vector<engine::RefPtr<PizzaDesign> >::iterator it = m_pizzas->begin(); it != m_pizzas->end(); ++it)
		(*it)->write(*writer);
	writer->endElement();
}

// 0x40EBE0
bool PizzaEditor::savePizzas(const std::string& fileName)
{
	engine::XmlWriter* writer;
	try
	{
		writer = new engine::XmlWriter(fileName.c_str());
	}
	catch (engine::Exception e)
	{
		MessageBoxA(NULL, e.m_message.c_str(), "ERROR", MB_OK);
		return false;
	}
	writePizzas(writer);
	writer->close();
	return true;
}

// 0x40EF80
PizzaEditor::~PizzaEditor()
{
}

// 0x40F300
PizzaEditor::PizzaEditor(engine::Screen* editorScreen, engine::Screen* nameScreen, std::vector<engine::RefPtr<PizzaDesign> >* pizzas, bool userPizzas)
{
	m_editorScreen = editorScreen;
	m_nameScreen = nameScreen;
	m_pizzas = pizzas;
	m_editorScreen->m_actionSignal.connect(this, &PizzaEditor::onCommand);
	m_nameScreen->m_actionSignal.connect(this, &PizzaEditor::onCommand);
	m_modified = false;
	m_userPizzas = userPizzas;
	m_pizza = NULL;
}

// 0x40F4D0
void PizzaEditor::addTopping(const engine::Vector2& pos, Topping* topping)
{
	ToppingButton* button = new ToppingButton();
	button->setTopping(topping);
	m_toppingImages.push_back(button);
	m_targetToppings->addChild(button);
	button->setPosition(pos);
	button->setEnabled(false);
}

// 0x40F5B0
void PizzaEditor::showPizza()
{
	if (m_pizzaIndex < (int)m_pizzas->size() && m_pizzaIndex >= 0)
	{
		m_pizza = m_pizzas->at(m_pizzaIndex);
		updateCaption();
		m_toppingImages.clear();
		m_targetToppings->removeAllChildren();
		for (std::vector<ToppingPlacement*>::iterator it = m_pizza->m_toppings.begin(); it != m_pizza->m_toppings.end(); ++it)
			addTopping(engine::Vector2((*it)->position), (*it)->topping);
	}
}

// 0x40F670
void PizzaEditor::nextPizza()
{
	savePizza();
	if (++m_pizzaIndex == (int)m_pizzas->size())
		m_pizzaIndex = 0;
	showPizza();
}

// 0x40F6B0
void PizzaEditor::addPizza()
{
	savePizza();
	PizzaDesign* pizza = new PizzaDesign("Untitled");
	pizza->m_id = getGame()->getNextPizzaId(m_userPizzas);
	getGame()->setNextPizzaId(pizza->m_id + 1, m_userPizzas);
	getGame()->addPizza(pizza, m_userPizzas);
	m_pizzaIndex = m_pizzas->size() - 1;
	m_pizza = pizza;
	showPizza();
}

// 0x40F800
void PizzaEditor::prevPizza()
{
	savePizza();
	if (--m_pizzaIndex < 0)
		m_pizzaIndex = m_pizzas->size() - 1;
	showPizza();
}

// 0x40F840
void PizzaEditor::onMouseDown(const engine::Point& pos)
{
	engine::Vector2 offset = pos - engine::Point(m_targetToppings->getScreenPosition());
	if (offset.length() < 128.0f && m_topping)
		addTopping(offset, m_topping);
}

// 0x40F8B0
void PizzaEditor::selectTopping(Topping* topping)
{
	if (m_topping)
	{
		engine::getApplication()->m_mouseMoveSignal.disconnect(this);
		engine::getApplication()->m_mouseDownSignal.disconnect(this);
		engine::getApplication()->m_rightMouseDownSignal.disconnect(this);
	}
	m_topping = topping;
	if (m_topping)
	{
		m_cursorImage->setImage(m_topping->m_image);
		engine::getApplication()->m_mouseMoveSignal.connect(this, &PizzaEditor::onMouseMove);
		engine::getApplication()->m_mouseDownSignal.connect(this, &PizzaEditor::onMouseDown);
		engine::getApplication()->m_rightMouseDownSignal.connect(this, &PizzaEditor::onRightMouseDown);
	}
	else
	{
		m_cursorImage->setImage(NULL);
	}
}

// 0x40F9D0
void PizzaEditor::open()
{
	m_toppingButtons->removeAllChildren();
	UserProgress* profile = PizzaFrenzy::getProfile();
	TileManager* manifest = PizzaFrenzy::getTileManifest();
	float x = 0.0f;
	float y = 0.0f;
	for (std::vector<engine::RefPtr<Topping> >::iterator it = manifest->m_toppingList.begin(); it != manifest->m_toppingList.end(); ++it)
	{
		Topping* topping = *it;
		if (!m_userPizzas || profile->getToppingLevel(topping) >= 0)
		{
			OrderButton* button = new OrderButton(topping, -1, m_editorScreen);
			button->setPosition(x, y);
			button->setScale(0.9f);
			m_toppingButtons->addChild(button);
		}
		x += 60.0f;
		if (x >= 750.0f)
		{
			x = 0.0f;
			y += 60.0f;
		}
	}
	if (m_pizzaIndex < 0)
		addPizza();
	else
		showPizza();
}

// 0x40FB20
void PizzaEditor::deletePizza()
{
	if (m_pizzas->size() > 0)
	{
		std::vector<engine::RefPtr<PizzaDesign> >::iterator it = m_pizzas->begin();
		for (int i = 0; i < m_pizzaIndex; i++)
			++it;
		m_pizzas->erase(it);
	}
	if (m_pizzaIndex >= (int)m_pizzas->size())
		m_pizzaIndex = m_pizzas->size() - 1;
	m_pizza = NULL;
	if (m_pizzas->empty())
		addPizza();
	else
		showPizza();
}

// 0x40FC20
void PizzaEditor::onCommand(const std::string& command)
{
	if (command == "editorSnapToGrid")
	{
	}
	else if (command == "editCaption")
	{
		engine::EditBox* nameEntry = static_cast<engine::EditBox*>(m_nameScreen->getComponent("nameEntry"));
		nameEntry->setText(m_pizza->getName());
		getGame()->onScreenEvent("namePizza");
		selectTopping(NULL);
	}
	else if (command == "editorExit")
	{
		savePizza();
		savePizzas(m_userPizzas ? std::string("userPizzas.xml") : std::string("res\\manifests\\pizzas.xml"));
		getGame()->onScreenEvent("editorExit");
	}
	else if (command == "prevPizza")
	{
		prevPizza();
	}
	else if (command == "addPizza")
	{
		addPizza();
	}
	else if (command == "nextPizza")
	{
		nextPizza();
	}
	else if (command == "resetPizza")
	{
		showPizza();
	}
	else if (command == "deletePizza")
	{
		deletePizza();
	}
	else if (command == "nameEntryDone")
	{
		engine::EditBox* nameEntry = static_cast<engine::EditBox*>(m_nameScreen->getComponent("nameEntry"));
		std::string name = nameEntry->getText();
		if (!name.empty())
		{
			for (std::string::size_type i = name.find('"'); i != std::string::npos; i = name.find('"', i))
				name[i] = '\'';
			m_pizza->m_name = name;
			updateCaption();
			getGame()->onScreenEvent("done");
		}
	}
	else if (command == "nameEntryCancel")
	{
		getGame()->onScreenEvent("done");
	}
	else
	{
		Topping* topping = PizzaFrenzy::getTileManifest()->getTopping(command);
		if (topping)
			selectTopping(topping);
	}
}

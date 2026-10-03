// ToppingButton: the draggable, selectable topping piece of the pizza editor.
#include "engine/Application.h"
#include "engine/Selector.h"

#include "TileManager.h"
#include "ToppingButton.h"

// 0x419760
void ToppingButton::mouseMove(const engine::MouseEvent& event)
{
	if (m_state == 3 || m_state == 2)
		setPosition(engine::getApplication()->getMousePosition() + m_dragOffset);
}

// 0x4197C0
void ToppingButton::mouseDown(const engine::MouseEvent& event)
{
	engine::Button::mouseDown(event);
	m_dragOffset = getPosition() - engine::getApplication()->getMousePosition();
}

// 0x419830
void ToppingButton::setSelected(bool selected)
{
	if (selected)
		m_selector->select(1, 0.0f, true);
	else
		m_selector->select(0, 0.0f, true);
	m_selected = selected;
}

// 0x419870
bool ToppingButton::isSelected() const
{
	return m_selected;
}

// 0x419880
ToppingButton::~ToppingButton()
{
}

// 0x419950
ToppingButton::ToppingButton()
{
}

// 0x419A20
Topping* ToppingButton::getTopping() const
{
	return m_topping;
}

// 0x419A30
void ToppingButton::setTopping(Topping* topping)
{
	setHotspotMode(1);
	engine::ImageButton::setImages(topping->m_image, NULL, NULL);
	setSounds("", "res\\sounds\\menu_clicked.ogg");
	m_topping = topping;
	m_selected = false;
}

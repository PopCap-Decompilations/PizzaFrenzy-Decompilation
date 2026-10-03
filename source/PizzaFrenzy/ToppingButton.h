// ToppingButton: the draggable, selectable topping piece of the pizza editor.
#pragma once

#include "engine/ImageButton.h"
#include "engine/Object.h"
#include "engine/Point.h"
#include "engine/RefPtr.h"

class Topping;

// An ImageButton drawn with its topping's image; dragged while pressed.
class ToppingButton : public engine::ImageButton
{
public:
	ToppingButton();
	virtual ~ToppingButton();

	virtual void setTopping(Topping* topping);								// slot 92
	virtual Topping* getTopping() const;									// slot 93
	virtual void setSelected(bool selected);								// slot 94
	virtual bool isSelected() const;										// slot 95

	// overrides
	virtual void mouseDown(const engine::MouseEvent& event);				// engine::Component slot 40
	virtual void mouseMove(const engine::MouseEvent& event);				// engine::Component slot 52

	engine::RefPtr<engine::Object> m_unused;			// +0x180 released by the destructor, never set
	engine::RefPtr<Topping> m_topping;					// +0x184
	engine::Point m_dragOffset;							// +0x188 button position minus mouse at mouse down
	bool m_selected;									// +0x190
};

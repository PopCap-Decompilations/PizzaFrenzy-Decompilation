// OrderButton: a button (res\pizza\orderButton.jpg) showing a topping's icon and a count.
#pragma once

#include <string>

#include "engine/Container.h"
#include "engine/RefPtr.h"

namespace engine
{
	class Image;
	class ImageButton;
	class ScreenLayout;
	class TextItem;
}

class Topping;

// Used by the pizza editor, the new-topping and topping-selection screens and the decorate-pizza game. A negative
// count hides the count text. Implicit destructor (0x4276F0).
class OrderButton : public engine::Container
{
public:
	OrderButton(Topping* topping, int count, engine::ScreenLayout* layout);

	virtual void setCountText(const std::string& text);						// slot 75
	virtual void enable();														// slot 76
	virtual void disable();														// slot 77

	// overrides
	virtual void setScale(float scale);											// engine::Component slot 8
	virtual void setEnabled(bool enabled);										// engine::Component slot 32

	engine::RefPtr<engine::ImageButton> m_button;		// +0x128 orderButton.jpg, named after the topping
	engine::RefPtr<engine::Image> m_icon;				// +0x12C the topping's image
	engine::RefPtr<Topping> m_topping;					// +0x130
	engine::RefPtr<engine::TextItem> m_countText;		// +0x134 buttonFont.xml
	int m_count;										// +0x138 count shown; < 0 hides the text
};

// engine::Checkbox: two-state image button (a Selector child shows the unchecked/checked images and their hovers).
#pragma once

#include <string>

#include "Button.h"

namespace engine
{
	class Bitmap;
	class Selector;

	// Built by the layout element <checkBox> and grouped by RadioGroup. Implicit constructor (inlined in
	// ScreenLayoutParser::parseCheckBox; it leaves m_checked and m_selector to create()) and implicit destructor.
	// Layout: Button +0x00, members +0x17C, then the vtordisp and Interface (0x18C bytes).
	class Checkbox : public Button
	{
	public:
		// Component
		// 0x484E70 (emitted in ScreenLayoutParser.cpp, with the vtable: the type name is inline)
		virtual std::string getTypeName() const
		{
			return "Checkbox";
		}

		// Button
		virtual void onStateChanged();
		virtual void fireClick();

		// slot 91: false unless unchecked and checked are given; creates the Selector with the four images (the
		// hover images default to the plain ones)
		virtual bool create(Bitmap* unchecked, Bitmap* uncheckedOver, Bitmap* checked, Bitmap* checkedOver);
		// slot 92
		virtual bool isChecked() const;
		// slot 93: stores it, then updateCheckImage()
		virtual void setChecked(bool checked);
		// slot 94: Selector frame from the button state and m_checked
		virtual void updateCheckImage();

		bool m_checked;							// +0x17C current state
		Selector* m_selector;					// +0x180 frames 0 unchecked, 1 uncheckedOver, 2 checked, 3 checkedOver
	};
}

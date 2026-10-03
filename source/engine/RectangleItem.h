// engine::RectangleItem: a filled rectangle in the component's colour.
#pragma once

#include <string>

#include "Component.h"
#include "Rect.h"

namespace engine
{
	class Graphics;

	// Adds no members: the rectangle is the Component's bounds (+0x78). Component +0x00, then the vtordisp (+0x108)
	// and the Interface subobject (+0x10C): 0x110 bytes. The constructor and setRect take the Rect by value (the
	// callee destroys it: Rect has a user-declared destructor). Implicit destructor: its copy 0x483A50 is a bare
	// jump to ~Component without vfptr stores.
	class RectangleItem : public Component
	{
	public:
		RectangleItem(Rect rect);

		virtual std::string getTypeName() const;								// slot 24 (engine::Component)
		virtual void draw(Graphics& g);											// slot 36 (engine::Component)
		virtual void updateBounds();											// slot 38 (engine::Component): empty, folded 0x4D0470

		virtual void setRect(Rect rect);										// slot 66
	};
}

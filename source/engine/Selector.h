// engine::Selector: a container that shows only its selected child, with an optional timed crossfade.
#pragma once

#include <string>

#include "Container.h"

namespace engine
{
	class Graphics;

	// Container +0x00, members from +0x128, then the vtordisp (+0x13C) and the Interface subobject (+0x140): 0x144
	// bytes. Base of AnimImage; ImageButton and Checkbox keep their images in one. Implicit destructor (the deleting
	// destructor is the folded Container one, 0x47BBF0). Slots 76 and 78 have the same body (0x47B6C0).
	class Selector : public Container
	{
	public:
		Selector();

		virtual std::string getTypeName() const;								// slot 24 (engine::Component)
		virtual void draw(Graphics& g);											// slot 36 (engine::Component)
		virtual void update(UpdateContext& context);							// slot 37 (engine::Component)
		virtual void updateBounds();											// slot 38 (engine::Component)

		virtual void select(int index, float fadeTime, bool fadeInNext);		// slot 75: fadeTime 0 switches at once
		virtual int getSelection() const;										// slot 76: m_selected, folded 0x47B6C0
		virtual Component* getSelectedChild() const;							// slot 77: 0 when none
		virtual int getSelectedIndex() const;									// slot 78: m_selected, folded 0x47B6C0
		virtual int getCount() const;											// slot 79: child count, folded 0x47B6D0

		int m_selected;							// +0x128 -1 = none
		int m_next;								// +0x12C child being faded to, -1
		float m_fadeTimer;						// +0x130
		float m_fadeTime;						// +0x134 0 = no fade running
		bool m_fadeInNext;						// +0x138 true: next child's alpha 0 to 1; false: current child's 1 to 0
	};
}

// engine::ImageButton: a button whose look is a Selector of three images (normal, hover, pressed).
#pragma once

#include <string>

#include "Button.h"

namespace engine
{
	class Bitmap;
	class Selector;

	// Button +0x00, m_selector +0x17C, then the vtordisp (+0x180) and the Interface subobject (+0x184): 0x188 bytes.
	// Base of ToppingButton. getTypeName is inline (its out-of-line copy was emitted in another object). The
	// destructor is implicit: its copy 0x419750 is a bare jump to ~Button without the vfptr stores a user-declared
	// destructor has (folded with Checkbox's, deleting destructor 0x4849F0).
	class ImageButton : public Button
	{
	public:
		ImageButton();

		// 0x419B60 (copy emitted in another object)
		virtual std::string getTypeName() const					// slot 24 (engine::Component)
		{
			return "ImageButton";
		}

		virtual void onStateChanged();							// slot 83 (engine::Button)
		virtual void setPulseLevel(float level);				// slot 89 (engine::Button)
		virtual void endPulse();								// slot 90 (engine::Button): onStateChanged(), folded 0x4646E0

		virtual bool setImages(Bitmap* normal, Bitmap* hover, Bitmap* pressed);	// slot 91

		Selector* m_selector;					// +0x17C child Selector with the three images (raw; the child list holds it)
	};
}

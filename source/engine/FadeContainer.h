// engine::FadeContainer: a container that fades its alpha in or out over a duration, with a completion callback.
#pragma once

#include <string>

#include "Container.h"

namespace engine
{
	struct UpdateContext;

	// Container +0x00, members from +0x128, then the vtordisp (+0x144) and the Interface subobject (+0x148): 0x14C
	// bytes. getTypeName and clearFadeCallback are inline (their copies are in the game's objects); the destructor is
	// implicit (0x44FB40 is a jmp to ~Container that stores no vtable).
	class FadeContainer : public Container
	{
	public:
		FadeContainer();

		// 0x450440
		virtual std::string getTypeName() const							// slot 24 (engine::Component)
		{
			return "FadeContainer";
		}

		virtual void update(UpdateContext& context);					// slot 37 (engine::Component): runs the fade

		// amount: the target alpha of a fade in, or the alpha taken off by a fade out; removeWhenDone (fade out
		// only) sets the remove flag 0x10 when a full fade out ends
		virtual void fade(bool fadeIn, float duration, float amount, bool removeWhenDone);	// slot 75
		virtual void setFadeCallback(void (*callback)(void*), void* userData);				// slot 76: cdecl callback

		// 0x401140
		virtual void clearFadeCallback()								// slot 77
		{
			m_fadeCallback = 0;
			m_fadeCallbackData = 0;
		}

		virtual bool isFadeFinished() const;							// slot 78: m_fadeTimeLeft == 0

		float m_fadeTimeLeft;								// +0x128
		float m_fadeDuration;								// +0x12C 1
		bool m_fadingIn;									// +0x130
		void (*m_fadeCallback)(void*);						// +0x134 run when the fade ends
		void* m_fadeCallbackData;							// +0x138
		float m_fadeAmount;									// +0x13C 1
		bool m_removeWhenFaded;								// +0x140
	};
}

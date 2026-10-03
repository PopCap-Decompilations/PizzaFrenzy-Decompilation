// engine::SlideTransition: a screen transition that moves its target along a two-key path as its progress runs.
#pragma once

#include "Point.h"
#include "PropertyTrack.h"
#include "Transition.h"

namespace engine
{
	// Transition +0x00 (Animator +0x00, Object +0x08), members from +0x2C, then the vtordisp (+0x8C) and the
	// Interface subobject (+0x90): 0x94 bytes. The path has a key at t = 0 (m_from) and one at t = 1 (m_to); screens
	// slide in from (0, +-600) or (+-800, 0) in 0.2 s. The destructor (0x475280) is implicit.
	class SlideTransition : public Transition
	{
	public:
		SlideTransition(float duration);

		virtual void apply();					// slot 5 (engine::Transition): target alpha 1, position from the path

		void setMotion(Vector2 from, Vector2 to);

		Vector2 m_from;							// +0x2C start offset
		Vector2 m_to;							// +0x34 end offset
		PositionTrack m_path;					// +0x3C motion path (0x50 bytes), animated with m_progress
	};
}

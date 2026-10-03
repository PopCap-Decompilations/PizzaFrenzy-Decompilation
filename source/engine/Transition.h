// engine::Transition, an animator that drives a progress value between 0 and 1 over a duration (the screens' show and
// hide transitions; apply() shows the progress on the target), and engine::FadeTransition, which fades the target.
#pragma once

#include "Animator.h"
#include "Object.h"

namespace engine
{
	class Component;
	struct UpdateContext;

	// Animator +0x00 (its vbptr +0x04 is the class's), Object +0x08, members from +0x14, then the vtordisp (+0x2C) and
	// the Interface subobject (+0x30): 0x34 bytes. The destructor is implicit (0x4016D0 and the scalar deleting
	// destructor 0x4016B0, reached through the Object vtable's thunk 0x469080, are emitted in the game's objects).
	// The constructor leaves m_target, m_progress and m_timeLeft uninitialised.
	class Transition : public Animator, public Object
	{
	public:
		Transition(float duration);

		// time left -= elapsed, progress from it, apply(), finished at 1 (direction 0) or 0 (direction 1)
		virtual void update(UpdateContext& context, Component* component);	// slot 0 (engine::Animator)

		virtual void setTarget(Component* target);						// slot 1 (0x490FC0 folded)
		virtual void setProgress(float progress);						// slot 2: clamped to 0..1, apply()
		virtual void start(int direction);								// slot 3: 0 in (0 -> 1), 1 out (1 -> 0)
		virtual bool isFinished() const;								// slot 4
		virtual void apply() = 0;										// slot 5

		Component* m_target;								// +0x14
		int m_direction;									// +0x18 0 in, 1 out
		float m_progress;									// +0x1C
		float m_duration;									// +0x20
		float m_timeLeft;									// +0x24
		bool m_finished;									// +0x28 true initially
	};

	// Transition with no members of its own (0x34 bytes) and an implicit destructor. The constructor is inline: its
	// only copy (0x401620) was emitted in the game's first object, the one that creates the transitions
	// (LoadingState::update: 0.5, 0.4, 0.3 and 0.1 s), next to Transition's implicit destructor.
	class FadeTransition : public Transition
	{
	public:
		// 0x401620
		FadeTransition(float duration)
			: Transition(duration)
		{
		}

		// target alpha = progress; hidden (mask flag 1) at 0, shown otherwise
		virtual void apply();											// slot 5 (engine::Transition)
	};
}

// engine::Animator: runs on a component every update (Component::m_animators).
#pragma once

#include "Interface.h"

namespace engine
{
	class Component;
	struct UpdateContext;

	// {vfptr, vbptr} before the virtual Interface: in engine::Transition it takes +0x00..+0x08, the vbptr at +0x04 is
	// Transition's own, and Object follows at +0x08. Its abstract vtable {_purecall} (0x503A70) was merged with
	// ActionListener's, PixelFilter's and Runnable's; constructor always inlined, no destructor of its own.
	class Animator : public virtual Interface
	{
	public:
		virtual void update(UpdateContext& context, Component* component) = 0;	// slot 0: Component::updateAnimators
	};
}

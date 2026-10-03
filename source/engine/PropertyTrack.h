// engine::PropertyAnimator and the keyframe property tracks (ScaleTrack, AlphaTrack, PositionTrack).
#pragma once

#include "Component.h"
#include "Interface.h"
#include "KeyframeCurve.h"
#include "Point.h"

namespace engine
{
	// Applies a track's value at a time to a component: the interface through which AnimatedContainer, StoryScreen,
	// DecoratePizzaGame, CouponMoveAction and SlideTransition drive the tracks (their subobject at +0x40). Its vtable
	// {_purecall} was merged with the other one-slot interfaces' (0x503A70), but it is not Animator: slot 0 takes the
	// time as a float (Animator's takes an UpdateContext).
	class PropertyAnimator : public virtual Interface
	{
	public:
		virtual void animate(float time, Component* target) = 0;				// slot 0
	};

	// The tracks are held by value in engine and game objects (SlideTransition, AnimatedContainer, StoryScreen,
	// DecoratePizzaGame, CouponMoveAction). All their functions are inline: the original emitted them in the first
	// object that used them, AnimatedContainer's. Layout: KeyframeCurve +0x00, PropertyAnimator +0x40, then the
	// vtordisp (+0x48) and the Interface subobject (+0x4C): 0x50 bytes.

	// A float keyframe curve that writes its value into a component property through apply(). Abstract; implicit
	// constructor (always inlined) and destructor (0x42C110).
	class FloatPropertyTrack : public KeyframeCurve<float>, public PropertyAnimator
	{
	public:
		virtual void apply(Component* target, const float& value) = 0;			// slot 6

		// overrides

		// 0x42C600 (PropertyAnimator slot 0)
		virtual void animate(float time, Component* target)
		{
			apply(target, evaluate(time));
		}
	};

	// Drives Component::setScale(float). Implicit destructor.
	class ScaleTrack : public FloatPropertyTrack
	{
	public:
		// 0x42C710
		ScaleTrack()
		{
		}

		// overrides

		// 0x42C7B0 (FloatPropertyTrack slot 6)
		virtual void apply(Component* target, const float& value)
		{
			target->setScale(value);
		}
	};

	// Drives Component::setAlpha. Implicit destructor.
	class AlphaTrack : public FloatPropertyTrack
	{
	public:
		// 0x42C7D0
		AlphaTrack()
		{
		}

		// overrides

		// 0x42C870 (FloatPropertyTrack slot 6)
		virtual void apply(Component* target, const float& value)
		{
			target->setAlpha(value);
		}
	};

	// A 2D keyframe curve that writes its value into a component property through apply(). Abstract; implicit
	// constructor (always inlined) and destructor (0x42C120).
	class PointPropertyTrack : public KeyframeCurve<Vector2>, public PropertyAnimator
	{
	public:
		virtual void apply(Component* target, const Vector2& value) = 0;		// slot 6

		// overrides

		// 0x42C650 (PropertyAnimator slot 0)
		virtual void animate(float time, Component* target)
		{
			apply(target, evaluate(time));
		}
	};

	// Drives Component::setPosition(const Vector2&). Implicit destructor.
	class PositionTrack : public PointPropertyTrack
	{
	public:
		// 0x42C890
		PositionTrack()
		{
		}

		// overrides

		// 0x42C930 (PointPropertyTrack slot 6)
		virtual void apply(Component* target, const Vector2& value)
		{
			target->setPosition(value);
		}
	};
}

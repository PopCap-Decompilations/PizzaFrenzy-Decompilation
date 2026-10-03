// engine::Curve<T>: an abstract time curve (keys added, value evaluated at a time).
#pragma once

#include "Object.h"

namespace engine
{
	template <class T> struct Keyframe;

	// Object +0x00 and five pure slots; implemented by KeyframeCurve<T>. Constructor always inlined, implicit
	// destructor (slot 0 is the folded empty-destructor 0x44A0A0). Curve<float> and Curve<Vector2> share one vtable
	// (0x5035BC, merged by /OPT:ICF).
	template <class T>
	class Curve : public Object
	{
	public:
		virtual void addKey(const Keyframe<T>& key) = 0;		// slot 1: insert a key ordered by time
		virtual void clear() = 0;								// slot 2: remove all keys
		virtual T evaluate(float time) = 0;						// slot 3: value at time
		virtual float getStartTime() const = 0;					// slot 4: first key's time (0 if empty)
		virtual float getEndTime() const = 0;					// slot 5: last key's time (0 if empty)
	};
}

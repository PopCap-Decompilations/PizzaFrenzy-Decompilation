// engine::Keyframe<T> and engine::KeyframeCurve<T>: keyframed curves with eased segments (float and Vector2).
#pragma once

#include <set>

#include "Curve.h"
#include "EaseCurve.h"
#include "Point.h"

namespace engine
{
	// A key of KeyframeCurve<T> (plain value type; 0x18 bytes for float, 0x1C for Vector2). The in/out times and
	// weights shape the eased segments on either side of the key; the curve's set orders the keys by time.
	template <class T>
	struct Keyframe
	{
		// inlined in the set's searches: keys are ordered by time
		bool operator<(const Keyframe& other) const
		{
			return time < other.time;
		}

		float time;								// +0x00
		T value;								// +0x04
		float inTime;							// ease of the segment ending at this key
		float inWeight;
		float outTime;							// ease of the segment starting at this key
		float outWeight;
	};

	// Instantiated for float and Vector2 (the game's property tracks derive from them). Object +0x00, members from
	// +0x0C, then the vtordisp (+0x40) and the Interface subobject (+0x44): 0x48 bytes. evaluate() caches the
	// segment around the last time asked for (m_segment and its ease m_ease).
	template <class T>
	class KeyframeCurve : public Curve<T>
	{
	public:
		typedef typename std::set<Keyframe<T> >::iterator iterator;
		typedef typename std::set<Keyframe<T> >::const_iterator const_iterator;

		KeyframeCurve();
		virtual ~KeyframeCurve();

		virtual void addKey(const Keyframe<T>& key);			// slot 1 (engine::Curve<T>)
		virtual void clear();									// slot 2 (engine::Curve<T>)
		virtual T evaluate(float time);							// slot 3 (engine::Curve<T>)
		virtual float getStartTime() const;						// slot 4 (engine::Curve<T>): folded 0x47FBD0 (both instantiations)
		virtual float getEndTime() const;						// slot 5 (engine::Curve<T>)

		void setupSegment(const_iterator from, const_iterator to);

		std::set<Keyframe<T> > m_keys;			// +0x0C ordered by time
		iterator m_segment;						// +0x18 key starting the cached segment (end() when none)
		EaseCurve m_ease;						// +0x1C ease of the cached segment (0x24 bytes, not initialised by the constructor)
	};
}

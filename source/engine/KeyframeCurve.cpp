#include "KeyframeCurve.h"

#include <algorithm>

namespace engine
{
	// the value between two keys at the eased fraction t (inlined into evaluate: float arithmetic for float, a
	// component-wise local Vector2 for Vector2)
	static inline float interpolate(float from, float to, float t)
	{
		return (to - from) * t + from;
	}

	static inline Vector2 interpolate(const Vector2& from, const Vector2& to, float t)
	{
		Vector2 value;
		value.x = (to.x - from.x) * t + from.x;
		value.y = (to.y - from.y) * t + from.y;
		return value;
	}

	// 0x47FBD0 (folded)
	template <class T>
	float KeyframeCurve<T>::getStartTime() const
	{
		if (m_keys.empty())
			return 0.0f;
		return m_keys.begin()->time;
	}

	// 0x47FBF0 (KeyframeCurve<float>)
	// 0x47FC70 (KeyframeCurve<Vector2>)
	template <class T>
	void KeyframeCurve<T>::setupSegment(const_iterator from, const_iterator to)
	{
		float scale = 1.0f / (to->time - from->time);
		float accelEnd = from->outTime * scale;
		float decelStart = 1.0f - to->inTime * scale;
		m_ease.init(from->outWeight, to->inWeight, accelEnd, (std::max)(accelEnd, decelStart));
	}

	// 0x47FCF0 (KeyframeCurve<float>)
	// 0x47FE60 (KeyframeCurve<Vector2>)
	template <class T>
	void KeyframeCurve<T>::clear()
	{
		m_keys.clear();
		m_segment = m_keys.end();
	}

	// 0x47FD30 (KeyframeCurve<float>)
	// 0x47FEA0 (KeyframeCurve<Vector2>)
	template <class T>
	T KeyframeCurve<T>::evaluate(float time)
	{
		bool changed = false;
		iterator next = m_segment;
		++next;
		while (next != m_keys.end() && time > next->time)
		{
			m_segment = next;
			++next;
			changed = true;
		}
		while (m_segment != m_keys.begin() && time < m_segment->time)
		{
			next = m_segment;
			--m_segment;
			changed = true;
		}
		if (next == m_keys.end())
			return m_segment->value;
		if (changed)
			setupSegment(m_segment, next);
		if (time < m_segment->time)
			return m_segment->value;
		float t = (time - m_segment->time) / (next->time - m_segment->time);
		return interpolate(m_segment->value, next->value, m_ease.evaluate(t));
	}

	// 0x480000 (KeyframeCurve<float>)
	// 0x480030 (KeyframeCurve<Vector2>)
	template <class T>
	float KeyframeCurve<T>::getEndTime() const
	{
		if (m_keys.empty())
			return 0.0f;
		return m_keys.rbegin()->time;
	}

	// 0x480C40 (KeyframeCurve<float>)
	// 0x480C70 (KeyframeCurve<Vector2>)
	template <class T>
	void KeyframeCurve<T>::addKey(const Keyframe<T>& key)
	{
		m_segment = m_keys.insert(key).first;
	}

	// 0x480D00 (KeyframeCurve<float>)
	// 0x480EB0 (KeyframeCurve<Vector2>)
	template <class T>
	KeyframeCurve<T>::KeyframeCurve()
	{
		clear();
	}

	// 0x480DF0 (KeyframeCurve<float>)
	// 0x480FA0 (KeyframeCurve<Vector2>)
	template <class T>
	KeyframeCurve<T>::~KeyframeCurve()
	{
		clear();
	}

	// the two instances the engine and the game's property tracks use
	template class KeyframeCurve<float>;
	template class KeyframeCurve<Vector2>;
}

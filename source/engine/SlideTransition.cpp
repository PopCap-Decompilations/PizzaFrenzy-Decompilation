#include "SlideTransition.h"

#include "Component.h"
#include "KeyframeCurve.h"

namespace
{
	// The keys are built by an inlined constructor-like helper of the original (no address): the value comes by
	// value (copy-constructed from the member), then the key's members are assigned one by one (the value's default
	// constructor runs after the argument is built). engine::Keyframe itself is a plain struct.
	engine::Keyframe<engine::Vector2> makeKeyframe(float time, engine::Vector2 value, float inTime, float inWeight, float outTime, float outWeight)
	{
		engine::Keyframe<engine::Vector2> key;
		key.time = time;
		key.value = value;
		key.inTime = inTime;
		key.inWeight = inWeight;
		key.outTime = outTime;
		key.outWeight = outWeight;
		return key;
	}
}

namespace engine
{
	// 0x475040
	void SlideTransition::apply()
	{
		m_target->setAlpha(1.0f);
		m_path.animate(m_progress, m_target);
	}

	// 0x475070: a key at t = 0 on from and one at t = 1 on to, eased in and out
	void SlideTransition::setMotion(Vector2 from, Vector2 to)
	{
		m_path.clear();
		m_to = to;
		m_from = from;
		m_path.addKey(makeKeyframe(0.0f, m_from, 0.0f, 1.0f, 0.0f, 1.0f));
		m_path.addKey(makeKeyframe(1.0f, m_to, 0.3f, 0.0f, 0.3f, 1.0f));
	}

	// 0x475160
	SlideTransition::SlideTransition(float duration)
		: Transition(duration)
	{
		m_from = Vector2(0.0f, 0.0f);
		m_to = Vector2(0.0f, 0.0f);
	}
}

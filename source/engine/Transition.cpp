#include "Transition.h"

#include "Component.h"

namespace engine
{
	// 0x468E50
	void FadeTransition::apply()
	{
		m_target->setAlpha(m_progress);
		if (m_progress == 0.0f)
			m_target->maskFlags(1);
		else
			m_target->unmaskFlags(1);
	}

	// 0x468E90
	void Transition::start(int direction)
	{
		if (m_finished)
		{
			m_direction = direction;
			m_timeLeft = m_duration;
			m_finished = false;
			m_progress = 0.0f;
		}
		else if (direction != m_direction)
		{
			m_direction = direction;
			m_timeLeft = m_duration * m_progress;
		}
	}

	// 0x468ED0
	bool Transition::isFinished() const
	{
		return m_finished;
	}

	// 0x468EE0
	void Transition::update(UpdateContext& context, Component* component)
	{
		if (m_finished)
			return;
		m_timeLeft -= context.elapsed;
		m_timeLeft = m_timeLeft < 0.0f ? 0.0f : m_timeLeft;
		switch (m_direction)
		{
		case 0:
			m_progress = 1.0f - m_timeLeft / m_duration;
			break;
		case 1:
			m_progress = m_timeLeft / m_duration;
			break;
		}
		apply();
		if ((m_direction == 0 && m_progress == 1.0f) || (m_direction == 1 && m_progress == 0.0f))
			m_finished = true;
	}

	// 0x468F70: the time left is progress * duration in both directions
	void Transition::setProgress(float progress)
	{
		m_progress = progress;
		m_progress = m_progress > 1.0f ? 1.0f : m_progress;
		m_progress = m_progress < 0.0f ? 0.0f : m_progress;
		m_timeLeft = m_progress * m_duration;
		apply();
		if (!m_finished)
		{
			if ((m_direction == 0 && m_progress == 1.0f) || (m_direction == 1 && m_progress == 0.0f))
				m_finished = true;
		}
	}

	// 0x469000
	Transition::Transition(float duration)
	{
		m_duration = duration;
		m_direction = 0;
		m_finished = true;
	}

	// 0x490FC0 (folded)
	void Transition::setTarget(Component* target)
	{
		m_target = target;
	}
}

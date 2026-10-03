#include "EaseCurve.h"

namespace engine
{
	// 0x494710
	void EaseCurve::init(float startSpeed, float endSpeed, float accelEnd, float decelStart)
	{
		float decelTime = 1.0f - decelStart;
		m_startSpeed = startSpeed;
		m_endSpeed = endSpeed;
		m_accelEnd = accelEnd;
		m_decelStart = decelStart;
		float cruiseTime = decelStart - accelEnd;
		m_cruiseSpeed = (2.0f - startSpeed * accelEnd - decelTime * endSpeed) / (cruiseTime + 1.0f);
		if (accelEnd == 0.0f)
			m_accel = 0.0f;
		else
			m_accel = (m_cruiseSpeed - startSpeed) / (accelEnd + accelEnd);
		if (decelStart == 1.0f)
			m_decel = 0.0f;
		else
			m_decel = (endSpeed - m_cruiseSpeed) / (decelTime + decelTime);
		m_accelEndPos = (m_accel * accelEnd + startSpeed) * accelEnd;
		m_decelStartPos = m_cruiseSpeed * cruiseTime + m_accelEndPos;
	}

	// 0x4947E0
	float EaseCurve::evaluate(float t) const
	{
		if (t < m_accelEnd)
			return (t * m_accel + m_startSpeed) * t;
		if (t < m_decelStart)
			return (t - m_accelEnd) * m_cruiseSpeed + m_accelEndPos;
		float decelT = t - m_decelStart;
		return (decelT * m_decel + m_cruiseSpeed) * decelT + m_decelStartPos;
	}
}

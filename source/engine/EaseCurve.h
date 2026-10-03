// engine::EaseCurve: an eased position over t in 0..1 (accelerate, cruise, decelerate).
#pragma once

namespace engine
{
	// Trapezoid velocity profile: the speed goes from m_startSpeed to the cruise speed until m_accelEnd, stays
	// there until m_decelStart, then goes to m_endSpeed at t = 1; evaluate(t) is the position (1 at t = 1). Used by
	// KeyframeCurve<T>::setupSegment/evaluate. No vtable, no constructor (0x24 bytes).
	class EaseCurve
	{
	public:
		void init(float startSpeed, float endSpeed, float accelEnd, float decelStart);
		float evaluate(float t) const;

		float m_startSpeed;						// +0x00
		float m_endSpeed;						// +0x04
		float m_accelEnd;						// +0x08 t where the acceleration ends
		float m_decelStart;						// +0x0C t where the deceleration starts
		float m_cruiseSpeed;					// +0x10 (2 - v0*t1 - (1 - t2)*v1) / (t2 - t1 + 1)
		float m_accel;							// +0x14 t^2 coefficient before m_accelEnd: (cruise - v0) / (2*t1), 0 if t1 is 0
		float m_decel;							// +0x18 t^2 coefficient after m_decelStart: (v1 - cruise) / (2*(1 - t2)), 0 if t2 is 1
		float m_accelEndPos;					// +0x1C position at m_accelEnd
		float m_decelStartPos;					// +0x20 position at m_decelStart
	};
}

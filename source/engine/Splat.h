// engine::Splat: a pop-up text or image made by the SplatFactory, animated over its life.
#pragma once

#include <string>

#include "Color.h"
#include "Container.h"
#include "Point.h"
#include "sigslot.h"

namespace engine
{
	// Moves by velocity, drag and acceleration or towards a target (moveTo), scales and fades over its life, then
	// SplatFactory calls fireFinished (callback, then m_onFinished with m_eventArg). Its content is its only child
	// (setContent, or a TextItem made by setText). Container +0x00, members from +0x128, then the vtordisp (+0x1C8)
	// and the Interface subobject (+0x1CC): 0x1D0 bytes. Implicit destructor (0x481A00 destroys the members without
	// the vfptr stores of a user-declared destructor).
	class Splat : public Container
	{
	public:
		Splat();

		virtual void setPosition(float x, float y);								// slot 6 (engine::Component): stores only
		virtual std::string getTypeName() const;								// slot 24 (engine::Component)
		virtual void update(UpdateContext& context);							// slot 37 (engine::Component)

		void setStartPosition(float x, float y);
		void moveTo(float x, float y);
		void setCallback(void (*callback)(void*), void* data);
		void setShadow(bool enabled, float alpha, Vector2 offset);	// the caller copies the offset into the argument
		void setContent(Component* content);
		void setText(const std::string& text);
		void fireFinished();

		sigslot::signal1<int> m_onFinished;		// +0x128 emitted by fireFinished with m_eventArg
		Vector2 m_velocity;						// +0x138 pixels per second
		float m_drag;							// +0x140 speed change per second along the velocity (negative stops at 0)
		float m_startScale;						// +0x144 1
		float m_endScale;						// +0x148 1
		float m_scaleStart;						// +0x14C life fraction where scaling starts
		float m_scaleEnd;						// +0x150 life fraction where scaling ends
		float m_startAlpha;						// +0x154 1
		float m_endAlpha;						// +0x158 1
		Vector2 m_acceleration;					// +0x15C added to the velocity each second
		Vector2 m_startPosition;				// +0x164
		Vector2 m_targetPosition;				// +0x16C moveTo target
		std::string m_fontName;					// +0x174 font of setText
		Color m_textColor;						// +0x190 white = untinted
		float m_lifeTime;						// +0x1A0 1
		float m_timeLeft;						// +0x1A4 1
		bool m_moveToTarget;					// +0x1A8 interpolate start -> target instead of moving by velocity
		void (*m_callback)(void*);				// +0x1AC called by fireFinished
		void* m_callbackData;					// +0x1B0
		int m_eventArg;							// +0x1B4 argument of m_onFinished
		bool m_shadow;							// +0x1B8 text drop shadow
		float m_shadowAlpha;					// +0x1BC
		Vector2 m_shadowOffset;					// +0x1C0
	};
}

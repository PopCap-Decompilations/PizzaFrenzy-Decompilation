// engine::Button: the abstract button (mouse states, hover scale, alpha pulse, sounds, listeners with commands).
#pragma once

#include <string>
#include <vector>

#include "Container.h"
#include "Point.h"

namespace engine
{
	class ButtonListener;
	class SoundHandle;

	// A listener added with Button::addListener and the command it is called with (0x20 bytes, allocated by
	// addListener, deleted by ~Button).
	struct ButtonListenerEntry
	{
		ButtonListener* listener;				// +0x00
		std::string command;					// +0x04
	};

	// Container +0x00, own members from +0x128, then the vtordisp (+0x17C) and the Interface subobject (+0x180):
	// 0x184 bytes. Mouse state machine: m_state 0 up, 1 hover, 2 pressed with the mouse outside, 3 pressed; the look
	// is refreshed by onStateChanged (ImageButton, Checkbox, TextButton). Does not override getTypeName.
	class Button : public Container
	{
	public:
		Button();
		virtual ~Button();

		virtual void activate();												// slot 34 (engine::Component)
		virtual void deactivate();												// slot 35 (engine::Component)
		virtual void update(UpdateContext& context);							// slot 37 (engine::Component)
		virtual void mouseDown(const MouseEvent& event);						// slot 40 (engine::Component)
		virtual void mouseUp(const MouseEvent& event);							// slot 43 (engine::Component)
		virtual void mouseMove(const MouseEvent& event);						// slot 52 (engine::Component): empty, folded 0x492310
		virtual void mouseEnter(const MouseEvent& event);						// slot 54 (engine::Component)
		virtual void mouseLeave(const MouseEvent& event);						// slot 57 (engine::Component)

		virtual void setBaseScale(Vector2 scale);								// slot 75: also Component::setScale(x, y)
		virtual void setNormalScale(Vector2 scale);								// slot 76
		virtual void setHoverScale(Vector2 scale);								// slot 77
		virtual void setScaleDuration(float seconds);							// slot 78
		virtual void addListener(ButtonListener* listener, std::string command);	// slot 79
		virtual void setSounds(const std::string& hoverSound, const std::string& clickSound);	// slot 80
		virtual void setPulsePeriod(float seconds);								// slot 81
		virtual void setHotspotMode(int mode);									// slot 82
		virtual void onStateChanged() = 0;										// slot 83
		virtual void fireMouseEnter();											// slot 84
		virtual void fireMouseLeave();											// slot 85
		virtual void fireMouseDown();											// slot 86
		virtual void fireMouseUp();												// slot 87
		virtual void fireClick();												// slot 88
		virtual void setPulseLevel(float level);								// slot 89
		virtual void endPulse();												// slot 90

		std::vector<ButtonListenerEntry*> m_listeners;	// +0x128
		SoundHandle* m_hoverSound;				// +0x138 from Application::createSound; played on mouse enter
		SoundHandle* m_clickSound;				// +0x13C played on mouse down
		int m_state;							// +0x140 0 up, 1 hover, 2 pressed with the mouse outside, 3 pressed
		bool m_scaleEffect;						// +0x144 normal scale != hover scale
		Vector2 m_normalScale;					// +0x148 (1, 1)
		Vector2 m_hoverScale;					// +0x150 (1, 1)
		Vector2 m_baseScale;					// +0x158 multiplied by the normal/hover scale
		float m_scaleTimer;						// +0x160
		float m_scaleDuration;					// +0x164 0.5
		int m_scalePhase;						// +0x168 0 at hover scale, 1 growing, 2 shrinking, 3 at normal scale (initial)
		float m_pulseTimer;						// +0x16C
		float m_pulsePeriod;					// +0x170
		bool m_pulseFalling;					// +0x174 true: level = timer/period (1 to 0), false: 1 - that (initial true)
		bool m_pulsing;							// +0x175
		int m_hotspotMode;						// +0x178 handed to the images by ImageButton::setImages
	};
}

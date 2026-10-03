// engine::WaveEffect: the sine-wave distortion the blitters apply to an image (Component::setEffect, handed to the
// software drawImage through the graphics state).
#pragma once

#include "Object.h"

namespace engine
{
	// Held in RefPtrs (Component +0x68, GraphicsState +0x2C), so an Object: its fields start at +0x0C. No instance is
	// created in this game and nothing sets its fields; only the getters below exist (the blitters call them). Row r
	// of a type-1 blit is shifted right by m_amplitude * (sin(m_frequency * (r + y) + m_phase) + 1) pixels.
	class WaveEffect : public Object
	{
	public:
		int getType() const;					// 0x411960 (folded): m_type
		float getAmplitude() const;
		float getFrequency() const;
		float getPhase() const;

		int m_type;								// +0x0C 1 = horizontal wave, the only type the blitters know
		float m_phase;							// +0x10 radians
		float m_amplitude;						// +0x14 pixels
		float m_frequency;						// +0x18 radians per row
	};
}

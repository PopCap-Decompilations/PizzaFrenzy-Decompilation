// engine::HsvFilter: a pixel filter that adds a hue/saturation/value offset.
#pragma once

#include "Object.h"
#include "PixelFilter.h"

namespace engine
{
	// PixelFilter is the non-virtual primary base (vfptr +0x00, vbptr +0x04), Object is virtual: members from +0x08,
	// then the vtordisp (+0x14), the Interface subobject (+0x18) and Object (+0x1C): 0x28 bytes. Recolours images
	// (e.g. the kitchen marquee variants). No user-declared destructor.
	class HsvFilter : public PixelFilter, public virtual Object
	{
	public:
		HsvFilter();

		virtual void filter(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a,
			unsigned char* outR, unsigned char* outG, unsigned char* outB, unsigned char* outA);	// slot 0 (engine::PixelFilter)

		void set(float hue, float saturation, float value);	// folded with AlphaPulse::set (0x47B110)
		void rgbToHsv(unsigned char r, unsigned char g, unsigned char b, float* h, float* s, float* v);	// this unused
		void hsvToRgb(float h, float s, float v, unsigned char* r, unsigned char* g, unsigned char* b);	// this unused

		float m_hue;							// +0x08 degrees added (wraps to [0, 360))
		float m_saturation;						// +0x0C added, result clamped to [0, 1]
		float m_value;							// +0x10 added, result clamped to [0, 1]
	};
}

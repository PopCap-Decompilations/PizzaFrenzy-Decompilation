#include "HsvFilter.h"

#include <algorithm>
#include <cmath>

namespace engine
{
	// 0x47B110 (folded)
	void HsvFilter::set(float hue, float saturation, float value)
	{
		m_hue = hue;
		m_saturation = saturation;
		m_value = value;
	}

	// 0x47B130
	void HsvFilter::rgbToHsv(unsigned char r, unsigned char g, unsigned char b, float* h, float* s, float* v)
	{
		float red = r * (1.0f / 255.0f);
		float green = g * (1.0f / 255.0f);
		float blue = b * (1.0f / 255.0f);
		float minimum = (std::min)((std::min)(red, green), blue);
		float maximum = (std::max)((std::max)(red, green), blue);
		*v = maximum;
		float delta = maximum - minimum;
		if (maximum != 0.0f)
		{
			*s = delta / maximum;
		}
		else
		{
			*s = 0.0f;
			*h = -1.0f;
			return;
		}
		if (delta == 0.0f)
		{
			*h = 0.0f;
			return;
		}
		if (red == maximum)
			*h = (green - blue) / delta;
		else if (green == maximum)
			*h = 2.0f + (blue - red) / delta;
		else
			*h = 4.0f + (red - green) / delta;
		*h *= 60.0f;
		if (*h < 0.0f)
			*h += 360.0f;
	}

	// 0x47B2D0
	void HsvFilter::hsvToRgb(float h, float s, float v, unsigned char* r, unsigned char* g, unsigned char* b)
	{
		if (s == 0.0f)
		{
			*r = *g = *b = (unsigned char)(v * 255.0f);
			return;
		}
		h *= 1.0f / 60.0f;
		int i = (int)floor(h);
		float f = h - i;
		float p = v * (1.0f - s);
		float q = v * (1.0f - s * f);
		float t = v * (1.0f - s * (1.0f - f));
		float red;
		float green;
		float blue;
		switch (i)
		{
		case 0:
			red = v;
			green = t;
			blue = p;
			break;
		case 1:
			red = q;
			green = v;
			blue = p;
			break;
		case 2:
			red = p;
			green = v;
			blue = t;
			break;
		case 3:
			red = p;
			green = q;
			blue = v;
			break;
		case 4:
			red = t;
			green = p;
			blue = v;
			break;
		default:
			red = v;
			green = p;
			blue = q;
			break;
		}
		*r = (unsigned char)(red * 255.0f);
		*g = (unsigned char)(green * 255.0f);
		*b = (unsigned char)(blue * 255.0f);
	}

	// 0x47B460
	HsvFilter::HsvFilter()
		: m_hue(0.0f), m_saturation(0.0f), m_value(0.0f)
	{
	}

	// 0x47B550
	void HsvFilter::filter(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a,
		unsigned char* outR, unsigned char* outG, unsigned char* outB, unsigned char* outA)
	{
		float h;
		float s;
		float v;
		rgbToHsv(r, g, b, &h, &s, &v);
		h += m_hue;
		s += m_saturation;
		v += m_value;
		while (h > 360.0f)
			h -= 360.0f;
		while (h < 0.0f)
			h += 360.0f;
		if (s < 0.0f)
			s = 0.0f;
		else if (s > 1.0f)
			s = 1.0f;
		if (v < 0.0f)
			v = 0.0f;
		else if (v > 1.0f)
			v = 1.0f;
		unsigned char red;
		unsigned char green;
		unsigned char blue;
		hsvToRgb(h, s, v, &red, &green, &blue);
		*outR = red;
		*outG = green;
		*outB = blue;
		*outA = a;
	}
}

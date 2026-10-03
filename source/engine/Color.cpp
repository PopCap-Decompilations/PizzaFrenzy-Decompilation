#include "Color.h"

namespace engine
{
	// 0x47E4B0: white
	Color::Color()
		: r(1.0f), g(1.0f), b(1.0f), a(1.0f)
	{
	}

	// 0x47E4D0
	Color::Color(const Color& other)
		: r(other.r), g(other.g), b(other.b), a(other.a)
	{
	}

	// 0x47E4F0
	Color::Color(float r, float g, float b)
		: r(r), g(g), b(b), a(1.0f)
	{
	}

	// 0x470B80 (folded)
	Color::Color(float r, float g, float b, float a)
		: r(r), g(g), b(b), a(a)
	{
	}

	// 0x47E510: alpha unchanged
	void Color::set(float r, float g, float b)
	{
		this->r = r;
		this->g = g;
		this->b = b;
	}

	// 0x47E530: 0..255, alpha unchanged
	void Color::set(int r, int g, int b)
	{
		this->r = r * (1.0f / 255.0f);
		this->g = g * (1.0f / 255.0f);
		this->b = b * (1.0f / 255.0f);
	}

	// 0x47E560: 0..255
	void Color::set(int r, int g, int b, int a)
	{
		this->r = r * (1.0f / 255.0f);
		this->g = g * (1.0f / 255.0f);
		this->b = b * (1.0f / 255.0f);
		this->a = a * (1.0f / 255.0f);
	}

	// 0x47E5A0: 0..255, alpha 1
	Color::Color(int r, int g, int b)
		: r(r * (1.0f / 255.0f)), g(g * (1.0f / 255.0f)), b(b * (1.0f / 255.0f)), a(1.0f)
	{
	}

	// 0x47E5E0: 0..255
	Color::Color(int r, int g, int b, int a)
		: r(r * (1.0f / 255.0f)), g(g * (1.0f / 255.0f)), b(b * (1.0f / 255.0f)), a(a * (1.0f / 255.0f))
	{
	}

	// 0x4D0470 (folded): the empty body shared by every empty function without stack arguments
	Color::~Color()
	{
	}

	// 0x470920 (folded): one body with engine::Rect::set (four dwords stored in order)
	void Color::set(float r, float g, float b, float a)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}
}

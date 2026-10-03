// engine::Color: a colour with float components 0..1 (plain value type, 16 bytes).
#pragma once

namespace engine
{
	// The int overloads take 0..255 and scale by 1/255. The copy constructor and the empty destructor are
	// user-declared and out of line: the destructors of the classes that hold a Color call ~Color (its body is
	// the folded empty function 0x4D0470).
	struct Color
	{
		Color();
		Color(const Color& other);
		Color(float r, float g, float b);
		Color(float r, float g, float b, float a);	// folded with engine::Rect::Rect (0x470B80)
		Color(int r, int g, int b);
		Color(int r, int g, int b, int a);
		~Color();

		void set(float r, float g, float b);
		void set(float r, float g, float b, float a);	// folded with engine::Rect::set (0x470920), used by Graphics::setColor
		void set(int r, int g, int b);
		void set(int r, int g, int b, int a);

		float r;								// +0x00
		float g;								// +0x04
		float b;								// +0x08
		float a;								// +0x0C
	};
}

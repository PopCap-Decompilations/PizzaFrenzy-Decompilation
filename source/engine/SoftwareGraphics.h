// engine::SoftwareGraphics: the software engine::Graphics that draws into an engine::Surface (the display each
// frame, or a MemoryImage from Application::createGraphics) with E07's blitters; and floatEquals, which they call.
#pragma once

#include "Graphics.h"
#include "Object.h"

namespace engine
{
	class Bitmap;
	struct IntRect;
	class Surface;
	struct Vector2;

	class SoftwareGraphics : public Object, public Graphics
	{
	public:
		SoftwareGraphics(Surface* surface);
		virtual ~SoftwareGraphics();

		// Graphics (the rects are IntRects: IntRect::offset/intersect/isEmpty in 4BF420 and 4BFC20, int fields)
		virtual void drawImage(Bitmap* image, const IntRect& source, float x, float y);	// slot 0
		virtual void drawImage(Bitmap* image, float x, float y);							// slot 1
		virtual void drawImage(Bitmap* image);												// slot 2
		virtual void drawRect(const IntRect& rect);											// slot 6
		virtual void drawRect(float width, float height);									// slot 7
		virtual void fillRect(const IntRect& rect);											// slot 8
		virtual void fillRect(float width, float height);									// slot 9
		virtual void drawLine(const Vector2& from, const Vector2& to);						// slot 10
		virtual void drawCircle(const Vector2& center, float radius);						// slot 11

		Surface* m_surface;						// +0x28 the target: a raw pointer, addRef'd by the constructor and released by the destructor
	};

	// |a - b| < 2.38418579e-07f (out of line here; inlined in drawImage and fillRect).
	bool floatEquals(float a, float b);
}

#include <math.h>

#include <algorithm>

#include "SoftwareGraphics.h"

#include "Blitter.h"
#include "Color.h"
#include "Point.h"
#include "Rect.h"
#include "Surface.h"

namespace engine
{
	// 0x4BF200
	SoftwareGraphics::~SoftwareGraphics()
	{
		m_surface->release();
	}

	// 0x4BF2A0
	void SoftwareGraphics::drawImage(Bitmap* image)
	{
		drawImage(image, IntRect(0, 0, image->getWidth(), image->getHeight()), 0.0f, 0.0f);
	}

	// 0x4BF320
	void SoftwareGraphics::drawImage(Bitmap* image, float x, float y)
	{
		drawImage(image, IntRect(0, 0, image->getWidth(), image->getHeight()), x, y);
	}

	// 0x4BF3A0
	void SoftwareGraphics::drawRect(float width, float height)
	{
		drawRect(IntRect(0, 0, (int)width, (int)height));
	}

	// 0x4BF420
	void SoftwareGraphics::drawRect(const IntRect& rect)
	{
		const Vector2& translation = getTranslation();
		IntRect r = rect;
		r.offset((int)translation.x, (int)translation.y);
		IntRect bounds(0, 0, m_surface->getWidth(), m_surface->getHeight());
		IntRect clipped;
		clipped.intersect(bounds, r);
		if (!clipped.isEmpty())
		{
			const Color& color = getColor();
			float alpha = getAlpha();
			Pixel pixel(color);
			PixelBuffer* buffer = m_surface->lock(clipped);
			drawRectOutline(*buffer, pixel, AlphaBlend((int)(alpha * 255.0f)));
		}
	}

	// 0x4BF570
	void SoftwareGraphics::fillRect(float width, float height)
	{
		fillRect(IntRect(0, 0, (int)width, (int)height));
	}

	// 0x4BF5F0
	void SoftwareGraphics::drawCircle(const Vector2& center, float radius)
	{
		const Color& color = getColor();
		float alpha = getAlpha();
		Pixel pixel(color);
		PixelBuffer* buffer = m_surface->lock();
		const Vector2& translation = getTranslation();		// unused: the circle ignores the translation
		engine::drawCircle(*buffer, pixel, center, radius, AlphaBlend((int)(alpha * 255.0f)));
	}

	// 0x4BF670
	SoftwareGraphics::SoftwareGraphics(Surface* surface)
		: m_surface(surface)
	{
		m_surface->addRef();
		setClipRect(IntRect(0, 0, m_surface->getWidth(), m_surface->getHeight()));
	}

	// 0x4BF790
	void SoftwareGraphics::drawLine(const Vector2& from, const Vector2& to)
	{
		const Color& color = getColor();
		float alpha = getAlpha();
		Pixel pixel(color);
		PixelBuffer* buffer = m_surface->lock();
		const Vector2& translation = getTranslation();
		Vector2 a(from);
		Vector2 b(to);
		a += translation;
		b += translation;
		// (sic) the right end is tested with max(a.x, a.x): b.x is not checked against the width
		if (buffer->m_height >= (std::max)(a.y, b.y) && 0.0f <= (std::min)(a.y, b.y)
			&& buffer->m_width >= (std::max)(a.x, a.x) && 0.0f <= (std::min)(a.x, b.x))
		{
			engine::drawLine(*buffer, pixel, a, b, AlphaBlend((int)(alpha * 255.0f)));
		}
	}

	// 0x4BF8C0
	bool floatEquals(float a, float b)
	{
		return fabs(a - b) < 2.38418579e-07f;
	}

	// 0x4BF8E0
	void SoftwareGraphics::drawImage(Bitmap* image, const IntRect& source, float x, float y)
	{
		Surface* surface = static_cast<Surface*>(image);
		float rotation = getRotation();
		float scaleX = getScaleX();
		float scaleY = getScaleY();
		float alpha = getAlpha();
		const IntRect& clipRect = getClipRect();
		const Vector2& translation = getTranslation();
		x += translation.x;
		y += translation.y;
		// Not set for other smoothing values (the original then passes whatever its register held: y's low byte).
		int smoothFlags;
		switch (getSmoothing())
		{
		case 0:
			x = (float)floor(x);
			y = (float)floor(y);
			smoothFlags = 0;
			break;
		case 1:
			smoothFlags = 15;
			break;
		}
		if (getColorMode() == 1)
		{
			const Color& color = getColor();
			engine::drawImage(m_surface, clipRect, surface, source, x, y, scaleX, scaleY, rotation, smoothFlags,
				AddColorBlend((int)(color.r * 255.0f), (int)(color.g * 255.0f), (int)(color.b * 255.0f),
				(int)(alpha * 255.0f)), getWave());
		}
		else if (getColorMode() == 2)
		{
			const Color& color = getColor();
			engine::drawImage(m_surface, clipRect, surface, source, x, y, scaleX, scaleY, rotation, smoothFlags,
				MulColorBlend((int)(color.r * 255.0f), (int)(color.g * 255.0f), (int)(color.b * 255.0f),
				(int)(alpha * 255.0f)), getWave());
		}
		else if (surface->getAlphaType() != 2 && floatEquals(alpha, 1.0f))
		{
			if (surface->getAlphaType() == 1)
			{
				engine::drawImage(m_surface, clipRect, surface, source, x, y, scaleX, scaleY, rotation, smoothFlags,
					AlphaTestBlend(surface->getAlphaThreshold()), getWave());
			}
			else
			{
				engine::drawImage(m_surface, clipRect, surface, source, x, y, scaleX, scaleY, rotation, smoothFlags,
					CopyBlend(), getWave());
			}
		}
		else
		{
			engine::drawImage(m_surface, clipRect, surface, source, x, y, scaleX, scaleY, rotation, smoothFlags,
				AlphaBlend((int)(alpha * 255.0f)), getWave());
		}
	}

	// 0x4BFC20
	void SoftwareGraphics::fillRect(const IntRect& rect)
	{
		const Vector2& translation = getTranslation();
		IntRect r = rect;
		r.offset((int)translation.x, (int)translation.y);
		const IntRect& clipRect = getClipRect();
		IntRect clipped;
		clipped.intersect(clipRect, r);
		if (!clipped.isEmpty())
		{
			PixelBuffer* buffer = m_surface->lock(clipped);
			const Color& color = getColor();
			float alpha = getAlpha();
			Pixel pixel(color);
			if (floatEquals(alpha, 1.0f) && pixel.a >= 255)
			{
				engine::fillRect(*buffer, pixel, CopyBlend());
			}
			else
			{
				engine::fillRect(*buffer, pixel, AlphaBlend((int)(alpha * 255.0f)));
			}
		}
	}
}

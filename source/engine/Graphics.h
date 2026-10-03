// engine::Graphics, the abstract 2D drawing context with its push/pop state stack, and engine::GraphicsState.
#pragma once

#include <deque>

#include "Color.h"
#include "Interface.h"
#include "Point.h"
#include "Rect.h"
#include "RefPtr.h"

namespace engine
{
	class Bitmap;
	class Font;
	class WaveEffect;

	// One entry of Graphics' state stack (value type, 0x44 bytes). The copy constructor is implicit (0x477970).
	struct GraphicsState
	{
		GraphicsState();
		~GraphicsState();

		Vector2 m_translation;					// +0x00 (0, 0)
		Vector2 m_scale;						// +0x08 (1, 1)
		float m_rotation;						// +0x10 0
		float m_alpha;							// +0x14 1
		Color m_color;							// +0x18 Color(0, 0, 0): black, alpha 1
		RefPtr<Font> m_font;					// +0x28 font of drawString
		RefPtr<WaveEffect> m_wave;				// +0x2C sine-wave distortion handed to the blitters (Component::setEffect)
		IntRect m_clipRect;						// +0x30 empty; pixels (SoftwareGraphics intersects IntRects with it)
		unsigned int m_flags;					// +0x40 hAlign bits 0-1, vAlign 2-3, colour mode 4-5, smoothing 6

		// The bit fields of m_flags (initialised data, read at run time).
		static unsigned int s_hAlignMask;		// 0x521300: 3
		static unsigned int s_vAlignMask;		// 0x521304: 0xC
		static unsigned int s_colorModeMask;	// 0x521308: 0x30
		static unsigned int s_smoothMask;		// 0x52130C: 0x40
		static int s_hAlignShift;				// 0x521310: 0
		static int s_vAlignShift;				// 0x521314: 2
		static int s_colorModeShift;			// 0x521318: 4
		static int s_smoothShift;				// 0x52131C: 6
	};

	// Not an engine::Object and not reference counted itself: vfptr +0x00, vbptr +0x04, m_states +0x08, Interface
	// +0x1C (0x20 bytes in the original; no vtordisp). E06's engine::SoftwareGraphics implements the pure slots
	// (Object +0x00, Graphics +0x0C). The state setters and getters work on the top of m_states, which is never
	// empty. The rects of slots 0, 6 and 8 and the clip rect are pixel IntRects (Font's glyph rects, IntRect::offset/
	// intersect/isEmpty in SoftwareGraphics). MSVC groups overloaded virtuals and reverses their order, so the
	// overloads are declared here in the reverse of their slot order.
	class Graphics : public virtual Interface
	{
	public:
		Graphics();

		virtual void drawImage(Bitmap* image) = 0;												// slot 2: whole image at (0, 0)
		virtual void drawImage(Bitmap* image, float x, float y) = 0;							// slot 1: whole image
		virtual void drawImage(Bitmap* image, const IntRect& source, float x, float y) = 0;	// slot 0: part of an image
		virtual void drawString(const char* text);												// slot 4: drawString(text, 0, 0)
		virtual void drawString(const char* text, float x, float y);							// slot 3: glyph by glyph
		virtual void drawChar(char c);															// slot 5
		virtual void drawRect(float width, float height) = 0;									// slot 7: outline of (0, 0, w, h)
		virtual void drawRect(const IntRect& rect) = 0;											// slot 6: outline
		virtual void fillRect(float width, float height) = 0;									// slot 9: fill (0, 0, w, h)
		virtual void fillRect(const IntRect& rect) = 0;											// slot 8
		virtual void drawLine(const Vector2& from, const Vector2& to) = 0;						// slot 10
		virtual void drawCircle(const Vector2& center, float radius) = 0;						// slot 11
		virtual ~Graphics();																	// slot 12

		void pushState();
		void popState();

		void setTranslation(float x, float y);
		void translate(float dx, float dy);
		const Vector2& getTranslation() const;
		void setScale(float sx, float sy);
		void scale(float sx, float sy);
		float getScaleX() const;
		float getScaleY() const;
		void rotate(float angle);
		float getRotation() const;
		void setAlpha(float alpha);
		void multiplyAlpha(float factor);
		float getAlpha() const;
		void setColor(const Color& color);
		void setColor(float r, float g, float b);
		void setColor(float r, float g, float b, float a);
		const Color& getColor() const;
		void setColorMode(int mode);
		int getColorMode() const;
		void setFont(Font* font);
		Font* getFont() const;
		void setWave(WaveEffect* wave);
		WaveEffect* getWave() const;
		void setHAlign(int align);								// 0 left, 1 centre, 2 right
		void setVAlign(int align);								// 0 top, 1 middle, 2 bottom
		void setSmoothing(int smooth);							// 0 snaps blits to whole pixels, 1 sub-pixel/filtered
		int getSmoothing() const;
		void setClipRect(const IntRect& rect);
		const IntRect& getClipRect() const;

		std::deque<GraphicsState> m_states;		// +0x08 state stack
	};
}

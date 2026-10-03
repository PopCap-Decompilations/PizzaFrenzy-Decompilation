// engine::Bitmap (the engine's image interface), engine::Surface (the lockable 32-bit BGRA pixel surface that
// implements it: base of the display and of the in-memory images), engine::Pixel and engine::PixelBuffer (the
// pixels of a locked rectangle).
#pragma once

#include "Interface.h"
#include "Object.h"
#include "Point.h"

namespace engine
{
	struct Color;
	struct IntRect;
	class PixelFilter;

	// 32-bit pixel, bytes in memory order B, G, R, A (A8R8G8B8).
	struct Pixel
	{
		Pixel();								// 0x4D1360 (folded): empty, the bytes are left uninitialised
		Pixel(const Color& color);				// each component * 255, truncated

		unsigned char b;						// +0x00
		unsigned char g;						// +0x01
		unsigned char r;						// +0x02
		unsigned char a;						// +0x03
	};

	// A locked rectangle: m_height rows of m_width pixels, m_pitch pixels apart.
	struct PixelBuffer
	{
		// The PNG reader's row copies, for m_width pixels: B, G, R from 3-byte RGB (alpha kept); A from the last
		// byte of each srcStride-byte source pixel.
		void copyRgbRow(unsigned char* dstRow, const unsigned char* rgb) const;
		void copyAlphaRow(unsigned char* dstRow, const unsigned char* src, int srcStride) const;

		int m_width;							// +0x00
		int m_height;							// +0x04
		int m_pitch;							// +0x08 in pixels
		Pixel* m_pixels;						// +0x0C first pixel; a DDrawDisplay is locked while it is not NULL
	};

	// The image interface the application's image loaders return, engine::Image components hold and the graphics
	// draw: sixteen pure virtuals, no destructor (images are released through the Interface). Its implicit
	// constructor is inlined in Surface's. {vfptr, vbptr} before the virtual Interface.
	class Bitmap : public virtual Interface
	{
	public:
		virtual int getWidth() const = 0;													// slot 0
		virtual int getHeight() const = 0;													// slot 1
		virtual void resize(int width, int height, int mode) = 0;							// slot 2: never called; empty in every image
		virtual void setPivotType(int pivotType) = 0;										// slot 3: 1 "center" = (width/2, height/2), else "upperleft" (0, 0)
		virtual void setPivot(int x, int y) = 0;											// slot 4: the point placed at the draw position; rotation and scale centre
		virtual const Point& getPivot() const = 0;											// slot 5
		virtual void setAlphaType(int alphaType, unsigned char alphaThreshold) = 0;		// slot 6: 0 none, 1 alphaTest (alpha > threshold drawn), 2 alphaBlend
		virtual int getAlphaType() const = 0;												// slot 7
		virtual void fill(const Color& color) = 0;											// slot 8
		virtual Bitmap* copyFilteredScaled(float scale, PixelFilter* filter) = 0;			// slot 9: copyFiltered(filter), then scale(scale, scale) on the copy
		virtual Bitmap* copyFiltered(PixelFilter* filter) = 0;								// slot 10: a new image of filter(x, y, r, g, b, a) of each pixel
		virtual Bitmap* copyScaled(float scaleX, float scaleY) = 0;						// slot 11: copy(), then scale(scaleX, scaleY) on the copy
		virtual Bitmap* copyScaledUniform(float scale) = 0;								// slot 12: copyScaled(scale, scale)
		virtual Bitmap* copy() = 0;															// slot 13: same pixels, alpha type, threshold and pivot
		virtual void scale(float scaleX, float scaleY) = 0;								// slot 14: resamples in place (Mitchell filter) and scales the pivot
		virtual void applyFilter(PixelFilter* filter) = 0;									// slot 15: copyFiltered in place
	};

	// Abstract: lock() returns &m_buffer describing the locked pixels. The display (DDrawDisplay) and the in-memory
	// images (MemoryImage) implement it. Layout: Object +0x00, Bitmap +0x0C (vfptr, vbptr), members +0x14..+0x33,
	// then the vtordisp and Interface (0x3C bytes).
	class Surface : public Object, public Bitmap
	{
	public:
		Surface();
		virtual ~Surface();

		// MSVC groups overloaded virtuals and reverses their order: declared lock() then lock(rect), they take slots
		// 2 and 1.
		virtual PixelBuffer* lock() = 0;								// slot 2: the whole surface
		virtual PixelBuffer* lock(const IntRect& rect) = 0;			// slot 1: a rectangle; DDrawDisplay returns NULL on failure
		virtual void unlock() = 0;										// slot 3
		virtual unsigned char getAlphaThreshold() const;				// slot 4

		// Bitmap
		virtual void resize(int width, int height, int mode);			// 0x455B10 (folded): empty
		virtual void setPivotType(int pivotType);
		virtual void setPivot(int x, int y);
		virtual const Point& getPivot() const;							// 0x467E60 (folded): m_pivot
		virtual void setAlphaType(int alphaType, unsigned char alphaThreshold);
		virtual int getAlphaType() const;
		virtual void fill(const Color& color);							// 0x492310 (folded): empty
		virtual void applyFilter(PixelFilter* filter);					// 0x492310 (folded): empty

		int m_alphaType;						// +0x14 0 none, 1 alphaTest, 2 alphaBlend; 0 after construction
		unsigned char m_alphaThreshold;			// +0x18 alphaTest: a source pixel is drawn when its alpha > m_alphaThreshold
												// +0x19 (padding)
		Point m_pivot;							// +0x1C in pixels; (0, 0) after construction
		PixelBuffer m_buffer;					// +0x24 the rectangle of the last lock(); not initialised by the constructor
	};
}

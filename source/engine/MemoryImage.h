// engine::MemoryImage: a 32-bit BGRA engine::Surface in system memory, the application's images (JPEG and PNG
// loading with an optional "<dir>_<name>.png" alpha mask, blank images, copies, scaling, per-pixel filters).
#pragma once

#include "Surface.h"

namespace engine
{
	class Bitmap;
	struct Color;
	struct IntRect;
	class PixelFilter;

	class MemoryImage : public Surface
	{
	public:
		MemoryImage();
		virtual ~MemoryImage();

		// Surface (the lock rect is an IntRect: 4BE7E0 calls IntRect::getWidth/getHeight)
		virtual PixelBuffer* lock(const IntRect& rect);
		virtual PixelBuffer* lock();
		virtual void unlock();

		// Bitmap (slots 0 and 1 are folded bodies: 0x4BE750, 0x4BE760)
		virtual int getWidth() const;
		virtual int getHeight() const;
		virtual void fill(const Color& color);
		virtual Bitmap* copyFilteredScaled(float scale, PixelFilter* filter);
		virtual Bitmap* copyFiltered(PixelFilter* filter);
		virtual Bitmap* copyScaled(float scaleX, float scaleY);
		virtual Bitmap* copyScaledUniform(float scale);
		virtual Bitmap* copy();
		virtual void scale(float scaleX, float scaleY);
		virtual void applyFilter(PixelFilter* filter);

		void create(int width, int height);
		void load(const char* fileName, bool fromFile);

		int m_width;							// +0x34 pixels; also the pitch of the lock record
		int m_height;							// +0x38
		Pixel* m_pixels;						// +0x3C new Pixel[m_width * m_height]; create() does not free a previous array
	};
}

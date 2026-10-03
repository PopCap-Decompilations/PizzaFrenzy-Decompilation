// engine::PngReader: reads a PNG image from an engine::InputStream into a locked engine::PixelBuffer with libpng
// 1.2.5 (the whole image, its colour only, or the alpha channel of a separate mask image).
#pragma once

#include "png.h"

#include "RefPtr.h"

namespace engine
{
	class InputStream;
	struct PixelBuffer;

	// No vtable: a local of MemoryImage::load, which opens it, sizes the image from it and reads into the image's
	// locked pixels. libpng's errors throw engine::Exception (error), its warnings are ignored. Every libpng failure
	// in open() throws engine::Exception().
	class PngReader
	{
	public:
		PngReader();							// 0x470B20 (folded): all four members 0
		~PngReader();							// close()

		void open(InputStream* stream);			// checks the signature, creates the libpng structures, reads the header
		void read(const PixelBuffer* dst);		// the whole image as BGRA (gray expanded, alpha filled with 0xFF when absent)
		void readRgb(const PixelBuffer* dst);	// colour only, dst's alpha kept (the image has a separate alpha mask)
		void readAlpha(const PixelBuffer* dst);	// the mask image: its gray (or last) channel into dst's alpha
		int getWidth() const;
		int getHeight() const;
		int getAlphaType() const;				// 2 (alphaBlend) when the colour type has an alpha channel, else 0
		void close();

		// libpng callbacks
		static void readData(png_structp png, png_bytep data, png_size_t length);	// reads from the io pointer's InputStream
		static void error(png_structp png, png_const_charp message);				// throws Exception(message)
		static void warning(png_structp png, png_const_charp message);				// 0x4D0470 (folded): empty

		RefPtr<InputStream> m_stream;			// +0x00 the stream given to open()
		png_structp m_png;						// +0x04
		png_infop m_info;						// +0x08
		png_infop m_endInfo;					// +0x0C
	};
}

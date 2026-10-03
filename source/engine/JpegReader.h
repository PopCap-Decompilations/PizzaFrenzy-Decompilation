// engine::JpegReader: reads a JPEG image from an engine::InputStream into a locked engine::PixelBuffer with libjpeg
// 6b (built with boolean = unsigned char), through a source manager adapted from IJG's jdatasrc.c.
#pragma once

#include <stdio.h>

// jmorecfg.h's INT16/INT32 typedefs clash with <windows.h>'s (INT32 is long there, int in basetsd.h); jpeglib.h does
// not use them, so they are skipped the way X11's xmd.h has them skipped.
#ifndef XMD_H
#define XMD_H
#endif

extern "C"
{
#include "jpeglib.h"
}

#include "RefPtr.h"

namespace engine
{
	class InputStream;
	struct PixelBuffer;

	// No vtable: a local of MemoryImage::load. libjpeg's errors use its default error_exit.
	class JpegReader
	{
	public:
		// jdatasrc.c's my_source_mgr over an InputStream (4096-byte buffer); its implicit destructor releases m_stream.
		struct Source
		{
			jpeg_source_mgr pub;				// +0x00 libjpeg's source manager (cinfo.src points here)
			RefPtr<InputStream> m_stream;		// +0x1C
			JOCTET* m_buffer;					// +0x20 4096 bytes from the JPOOL_PERMANENT pool
			boolean m_startOfFile;				// +0x24 no data read yet (an empty file is an error, a short one a warning)
												// +0x25 (padding)
		};

		JpegReader();							// only m_source's constructor
		~JpegReader();							// jpeg_destroy_decompress when a stream is set

		void open(InputStream* stream);			// creates the decompressor, reads the header, starts decompression (RGB output)
		void setInputStream(InputStream* stream);
		boolean read(const PixelBuffer* dst);	// every scanline as BGRA with alpha 0xFF; jpeg_finish_decompress's result (not normalised to bool)
		int getWidth() const;					// output_width
		int getHeight() const;					// 0x4D0CA0 (folded): output_height

		// libjpeg source manager callbacks
		static void initSource(j_decompress_ptr cinfo);
		static boolean fillInputBuffer(j_decompress_ptr cinfo);
		static void skipInputData(j_decompress_ptr cinfo, long numBytes);
		static void termSource(j_decompress_ptr cinfo);				// 0x4D0470 (folded): empty

		jpeg_decompress_struct m_decompress;	// +0x000 0x1B0 bytes
		jpeg_error_mgr m_errorManager;			// +0x1B0 jpeg_std_error's
		Source m_source;						// +0x234
												// +0x25C (padding: m_decompress's doubles make the size a multiple of 8, 0x260)
	};
}

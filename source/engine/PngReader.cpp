// engine::PngReader: libpng 1.2.5 reading from an engine::InputStream into a locked engine::PixelBuffer.
#include <string>

#include "PngReader.h"

#include "Exception.h"
#include "InputStream.h"
#include "Surface.h"

namespace engine
{
	// 0x470B20 (folded)
	PngReader::PngReader()
		: m_png(NULL), m_info(NULL), m_endInfo(NULL)
	{
	}

	// 0x4D0470 (folded)
	void PngReader::warning(png_structp png, png_const_charp message)
	{
	}

	// 0x4D0480
	void PngReader::readData(png_structp png, png_bytep data, png_size_t length)
	{
		InputStream* stream = (InputStream*)png_get_io_ptr(png);
		stream->read(data, length);
	}

	// 0x4D04A0
	void PngReader::read(const PixelBuffer* dst)
	{
		png_uint_32 width, height;
		int bitDepth, colorType, interlaceType, compressionType, filterType;
		png_get_IHDR(m_png, m_info, &width, &height, &bitDepth, &colorType, &interlaceType, &compressionType, &filterType);
		png_get_channels(m_png, m_info);
		png_get_rowbytes(m_png, m_info);
		png_set_expand(m_png);
		if (colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
			png_set_gray_to_rgb(m_png);
		png_set_filler(m_png, 0xFF, PNG_FILLER_AFTER);
		if (bitDepth == 16)
			png_set_strip_16(m_png);
		png_set_bgr(m_png);
		int passes = png_set_interlace_handling(m_png);
		png_read_update_info(m_png, m_info);
		for (int pass = 0; pass < passes; pass++)
		{
			for (unsigned int y = 0; y < (unsigned int)dst->m_height; y++)
				png_read_row(m_png, (png_bytep)(dst->m_pixels + y * dst->m_pitch), NULL);
		}
		png_read_end(m_png, m_endInfo);
	}

	// 0x4D05C0
	void PngReader::readRgb(const PixelBuffer* dst)
	{
		png_uint_32 width, height;
		int bitDepth, colorType, interlaceType, compressionType, filterType;
		png_get_IHDR(m_png, m_info, &width, &height, &bitDepth, &colorType, &interlaceType, &compressionType, &filterType);
		png_get_channels(m_png, m_info);
		png_get_rowbytes(m_png, m_info);
		png_set_expand(m_png);
		if (colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
			png_set_gray_to_rgb(m_png);
		if (bitDepth == 16)
			png_set_strip_16(m_png);
		if (colorType & PNG_COLOR_MASK_ALPHA)
			png_set_strip_alpha(m_png);
		int passes = png_set_interlace_handling(m_png);
		png_read_update_info(m_png, m_info);
		png_get_IHDR(m_png, m_info, &width, &height, &bitDepth, &colorType, &interlaceType, &compressionType, &filterType);
		png_get_channels(m_png, m_info);
		png_bytep row = new png_byte[png_get_rowbytes(m_png, m_info)];
		for (int pass = 0; pass < passes; pass++)
		{
			for (unsigned int y = 0; y < (unsigned int)dst->m_height; y++)
			{
				png_read_row(m_png, row, NULL);
				dst->copyRgbRow((unsigned char*)(dst->m_pixels + y * dst->m_pitch), row);
			}
		}
		delete[] row;
		png_read_end(m_png, m_endInfo);
	}

	// 0x4D0740
	void PngReader::readAlpha(const PixelBuffer* dst)
	{
		png_uint_32 width, height;
		int bitDepth, colorType, interlaceType, compressionType, filterType;
		png_get_IHDR(m_png, m_info, &width, &height, &bitDepth, &colorType, &interlaceType, &compressionType, &filterType);
		png_get_channels(m_png, m_info);
		png_get_rowbytes(m_png, m_info);
		if (colorType == PNG_COLOR_TYPE_PALETTE)
		{
			png_set_palette_to_rgb(m_png);
			if (png_get_valid(m_png, m_info, PNG_INFO_tRNS))
				png_set_tRNS_to_alpha(m_png);
			else
				png_set_rgb_to_gray_fixed(m_png, 1, -1, -1);
		}
		else if (colorType == PNG_COLOR_TYPE_RGB)
			png_set_rgb_to_gray_fixed(m_png, 1, -1, -1);
		if (bitDepth == 16)
			png_set_strip_16(m_png);
		else if (bitDepth < 8)
			png_set_gray_1_2_4_to_8(m_png);
		int passes = png_set_interlace_handling(m_png);
		png_read_update_info(m_png, m_info);
		png_get_IHDR(m_png, m_info, &width, &height, &bitDepth, &colorType, &interlaceType, &compressionType, &filterType);
		png_byte channels = png_get_channels(m_png, m_info);
		png_bytep row = new png_byte[png_get_rowbytes(m_png, m_info)];
		for (int pass = 0; pass < passes; pass++)
		{
			for (unsigned int y = 0; y < (unsigned int)dst->m_height; y++)
			{
				png_read_row(m_png, row, NULL);
				dst->copyAlphaRow((unsigned char*)(dst->m_pixels + y * dst->m_pitch), row, channels);
			}
		}
		delete[] row;
		png_read_end(m_png, m_endInfo);
	}

	// 0x4D0900
	int PngReader::getWidth() const
	{
		return png_get_image_width(m_png, m_info);
	}

	// 0x4D0920
	int PngReader::getHeight() const
	{
		return png_get_image_height(m_png, m_info);
	}

	// 0x4D0940
	int PngReader::getAlphaType() const
	{
		// 2 when the colour type has PNG_COLOR_MASK_ALPHA (4), else 0: a shift and a mask, no test
		return (png_get_color_type(m_png, m_info) >> 1) & 2;
	}

	// 0x4D0960
	void PngReader::close()
	{
		if (m_png != NULL)
		{
			png_destroy_read_struct(&m_png, &m_info, &m_endInfo);
			m_info = NULL;
			m_endInfo = NULL;
			m_png = NULL;
		}
		if (m_stream != NULL)
			m_stream = NULL;
	}

	// 0x4D09C0
	PngReader::~PngReader()
	{
		close();
	}

	// 0x4D0A20
	void PngReader::error(png_structp png, png_const_charp message)
	{
		throw Exception(message);
	}

	// 0x4D0AA0
	void PngReader::open(InputStream* stream)
	{
		m_stream = stream;
		png_byte signature[8];
		m_stream->read(signature, 8);
		if (png_sig_cmp(signature, 0, 8))
			throw Exception();
		m_png = png_create_read_struct(PNG_LIBPNG_VER_STRING, this, error, warning);
		if (m_png == NULL)
			throw Exception();
		m_info = png_create_info_struct(m_png);
		if (m_info == NULL)
			throw Exception();
		m_endInfo = png_create_info_struct(m_png);
		if (m_endInfo == NULL)
			throw Exception();
		png_set_read_fn(m_png, m_stream, readData);
		png_set_sig_bytes(m_png, 8);
		png_read_info(m_png, m_info);
	}
}

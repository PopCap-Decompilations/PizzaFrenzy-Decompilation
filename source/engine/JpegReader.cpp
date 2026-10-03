// engine::JpegReader: libjpeg 6b decompression from an engine::InputStream, with a source manager adapted from
// IJG's jdatasrc.c.
#include "JpegReader.h"

#include "InputStream.h"
#include "Surface.h"

#include "jerror.h"

namespace engine
{
	// 0x4D0BF0
	void JpegReader::initSource(j_decompress_ptr cinfo)
	{
		Source* src = (Source*)cinfo->src;
		src->m_startOfFile = TRUE;
	}

	// 0x4D0C00
	boolean JpegReader::read(const PixelBuffer* dst)
	{
		JSAMPARRAY buffer = (*m_decompress.mem->alloc_sarray)((j_common_ptr)&m_decompress, JPOOL_IMAGE,
			m_decompress.output_width * m_decompress.output_components, 1);
		while (m_decompress.output_scanline < m_decompress.output_height)
		{
			Pixel* pixel = dst->m_pixels + m_decompress.output_scanline * dst->m_pitch;
			jpeg_read_scanlines(&m_decompress, buffer, 1);
			const JSAMPLE* src = buffer[0];
			for (Pixel* end = pixel + dst->m_width; pixel < end; pixel++)
			{
				pixel->r = *src++;
				pixel->g = *src++;
				pixel->b = *src++;
				pixel->a = 0xFF;
			}
		}
		return jpeg_finish_decompress(&m_decompress);
	}

	// 0x4D0C90
	int JpegReader::getWidth() const
	{
		return m_decompress.output_width;
	}

	// 0x4D0CA0 (folded)
	int JpegReader::getHeight() const
	{
		return m_decompress.output_height;
	}

	// 0x4D0CD0
	boolean JpegReader::fillInputBuffer(j_decompress_ptr cinfo)
	{
		Source* src = (Source*)cinfo->src;
		size_t nbytes = src->m_stream->read(src->m_buffer, 4096);
		if (nbytes <= 0)
		{
			if (src->m_startOfFile)		// treat an empty input file as a fatal error
				ERREXIT(cinfo, JERR_INPUT_EMPTY);
			WARNMS(cinfo, JWRN_JPEG_EOF);
			// insert a fake EOI marker
			src->m_buffer[0] = (JOCTET)0xFF;
			src->m_buffer[1] = (JOCTET)JPEG_EOI;
			nbytes = 2;
		}
		src->pub.next_input_byte = src->m_buffer;
		src->pub.bytes_in_buffer = nbytes;
		src->m_startOfFile = FALSE;
		return TRUE;
	}

	// 0x4D0D40
	void JpegReader::skipInputData(j_decompress_ptr cinfo, long numBytes)
	{
		Source* src = (Source*)cinfo->src;
		if (numBytes > 0)
		{
			while (numBytes > (long)src->pub.bytes_in_buffer)
			{
				numBytes -= (long)src->pub.bytes_in_buffer;
				fillInputBuffer(cinfo);
			}
			src->pub.next_input_byte += (size_t)numBytes;
			src->pub.bytes_in_buffer -= (size_t)numBytes;
		}
	}

	// 0x4D0470 (folded)
	void JpegReader::termSource(j_decompress_ptr cinfo)
	{
	}

	// 0x4D0D80
	void JpegReader::setInputStream(InputStream* stream)
	{
		m_decompress.src = &m_source.pub;
		m_source.m_stream = stream;
		m_source.m_buffer = (JOCTET*)(*m_decompress.mem->alloc_small)((j_common_ptr)&m_decompress, JPOOL_PERMANENT,
			4096 * sizeof(JOCTET));
		m_source.pub.init_source = initSource;
		m_source.pub.fill_input_buffer = fillInputBuffer;
		m_source.pub.skip_input_data = skipInputData;
		m_source.pub.resync_to_restart = jpeg_resync_to_restart;	// the default method
		m_source.pub.term_source = termSource;
		m_source.pub.bytes_in_buffer = 0;		// forces fill_input_buffer on the first read
		m_source.pub.next_input_byte = NULL;
	}

	// 0x4D0E30
	void JpegReader::open(InputStream* stream)
	{
		m_decompress.err = jpeg_std_error(&m_errorManager);
		jpeg_create_decompress(&m_decompress);
		setInputStream(stream);
		jpeg_read_header(&m_decompress, TRUE);
		m_decompress.out_color_space = JCS_RGB;
		jpeg_start_decompress(&m_decompress);
	}

	// 0x4D0E80
	JpegReader::JpegReader()
	{
	}

	// 0x4D0E90
	JpegReader::~JpegReader()
	{
		if (m_source.m_stream != NULL)
		{
			jpeg_destroy_decompress(&m_decompress);
			m_source.m_stream = NULL;
		}
	}
}

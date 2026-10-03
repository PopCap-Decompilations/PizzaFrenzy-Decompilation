#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MemoryImage.h"

#include "Application.h"
#include "InputStream.h"
#include "JpegReader.h"
#include "PixelFilter.h"
#include "PngReader.h"
#include "Rect.h"
#include "Resampler.h"

namespace engine
{
	// load's _splitpath parts and the name of the image's alpha mask
	static char s_alphaFileName[264];
	static char s_directory[264];
	static char s_baseName[260];
	static char s_extension[12];

	// 0x4BE750 (folded)
	int MemoryImage::getWidth() const
	{
		return m_width;
	}

	// 0x4BE760 (folded)
	int MemoryImage::getHeight() const
	{
		return m_height;
	}

	// 0x4BE770
	Bitmap* MemoryImage::copyScaledUniform(float scale)
	{
		return copyScaled(scale, scale);
	}

	// 0x4BE780
	Bitmap* MemoryImage::copyScaled(float scaleX, float scaleY)
	{
		Bitmap* image = copy();
		image->scale(scaleX, scaleY);
		return image;
	}

	// 0x4BE7A0
	Bitmap* MemoryImage::copyFilteredScaled(float scale, PixelFilter* filter)
	{
		Bitmap* image = copyFiltered(filter);
		image->scale(scale, scale);
		return image;
	}

	// 0x4BE7C0
	PixelBuffer* MemoryImage::lock()
	{
		m_buffer.m_width = m_width;
		m_buffer.m_pitch = m_width;
		m_buffer.m_height = m_height;
		m_buffer.m_pixels = m_pixels;
		return &m_buffer;
	}

	// 0x4BE7E0
	PixelBuffer* MemoryImage::lock(const IntRect& rect)
	{
		m_buffer.m_width = rect.getWidth();
		m_buffer.m_height = rect.getHeight();
		m_buffer.m_pitch = m_width;
		m_buffer.m_pixels = m_pixels + rect.top * m_width + rect.left;
		return &m_buffer;
	}

	// 0x4BE820
	void MemoryImage::unlock()
	{
		m_buffer.m_pixels = 0;
	}

	// 0x4BE830
	void MemoryImage::applyFilter(PixelFilter* filter)
	{
		for (int y = 0; y < m_height; ++y)
		{
			for (int x = 0; x < m_width; ++x)
			{
				Pixel* pixel = &m_pixels[y * m_width + x];
				unsigned char r, g, b, a;
				filter->filter(x, y, pixel->r, pixel->g, pixel->b, pixel->a, &r, &g, &b, &a);
				pixel->r = r;
				pixel->g = g;
				pixel->b = b;
				pixel->a = a;
			}
		}
	}

	// 0x4BE8D0
	MemoryImage::MemoryImage()
		: m_width(0), m_height(0), m_pixels(0)
	{
	}

	// 0x4BE940
	MemoryImage::~MemoryImage()
	{
		if (m_pixels != 0)
		{
			delete[] m_pixels;
			m_pixels = 0;
			m_width = 0;
			m_height = 0;
		}
	}

	// 0x4BE9A0
	void MemoryImage::create(int width, int height)
	{
		m_width = width;
		m_height = height;
		m_pixels = new Pixel[width * height];
	}

	// 0x4BEA40
	Bitmap* MemoryImage::copy()
	{
		MemoryImage* image = new MemoryImage();
		image->create(getWidth(), getHeight());
		Pixel* src = m_pixels;
		Pixel* dst = image->m_pixels;
		Pixel* end = dst + m_width * m_height;
		while (dst < end)
		{
			*dst++ = *src++;
		}
		image->setAlphaType(getAlphaType(), getAlphaThreshold());
		image->setPivot(m_pivot.x, m_pivot.y);
		return image;
	}

	// 0x4BEB50
	Bitmap* MemoryImage::copyFiltered(PixelFilter* filter)
	{
		MemoryImage* image = new MemoryImage();
		image->create(m_width, m_height);
		Pixel* src = m_pixels;
		Pixel* dst = image->m_pixels;
		for (int y = 0; y < m_height; ++y)
		{
			for (int x = 0; x < m_width; ++x)
			{
				filter->filter(x, y, src->r, src->g, src->b, src->a, &dst->r, &dst->g, &dst->b, &dst->a);
				++src;
				++dst;
			}
		}
		image->setAlphaType(getAlphaType(), getAlphaThreshold());
		image->setPivot(m_pivot.x, m_pivot.y);
		return image;
	}

	// 0x4BECB0
	void MemoryImage::scale(float scaleX, float scaleY)
	{
		if (scaleX < 0.0f)
		{
			scaleX = -scaleX;
		}
		if (scaleY < 0.0f)
		{
			scaleY = -scaleY;
		}
		Resampler resampler(6);
		Resampler::Image src;
		src.width = getWidth();
		src.height = getHeight();
		src.pixelStride = 4;
		src.rowStride = src.width * 4;
		Resampler::Image dst;
		dst.width = (int)(src.width * scaleX);
		dst.height = (int)(src.height * scaleY);
		dst.pixelStride = 4;
		dst.rowStride = dst.width * 4;
		Pixel* pixels = new Pixel[dst.width * dst.height];
		src.data = &m_pixels->r;
		dst.data = &pixels->r;
		resampler.zoom(&dst, &src);
		src.data = &m_pixels->g;
		dst.data = &pixels->g;
		resampler.zoom(&dst, &src);
		src.data = &m_pixels->b;
		dst.data = &pixels->b;
		resampler.zoom(&dst, &src);
		src.data = &m_pixels->a;
		dst.data = &pixels->a;
		resampler.zoom(&dst, &src);
		delete[] m_pixels;
		m_width = dst.width;
		m_pixels = pixels;
		m_height = dst.height;
		Point pivot = getPivot();
		setPivot((int)(pivot.x * scaleX), (int)(pivot.y * scaleY));
	}

	// 0x4BEEC0
	void MemoryImage::fill(const Color& color)
	{
		int count = m_width * m_height;
		Pixel pixel(color);
		Pixel* end = m_pixels + count;
		for (Pixel* p = m_pixels; p < end; ++p)
		{
			*p = pixel;
		}
	}

	// 0x4BEF20
	void MemoryImage::load(const char* fileName, bool fromFile)
	{
		_splitpath(fileName, 0, s_directory, s_baseName, s_extension);
		sprintf(s_alphaFileName, "%s_%s.png", s_directory, s_baseName);
		bool hasAlphaFile = false;
		SeekableInputStream* stream;
		if (fromFile)
		{
			stream = getApplication()->getDiskInputStream(fileName);
			if (getApplication()->diskFileExists(s_alphaFileName))
			{
				hasAlphaFile = true;
			}
		}
		else
		{
			stream = getApplication()->getInputStream(fileName);
			if (getApplication()->fileExists(s_alphaFileName))
			{
				hasAlphaFile = true;
			}
		}
		stream->addRef();
		if (strstr(fileName, ".jpg") != 0)
		{
			JpegReader reader;
			reader.open(stream);
			create(reader.getWidth(), reader.getHeight());
			setAlphaType(0, 0);
			reader.read(lock());
		}
		else if (strstr(fileName, ".png") != 0)
		{
			PngReader reader;
			reader.open(stream);
			create(reader.getWidth(), reader.getHeight());
			if (hasAlphaFile)
			{
				reader.readRgb(lock());
			}
			else
			{
				setAlphaType(reader.getAlphaType(), 0);
				reader.read(lock());
			}
		}
		stream->release();
		if (hasAlphaFile)
		{
			if (fromFile)
			{
				stream = getApplication()->getDiskInputStream(s_alphaFileName);
			}
			else
			{
				stream = getApplication()->getInputStream(s_alphaFileName);
			}
			stream->addRef();
			PngReader reader;
			reader.open(stream);
			reader.readAlpha(lock());
			unlock();
			setAlphaType(2, 0);
			stream->release();
		}
	}
}

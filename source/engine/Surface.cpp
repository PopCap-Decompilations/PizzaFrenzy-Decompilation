#include "Surface.h"

#include "Color.h"

namespace engine
{
	// 0x4CF050
	Surface::~Surface()
	{
	}

	// 0x4CF080
	void Surface::setPivot(int x, int y)
	{
		m_pivot.set(x, y);
	}

	// 0x4CF090
	void Surface::setPivotType(int pivotType)
	{
		switch (pivotType)
		{
		case 1:
			setPivot(getWidth() / 2, getHeight() / 2);
			break;
		default:
			setPivot(0, 0);
			break;
		}
	}

	// 0x4CF0D0
	void Surface::setAlphaType(int alphaType, unsigned char alphaThreshold)
	{
		m_alphaType = alphaType;
		m_alphaThreshold = alphaThreshold;
	}

	// 0x4CF0F0
	int Surface::getAlphaType() const
	{
		return m_alphaType;
	}

	// 0x4CF100
	unsigned char Surface::getAlphaThreshold() const
	{
		return m_alphaThreshold;
	}

	// 0x4CF130
	Surface::Surface()
		: m_alphaType(0)
		, m_alphaThreshold(0)
		, m_pivot(0, 0)
	{
	}

	// 0x455B10 (folded)
	void Surface::resize(int width, int height, int mode)
	{
	}

	// 0x467E60 (folded)
	const Point& Surface::getPivot() const
	{
		return m_pivot;
	}

	// 0x492310 (folded)
	void Surface::fill(const Color& color)
	{
	}

	// 0x492310 (folded)
	void Surface::applyFilter(PixelFilter* filter)
	{
	}

	// 0x4CF1F0
	void PixelBuffer::copyRgbRow(unsigned char* dstRow, const unsigned char* rgb) const
	{
		unsigned char* end = dstRow + m_width * 4;
		for (; dstRow < end; dstRow += 4)
		{
			dstRow[2] = *rgb++;
			dstRow[1] = *rgb++;
			dstRow[0] = *rgb++;
		}
	}

	// 0x4CF220
	void PixelBuffer::copyAlphaRow(unsigned char* dstRow, const unsigned char* src, int srcStride) const
	{
		unsigned char* end = dstRow + m_width * 4;
		const unsigned char* alpha = src + srcStride - 1;
		for (; dstRow < end; dstRow += 4)
		{
			dstRow[3] = *alpha;
			alpha += srcStride;
		}
	}

	// 0x4D1360 (folded)
	Pixel::Pixel()
	{
	}

	// 0x4CF260
	Pixel::Pixel(const Color& color)
	{
		r = (unsigned char)(color.r * 255.0f);
		g = (unsigned char)(color.g * 255.0f);
		b = (unsigned char)(color.b * 255.0f);
		a = (unsigned char)(color.a * 255.0f);
	}
}

#include <cmath>
#include <string.h>

#include <mmintrin.h>

#include "Blitter.h"

#include "Matrix3.h"
#include "Point.h"
#include "Rect.h"
#include "Surface.h"
#include "WaveEffect.h"

namespace engine
{
	LARGE_INTEGER Stopwatch::s_frequency;
	__int64 Stopwatch::s_overhead;

	BlitTimer s_drawImageTimer;
	BlitTimer s_blitTimer2;
	BlitTimer s_blitTimer3;
	BlitTimer s_blitTimer4;
	BlitTimer s_scaleFilteredTimer;

	// ---- Profiling -------------------------------------------------------------------------------------------------

	// 0x4C1D00
	Stopwatch::~Stopwatch()
	{
	}

	// 0x4D0470 (folded)
	void Stopwatch::lock() const
	{
	}

	// 0x4D0470 (folded)
	void Stopwatch::unlock() const
	{
	}

	// 0x4C3E60
	void Stopwatch::init()
	{
		if (s_frequency.QuadPart == 0)
		{
			QueryPerformanceFrequency(&s_frequency);
			if (s_frequency.QuadPart == 0)
				s_frequency.QuadPart = 1;
			// time a start() and stop() pair
			m_time = 0;
			s_overhead = 0;
			start();
			stop();
			s_overhead = m_time;
		}
		m_time = 0;
	}

	// 0x4C1D30
	void Stopwatch::start()
	{
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		if (m_time < 0)
			m_time += now.QuadPart;
		else
			m_time = now.QuadPart;
	}

	// 0x4C1D80
	void Stopwatch::stop()
	{
		if (m_time <= 0)
		{
			unlock();
			return;
		}
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		m_time -= now.QuadPart;
		if (m_time < s_overhead)
			m_time -= s_overhead;
		else
			m_time = 0;
	}

	// 0x4C3EF0
	double Stopwatch::getElapsedSeconds() const
	{
		Stopwatch result(*this);
		result.stop();
		return (double)-result.m_time / (double)s_frequency.QuadPart;
	}

	// 0x4C9F10
	BlitTimer::BlitTimer()
		: m_pixelCount(0)
	{
	}

	// 0x4C1E50
	void resetFilteredBlitStats()
	{
		Stopwatch& timer = s_scaleFilteredTimer;
		timer.lock();
		timer.m_time = 0;
		timer.unlock();
		s_scaleFilteredTimer.m_pixelCount = 0;
	}

	// 0x4C3F90
	double getFilteredBlitRate()
	{
		double seconds = s_scaleFilteredTimer.getElapsedSeconds();
		if (seconds > 0.0)
			return (double)s_scaleFilteredTimer.m_pixelCount / seconds;
		return 0.0;
	}

	// ---- Blend operations ------------------------------------------------------------------------------------------

	// a * b / 255, rounded
	static inline int mul255(int a, int b)
	{
		int t = a * b + 128;
		return (t + (t >> 8)) >> 8;
	}

	static inline int clamp255(int value)
	{
		return value > 255 ? 255 : (value > 0 ? value : 0);
	}

	// the low word of value in all four words
	static inline __m64 broadcastWord(int value)
	{
		__m64 words = _mm_cvtsi32_si64(value);
		words = _mm_unpacklo_pi16(words, words);
		return _mm_unpacklo_pi16(words, words);
	}

	// 0x4C1E10
	AlphaTestBlend::AlphaTestBlend(unsigned char threshold)
		: threshold(threshold)
	{
	}

	// 0x4C1E20
	AlphaBlend::AlphaBlend(unsigned char alpha)
		: alpha(alpha)
	{
	}

	// 0x4C1E30 (folded)
	AddColorBlend::AddColorBlend(int red, int green, int blue, unsigned char alpha)
		: red(red), green(green), blue(blue), alpha(alpha)
	{
	}

	// 0x4C1E30 (folded)
	MulColorBlend::MulColorBlend(int red, int green, int blue, unsigned char alpha)
		: red(red), green(green), blue(blue), alpha(alpha)
	{
	}

	inline void CopyBlend::operator()(Pixel* dst, const Pixel* src) const
	{
		*dst = *src;
	}

	inline void AlphaTestBlend::operator()(Pixel* dst, const Pixel* src) const
	{
		if (src->a > threshold)
			*dst = *src;
	}

	inline void AlphaBlend::operator()(Pixel* dst, const Pixel* src) const
	{
		if (src->a == 255 && alpha == 255)
		{
			*dst = *src;
		}
		else if (src->a > 0 && alpha > 0)
		{
			int weight = (src->a * alpha) >> 4;
			__asm
			{
				pxor		mm5, mm5
				mov			eax, src
				movd		mm0, dword ptr [eax]
				punpcklbw	mm0, mm5
				mov			eax, dst
				movd		mm1, dword ptr [eax]
				punpcklbw	mm1, mm5
				movd		mm2, weight
				movq		mm3, mm2
				punpcklwd	mm3, mm2
				movq		mm2, mm3
				punpcklwd	mm3, mm2
				psubsw		mm0, mm1
				psllw		mm0, 4
				pmulhw		mm0, mm3
				paddsw		mm1, mm0
				packuswb	mm1, mm5
				movd		dword ptr [eax], mm1
			}
		}
	}

	inline void AddColorBlend::operator()(Pixel* dst, const Pixel* src) const
	{
		int a = mul255(alpha, src->a);
		dst->r = (unsigned char)clamp255(dst->r + mul255(a, red + src->r - dst->r));
		dst->g = (unsigned char)clamp255(dst->g + mul255(a, green + src->g - dst->g));
		dst->b = (unsigned char)clamp255(dst->b + mul255(a, blue + src->b - dst->b));
		dst->a = (unsigned char)(a + dst->a - mul255(a, dst->a));
	}

	inline void MulColorBlend::operator()(Pixel* dst, const Pixel* src) const
	{
		int a = mul255(alpha, src->a);
		dst->r = (unsigned char)(dst->r + mul255(a, mul255(red, src->r) - dst->r));
		dst->g = (unsigned char)(dst->g + mul255(a, mul255(green, src->g) - dst->g));
		dst->b = (unsigned char)(dst->b + mul255(a, mul255(blue, src->b) - dst->b));
		dst->a = (unsigned char)(a + dst->a - mul255(a, dst->a));
	}

	// ---- Fills, outline, line and circle -----------------------------------------------------------------------------

	// 0x4C2190 (CopyBlend)
	// 0x4C21F0 (AlphaBlend)
	template <class Blend>
	void fillRect(const PixelBuffer& dst, const Pixel& color, const Blend& blend)
	{
		Pixel* pixel = dst.m_pixels;
		int skip = dst.m_pitch - dst.m_width;
		for (int y = 0; y < dst.m_height; ++y)
		{
			for (int x = 0; x < dst.m_width; ++x)
			{
				blend(pixel, &color);
				++pixel;
			}
			pixel += skip;
		}
		__asm emms
	}

	// 0x4C1F30 (AlphaBlend)
	template <class Blend>
	void drawRectOutline(const PixelBuffer& dst, const Pixel& color, const Blend& blend)
	{
		// each loop's end is computed once before it; the column steps re-read m_pitch
		Pixel* end = dst.m_pixels + dst.m_width;
		for (Pixel* pixel = dst.m_pixels; pixel < end; ++pixel)
			blend(pixel, &color);
		end = dst.m_pixels + (dst.m_height - 1) * dst.m_pitch + dst.m_width;
		for (Pixel* pixel = dst.m_pixels + (dst.m_height - 1) * dst.m_pitch; pixel < end; ++pixel)
			blend(pixel, &color);
		end = dst.m_pixels + dst.m_pitch * dst.m_height;
		for (Pixel* pixel = dst.m_pixels; pixel < end; pixel += dst.m_pitch)
			blend(pixel, &color);
		end = dst.m_pixels + dst.m_pitch * dst.m_height + dst.m_width - 1;
		for (Pixel* pixel = dst.m_pixels + dst.m_width - 1; pixel < end; pixel += dst.m_pitch)
			blend(pixel, &color);
		__asm emms
	}

	// 0x4C3FC0 (AlphaBlend)
	template <class Blend>
	void drawLine(const PixelBuffer& dst, const Pixel& color, const Vector2& from, const Vector2& to,
		const Blend& blend)
	{
		// the steps come from the float difference, the end points are truncated separately
		int dx = (int)(to.x - from.x);
		int ax = (dx > 0 ? dx : -dx) << 1;
		int sx = dx > 0 ? 1 : (dx < 0 ? -1 : 0);
		int dy = (int)(to.y - from.y);
		int ay = (dy > 0 ? dy : -dy) << 1;
		int sy = dy > 0 ? 1 : (dy < 0 ? -1 : 0);
		int x = (int)from.x;
		int endX = (int)to.x;
		int y = (int)from.y;
		int endY = (int)to.y;
		if (ax > ay && sx != 0)
		{
			int d = ay - (ax >> 1);
			while (x != endX)
			{
				blend(dst.m_pixels + dst.m_pitch * y + x, &color);
				if (d >= 0)
				{
					y += sy;
					d -= ax;
				}
				x += sx;
				d += ay;
			}
		}
		else if (sy != 0)
		{
			int d = ax - (ay >> 1);
			while (y != endY)
			{
				blend(dst.m_pixels + dst.m_pitch * y + x, &color);
				if (d >= 0)
				{
					x += sx;
					d -= ay;
				}
				y += sy;
				d += ax;
			}
		}
		__asm emms
	}

	// 0x4C22E0 (AlphaBlend)
	template <class Blend>
	void plotCirclePoints(const PixelBuffer& dst, const Pixel& color, int x, int y, const Point& center,
		const Blend& blend)
	{
		// the original's bounds: a point on the bottom row or right column past the buffer is still drawn
		int px = center.x + x;
		int py = center.y + y;
		if (py <= dst.m_height && py >= 0 && px >= 0 && px <= dst.m_width)
			blend(dst.m_pixels + dst.m_pitch * py + px, &color);
		px = center.x + y;
		py = center.y + x;
		if (py <= dst.m_height && py >= 0 && px >= 0 && px <= dst.m_width)
			blend(dst.m_pixels + dst.m_pitch * py + px, &color);
		px = center.x + y;
		py = center.y - x;
		if (py <= dst.m_height && py >= 0 && px >= 0 && px <= dst.m_width)
			blend(dst.m_pixels + dst.m_pitch * py + px, &color);
		px = center.x + x;
		py = center.y - y;
		if (py <= dst.m_height && py >= 0 && px >= 0 && px <= dst.m_width)
			blend(dst.m_pixels + dst.m_pitch * py + px, &color);
		px = center.x - x;
		py = center.y - y;
		if (py <= dst.m_height && py >= 0 && px >= 0 && px <= dst.m_width)
			blend(dst.m_pixels + dst.m_pitch * py + px, &color);
		px = center.x - y;
		py = center.y - x;
		if (py <= dst.m_height && py >= 0 && px >= 0 && px <= dst.m_width)
			blend(dst.m_pixels + dst.m_pitch * py + px, &color);
		px = center.x - y;
		py = center.y + x;
		if (py <= dst.m_height && py >= 0 && px >= 0 && px <= dst.m_width)
			blend(dst.m_pixels + dst.m_pitch * py + px, &color);
		px = center.x - x;
		py = center.y + y;
		if (py <= dst.m_height && py >= 0 && px >= 0 && px <= dst.m_width)
			blend(dst.m_pixels + dst.m_pitch * py + px, &color);
	}

	// 0x4C2890 (AlphaBlend)
	template <class Blend>
	void drawCircle(const PixelBuffer& dst, const Pixel& color, const Vector2& center, float radius,
		const Blend& blend)
	{
		Point point;	// never used (the original constructs it)
		int x = 0;
		int y = (int)radius;
		int d = 1 - y;
		// center converts to a Point temporary for each call
		plotCirclePoints(dst, color, x, y, center, blend);
		while (y > x)
		{
			if (d < 0)
			{
				d += 2 * x + 3;
			}
			else
			{
				d += 2 * (x - y) + 5;
				--y;
			}
			++x;
			plotCirclePoints(dst, color, x, y, center, blend);
		}
	}

	// ---- Blits -------------------------------------------------------------------------------------------------------

	// 0x4C1E90
	template <>
	void blit<CopyBlend>(const PixelBuffer& dst, const PixelBuffer& src, const CopyBlend& blend)
	{
		int width = src.m_width < dst.m_width ? src.m_width : dst.m_width;
		int height = src.m_height < dst.m_height ? src.m_height : dst.m_height;
		Pixel* dstPixel = dst.m_pixels;
		Pixel* srcPixel = src.m_pixels;
		if (src.m_pitch == width && dst.m_pitch == width)
		{
			memcpy(dstPixel, srcPixel, width * height * sizeof(Pixel));
		}
		else
		{
			for (int y = 0; y < height; ++y)
			{
				memcpy(dstPixel, srcPixel, width * sizeof(Pixel));
				srcPixel += src.m_pitch;
				dstPixel += dst.m_pitch;
			}
		}
	}

	// 0x4C2CB0 (AlphaTestBlend)
	// 0x4C30A0 (AlphaBlend)
	// 0x4C3870 (MulColorBlend)
	// 0x4C5E70 (AddColorBlend)
	template <class Blend>
	void blit(const PixelBuffer& dst, const PixelBuffer& src, const Blend& blend)
	{
		int width = src.m_width < dst.m_width ? src.m_width : dst.m_width;
		int height = src.m_height < dst.m_height ? src.m_height : dst.m_height;
		int dstSkip = dst.m_pitch - width;
		int srcSkip = src.m_pitch - width;
		Pixel* dstPixel = dst.m_pixels;
		Pixel* srcPixel = src.m_pixels;
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				blend(dstPixel, srcPixel);
				++srcPixel;
				++dstPixel;
			}
			srcPixel += srcSkip;
			dstPixel += dstSkip;
		}
		__asm emms
	}

	// 0x4C4230 (CopyBlend)
	// 0x4C47C0 (AlphaTestBlend)
	// 0x4C4DB0 (AlphaBlend)
	// 0x4C55D0 (AddColorBlend)
	// 0x4C66F0 (MulColorBlend)
	template <class Blend>
	void blitSubpixel(const PixelBuffer& dst, const PixelBuffer& src, int fracX, int fracY, const Blend& blend)
	{
		// Each source pixel is split four ways: the rest stays at (x, y), weightX goes to (x + 1, y) (carry),
		// weightY to (x, y + 1) and weightXY to (x + 1, y + 1) (both collected in nextRow for the next row).
		Pixel nextRow[1000];
		memset(nextRow, 0, sizeof(nextRow));
		int weightXY = mul255(fracX, fracY);
		int weightY = mul255(255 - fracX, fracY);
		int weightX = mul255(255 - fracY, fracX);
		int width = src.m_width < dst.m_width ? src.m_width : dst.m_width;
		int height = src.m_height < dst.m_height ? src.m_height : dst.m_height;
		__m64 factorXY = broadcastWord(weightXY);
		int dstSkip = dst.m_pitch - width;
		__m64 factorY = broadcastWord(weightY);
		int srcSkip = src.m_pitch - width;
		__m64 factorX = broadcastWord(weightX);
		Pixel* dstPixel = dst.m_pixels;
		Pixel* srcPixel = src.m_pixels;
		Pixel pixel;
		__m64 zero = _mm_setzero_si64();
		for (int y = 0; y < height - 1; ++y)
		{
			int* next = (int*)nextRow;
			int above = next[0];			// what the previous row left for this pixel
			int aboveNext = next[1];
			next[0] = 0;
			__m64 carry = zero;
			for (int x = 0; x < width - 1; ++x)
			{
				__m64 source = _mm_unpacklo_pi8(_mm_cvtsi32_si64(*(int*)srcPixel), zero);
				__m64 toXY = _mm_srli_pi16(_mm_mullo_pi16(source, factorXY), 8);
				__m64 toY = _mm_srli_pi16(_mm_mullo_pi16(source, factorY), 8);
				__m64 toX = _mm_srli_pi16(_mm_mullo_pi16(source, factorX), 8);
				source = _mm_subs_pu16(source, toXY);
				source = _mm_subs_pu16(source, toY);
				source = _mm_subs_pu16(source, toX);
				__m64 sum = _mm_adds_pu16(_mm_unpacklo_pi8(_mm_cvtsi32_si64(above), zero), source);
				sum = _mm_adds_pu16(sum, carry);
				next[0] += _mm_cvtsi64_si32(_mm_packs_pu16(toY, zero));
				next[1] = _mm_cvtsi64_si32(_mm_packs_pu16(toXY, zero));
				carry = toX;
				*(int*)&pixel = _mm_cvtsi64_si32(_mm_packs_pu16(sum, zero));
				above = aboveNext;
				aboveNext = next[2];
				blend(dstPixel, &pixel);
				++srcPixel;
				++dstPixel;
				++next;
			}
			// the last column only receives
			*(int*)&pixel = _mm_cvtsi64_si32(_mm_packs_pu16(
				_mm_adds_pu16(_mm_unpacklo_pi8(_mm_cvtsi32_si64(above), zero), carry), zero));
			blend(dstPixel, &pixel);
			srcPixel += srcSkip + 1;
			dstPixel += dstSkip + 1;
		}
		// the last row is what the row above left
		for (int x = 0; x < width; ++x)
		{
			blend(dstPixel, &nextRow[x]);
			++dstPixel;
		}
		_mm_empty();
	}

	// 0x4C44B0 (CopyBlend)
	// 0x4C4A80 (AlphaTestBlend)
	// 0x4C51B0 (AlphaBlend)
	// 0x4C5B80 (AddColorBlend)
	// 0x4C3590 (MulColorBlend)
	template <class Blend>
	void blitWave(const PixelBuffer& dst, const PixelBuffer& src, const Blend& blend, const WaveEffect* wave, int y)
	{
		int width = src.m_width < dst.m_width ? src.m_width : dst.m_width;
		int height = src.m_height < dst.m_height ? src.m_height : dst.m_height;
		int dstSkip = dst.m_pitch - width;
		Pixel* dstPixel = dst.m_pixels;
		Pixel pixel;
		__m64 weight;
		__m64 carry;
		for (int row = 0; row < height; ++row)
		{
			__asm emms
			float shift = (std::sin(wave->getFrequency() * (row + y) + wave->getPhase()) + 1.0f)
				* wave->getAmplitude();
			if (shift < 0.0f)
				shift = 0.0f;
			int offset = (int)shift;
			Pixel* srcPixel = src.m_pixels + src.m_pitch * row + offset;
			__asm emms
			weight.m64_i32[0] = (int)(255.0f - (shift - offset) * 255.0f);
			Pixel source = *srcPixel;
			// mm0: the weight in all four words, mm5: zero, carry: the first pixel's weighted part
			__asm
			{
				movq		mm1, weight
				movq		mm0, mm1
				punpcklwd	mm0, mm1
				movq		mm1, mm0
				punpcklwd	mm0, mm1
				pxor		mm5, mm5
				movd		mm2, source
				punpcklbw	mm2, mm5
				pmullw		mm2, mm0
				psrlw		mm2, 8
				movq		carry, mm2
			}
			for (int x = 0; x < width; ++x)
			{
				source = *srcPixel;
				__asm
				{
					movd		mm2, source
					punpcklbw	mm2, mm5
					movq		mm4, mm2
					pmullw		mm2, mm0
					psrlw		mm2, 8
					psubusw		mm4, mm2
					paddusw		mm4, carry
					movq		carry, mm2
					packuswb	mm4, mm1
					movd		pixel, mm4
				}
				blend(dstPixel, &pixel);
				// the source stays on the first and last pixels of its row
				if (offset < width - 1 && offset >= 0)
					++srcPixel;
				++offset;
				++dstPixel;
			}
			__asm emms
			dstPixel += dstSkip;
		}
	}

	// 0x4C4680 (CopyBlend)
	// 0x4C4C60 (AlphaTestBlend)
	// 0x4C5400 (AlphaBlend)
	// 0x4C6490 (AddColorBlend)
	// 0x4C6C80 (MulColorBlend)
	template <class Blend>
	void blitScaled(const PixelBuffer& dst, const PixelBuffer& src, int flip, const Blend& blend)
	{
		if (dst.m_width == 0 || dst.m_height == 0)
			return;
		int startX;
		int stepX;
		if (flip & 1)
		{
			startX = src.m_width - 1;
			stepX = -1;
		}
		else
		{
			startX = 0;
			stepX = 1;
		}
		int startY;
		int stepY;
		if (flip & 2)
		{
			startY = src.m_height - 1;
			stepY = -src.m_pitch;
		}
		else
		{
			startY = 0;
			stepY = src.m_pitch;
		}
		Pixel* srcRow = src.m_pixels + src.m_pitch * startY + startX;
		Pixel* dstPixel = dst.m_pixels;
		int dstSkip = dst.m_pitch - dst.m_width;
		// the error terms step by sizes read once (the heights) or once per row (the widths); the loop conditions
		// re-read the destination's size
		int srcHeight = src.m_height;
		int dstHeight = dst.m_height;
		int errorY = 0;
		for (int y = 0; y < dst.m_height; ++y)
		{
			Pixel* srcPixel = srcRow;
			int srcWidth = src.m_width;
			int dstWidth = dst.m_width;
			int errorX = 0;
			for (int x = 0; x < dst.m_width; ++x)
			{
				blend(dstPixel, srcPixel);
				errorX += srcWidth;
				while (errorX >= dstWidth)
				{
					errorX -= dstWidth;
					srcPixel += stepX;
				}
				++dstPixel;
			}
			errorY += srcHeight;
			while (errorY >= dstHeight)
			{
				errorY -= dstHeight;
				srcRow += stepY;
			}
			dstPixel += dstSkip;
		}
		__asm emms
	}

	// 0x4C2930 (CopyBlend)
	// 0x4C2D30 (AlphaTestBlend)
	// 0x4C31B0 (AlphaBlend)
	// 0x4C6010 (AddColorBlend)
	// 0x4C3A00 (MulColorBlend)
	template <class Blend>
	void blitScaledFiltered(const PixelBuffer* dst, const PixelBuffer* src, int fracX, int fracY, int flip,
		const Blend& blend)
	{
		s_scaleFilteredTimer.start();
		if (dst == NULL || src == NULL || dst->m_width < 1 || dst->m_height < 1 || src->m_width <= 1
			|| src->m_height <= 1)
			return;
		int startX;
		int stepX;
		if (flip & 1)
		{
			startX = src->m_width - 1;
			stepX = -1;
		}
		else
		{
			startX = 0;
			stepX = 1;
		}
		int startY;
		int stepY;
		if (flip & 2)
		{
			startY = src->m_height - 1;
			stepY = -src->m_pitch;
		}
		else
		{
			startY = 0;
			stepY = src->m_pitch;
		}
		int dstSkip = dst->m_pitch - dst->m_width;
		Pixel* dstPixel = dst->m_pixels;
		Pixel* srcRow = src->m_pixels + src->m_pitch * startY + startX;
		// 8.8 fixed point source steps per destination pixel
		int stepU = ((src->m_width << 8) - 256) / dst->m_width;
		int stepV = ((src->m_height << 8) - 256) / dst->m_height;
		Pixel pixel;
		int v = 0;
		for (int y = 0; y < dst->m_height; ++y)
		{
			srcRow += v / 256 * stepY;
			v %= 256;
			Pixel* srcPixel = srcRow;
			int u = 0;
			__m64 zero = _mm_setzero_si64();
			for (int x = 0; x < dst->m_width; ++x)
			{
				srcPixel += u / 256 * stepX;
				u %= 256;
				// the four neighbours' weights, each scaled by its alpha
				Pixel* below = srcPixel + stepY;
				int weight11 = (below[stepX].a * u * v) >> 16;
				int weight01 = (below[0].a * (255 - u) * v) >> 16;
				int weight10 = (srcPixel[stepX].a * (255 - v) * u) >> 16;
				int weight00 = ((255 - weight10 - weight01 - weight11) * srcPixel[0].a) >> 8;
				__m64 part11 = _mm_mullo_pi16(_mm_unpacklo_pi8(_mm_cvtsi32_si64(*(int*)&below[stepX]), zero),
					broadcastWord(weight11));
				__m64 part01 = _mm_mullo_pi16(_mm_unpacklo_pi8(_mm_cvtsi32_si64(*(int*)&below[0]), zero),
					broadcastWord(weight01));
				__m64 part10 = _mm_mullo_pi16(_mm_unpacklo_pi8(_mm_cvtsi32_si64(*(int*)&srcPixel[stepX]), zero),
					broadcastWord(weight10));
				__m64 part00 = _mm_mullo_pi16(_mm_unpacklo_pi8(_mm_cvtsi32_si64(*(int*)&srcPixel[0]), zero),
					broadcastWord(weight00));
				__m64 sum = _mm_adds_pu16(_mm_adds_pu16(_mm_adds_pu16(part00, part10), part01), part11);
				*(int*)&pixel = _mm_cvtsi64_si32(_mm_packs_pu16(_mm_srli_pi16(sum, 8), zero));
				blend(dstPixel, &pixel);
				++dstPixel;
				u += stepU;
			}
			v += stepV;
			dstPixel += dstSkip;
		}
		_mm_empty();
		s_scaleFilteredTimer.stop();
		s_scaleFilteredTimer.m_pixelCount += dst->m_width * dst->m_height;
	}

	// 0x4C6EE0 (CopyBlend)
	// 0x4C77E0 (AlphaTestBlend)
	// 0x4C8100 (AlphaBlend)
	// 0x4C8A90 (AddColorBlend)
	// 0x4C94E0 (MulColorBlend)
	template <class Blend>
	void drawPolygon(const PixelBuffer& dst, const PixelBuffer& src, const Vector2* dstPoints, const Point* srcPoints,
		int count, const Blend& blend)
	{
		// from the top vertex, the left edges run backwards through the points and the right edges forwards
		int top = 0;
		for (int i = 1; i < count; ++i)
		{
			if (dstPoints[i].y < dstPoints[top].y)
				top = i;
		}
		int left = top - 1;
		if (left < 0)
			left += count;
		else if (left >= count)
			left -= count;
		EdgeStepper leftX;
		EdgeStepper leftU;
		EdgeStepper leftV;
		int steps = (int)(dstPoints[left].y - dstPoints[top].y + 1.0f);
		leftX.init((int)dstPoints[top].x, (int)dstPoints[left].x, steps);
		leftU.init(srcPoints[top].x, srcPoints[left].x, steps);
		leftV.init(srcPoints[top].y, srcPoints[left].y, steps);
		int right = top + 1;
		if (right < 0)
			right += count;
		else if (right >= count)
			right -= count;
		EdgeStepper rightX;
		EdgeStepper rightU;
		EdgeStepper rightV;
		steps = (int)(dstPoints[right].y - dstPoints[top].y + 1.0f);
		rightX.init((int)dstPoints[top].x, (int)dstPoints[right].x, steps);
		rightU.init(srcPoints[top].x, srcPoints[right].x, steps);
		rightV.init(srcPoints[top].y, srcPoints[right].y, steps);
		int y = (int)dstPoints[top].y;
		for (;;)
		{
			if (leftX.index >= leftX.count)
			{
				int from = left;
				left = from - 1;
				if (left < 0)
					left += count;
				else if (left >= count)
					left -= count;
				steps = (int)(dstPoints[left].y - dstPoints[from].y + 1.0f);
				leftX.init((int)dstPoints[from].x, (int)dstPoints[left].x, steps);
				leftU.init(srcPoints[from].x, srcPoints[left].x, steps);
				leftV.init(srcPoints[from].y, srcPoints[left].y, steps);
				if (from == right)
					return;
			}
			else if (rightX.index >= rightX.count)
			{
				int from = right;
				right = from + 1;
				if (right < 0)
					right += count;
				else if (right >= count)
					right -= count;
				steps = (int)(dstPoints[right].y - dstPoints[from].y + 1.0f);
				rightX.init((int)dstPoints[from].x, (int)dstPoints[right].x, steps);
				rightU.init(srcPoints[from].x, srcPoints[right].x, steps);
				rightV.init(srcPoints[from].y, srcPoints[right].y, steps);
				if (from == left)
					return;
			}
			else
			{
				if (y >= 0 && y < dst.m_height)
				{
					int width = rightX.value - leftX.value + 1;
					EdgeStepper u;
					EdgeStepper v;
					u.init(leftU.value, rightU.value, width);
					v.init(leftV.value, rightV.value, width);
					for (int x = leftX.value; x <= rightX.value; ++x)
					{
						if (x >= 0 && x < dst.m_width)
							blend(dst.m_pixels + dst.m_pitch * y + x, src.m_pixels + src.m_pitch * v.value + u.value);
						u.step();
						v.step();
					}
					__asm emms
				}
				leftX.step();
				leftU.step();
				leftV.step();
				rightX.step();
				rightU.step();
				rightV.step();
				++y;
			}
		}
	}

	// 0x4C9F50 (CopyBlend)
	// 0x4CA030 (AlphaTestBlend)
	// 0x4CA110 (AlphaBlend)
	// 0x4CA1F0 (AddColorBlend)
	// 0x4CA2D0 (MulColorBlend)
	template <class Blend>
	void blitRotated(const PixelBuffer& dst, const PixelBuffer& src, const Matrix3& transform, const Blend& blend)
	{
		Vector2 dstPoints[4];
		Point srcPoints[4];
		srcPoints[0].set(0, 0);
		srcPoints[1].set(src.m_width - 1, 0);
		srcPoints[2].set(src.m_width - 1, src.m_height - 1);
		srcPoints[3].set(0, src.m_height - 1);
		for (int i = 0; i < 4; ++i)
			dstPoints[i] = transform.transform((float)srcPoints[i].x, (float)srcPoints[i].y);
		drawPolygon(dst, src, dstPoints, srcPoints, 4, blend);
	}

	// 0x4CA3B0 (CopyBlend)
	// 0x4CAB60 (AlphaTestBlend)
	// 0x4CB310 (AlphaBlend)
	// 0x4CBAC0 (AddColorBlend)
	// 0x4CC270 (MulColorBlend)
	template <class Blend>
	void drawImage(Surface* dst, const IntRect& clip, Surface* src, const IntRect& srcRect, float x, float y,
		float scaleX, float scaleY, float angle, unsigned char smoothFlags, const Blend& blend, const WaveEffect* wave)
	{
		const Point& pivot = src->getPivot();
		if (fabs(angle) < 2.38418579e-07f)
		{
			// source is srcRect, target where it lands on dst
			Rect source((float)srcRect.left, (float)srcRect.top, (float)srcRect.right, (float)srcRect.bottom);
			Rect target(source);
			target.offset(-source.left, -source.top);
			target.offset((float)-pivot.x, (float)-pivot.y);
			target.scale(scaleX, scaleY);
			target.normalize();
			target.offset(x, y);
			Rect clipArea((float)clip.left, (float)clip.top, (float)clip.right, (float)clip.bottom);
			if (!clipArea.contains(target))
			{
				Rect clipped;
				clipped.intersect(clipArea, target);
				target = clipped;
				if (target.isEmpty())
					return;
				// the clipped target back in source coordinates
				clipped.offset(-x, -y);
				clipped.scale(1.0f / scaleX, 1.0f / scaleY);
				clipped.normalize();
				clipped.offset((float)pivot.x, (float)pivot.y);
				clipped.offset(source.left, source.top);
				source = clipped;
				if (fabs(scaleX - 1.0f) < 2.38418579e-07f && floatEquals(scaleY, 1.0f))
				{
					// unscaled: the source starts at the next whole pixel, the target moves as far
					if ((int)source.left - source.left != 0.0f)
					{
						target.left += (int)source.left + 1.0f - source.left;
						source.left += 1.0f;
					}
					if ((int)source.top - source.top != 0.0f)
					{
						target.top += (int)source.top + 1.0f - source.top;
						source.top += 1.0f;
					}
				}
			}
			IntRect targetRect((int)target.left, (int)target.top, (int)target.right, (int)target.bottom);
			IntRect sourceRect((int)source.left, (int)source.top, (int)source.right, (int)source.bottom);
			if (!targetRect.isEmpty() && !sourceRect.isEmpty())
			{
				int fracX = (int)((target.left - floor(target.left)) * 255.0f);
				int fracY = (int)((target.top - floor(target.top)) * 255.0f);
				PixelBuffer* dstBuffer;
				PixelBuffer* srcBuffer;
				if (fabs(scaleX - 1.0f) < 2.38418579e-07f && floatEquals(scaleY, 1.0f))
				{
					dstBuffer = dst->lock(targetRect);
					srcBuffer = src->lock(sourceRect);
					s_drawImageTimer.start();
					if (fracX != 0 || fracY != 0)
						blitSubpixel(*dstBuffer, *srcBuffer, fracX, fracY, blend);
					else if (wave != NULL && wave->getType() == 1)
						blitWave(*dstBuffer, *srcBuffer, blend, wave, (int)source.top);
					else
						blit(*dstBuffer, *srcBuffer, blend);
				}
				else
				{
					dstBuffer = dst->lock(targetRect);
					srcBuffer = src->lock(sourceRect);
					s_drawImageTimer.start();
					int flip = 0;
					if (scaleX < 0.0f)
						flip |= 1;
					if (scaleY < 0.0f)
						flip |= 2;
					if (smoothFlags & 4)
						blitScaledFiltered(dstBuffer, srcBuffer, fracX, fracY, flip, blend);
					else
						blitScaled(*dstBuffer, *srcBuffer, flip, blend);
				}
				s_drawImageTimer.stop();
				s_drawImageTimer.m_pixelCount += dstBuffer->m_height * dstBuffer->m_width;
				dst->unlock();
				src->unlock();
			}
		}
		else
		{
			Matrix3 transform;
			transform.setIdentity();
			transform.translate((float)-pivot.x, (float)-pivot.y);
			transform.scale(scaleX, scaleY);
			transform.rotate(angle);
			transform.translate(x, y);
			s_drawImageTimer.start();
			blitRotated(*dst->lock(), *src->lock(srcRect), transform, blend);
			s_drawImageTimer.stop();
			s_drawImageTimer.m_pixelCount +=
				(__int64)((float)srcRect.getHeight() * srcRect.getWidth() * scaleX * scaleY);
			dst->unlock();
			src->unlock();
		}
	}

	// ---- The instantiations SoftwareGraphics calls -------------------------------------------------------------------

	template void fillRect<CopyBlend>(const PixelBuffer& dst, const Pixel& color, const CopyBlend& blend);
	template void fillRect<AlphaBlend>(const PixelBuffer& dst, const Pixel& color, const AlphaBlend& blend);
	template void drawRectOutline<AlphaBlend>(const PixelBuffer& dst, const Pixel& color, const AlphaBlend& blend);
	template void drawLine<AlphaBlend>(const PixelBuffer& dst, const Pixel& color, const Vector2& from,
		const Vector2& to, const AlphaBlend& blend);
	template void drawCircle<AlphaBlend>(const PixelBuffer& dst, const Pixel& color, const Vector2& center,
		float radius, const AlphaBlend& blend);
	template void drawImage<CopyBlend>(Surface* dst, const IntRect& clip, Surface* src, const IntRect& srcRect,
		float x, float y, float scaleX, float scaleY, float angle, unsigned char smoothFlags, const CopyBlend& blend,
		const WaveEffect* wave);
	template void drawImage<AlphaTestBlend>(Surface* dst, const IntRect& clip, Surface* src, const IntRect& srcRect,
		float x, float y, float scaleX, float scaleY, float angle, unsigned char smoothFlags,
		const AlphaTestBlend& blend, const WaveEffect* wave);
	template void drawImage<AlphaBlend>(Surface* dst, const IntRect& clip, Surface* src, const IntRect& srcRect,
		float x, float y, float scaleX, float scaleY, float angle, unsigned char smoothFlags, const AlphaBlend& blend,
		const WaveEffect* wave);
	template void drawImage<AddColorBlend>(Surface* dst, const IntRect& clip, Surface* src, const IntRect& srcRect,
		float x, float y, float scaleX, float scaleY, float angle, unsigned char smoothFlags,
		const AddColorBlend& blend, const WaveEffect* wave);
	template void drawImage<MulColorBlend>(Surface* dst, const IntRect& clip, Surface* src, const IntRect& srcRect,
		float x, float y, float scaleX, float scaleY, float angle, unsigned char smoothFlags,
		const MulColorBlend& blend, const WaveEffect* wave);
}

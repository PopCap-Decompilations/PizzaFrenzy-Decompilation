// The software blitters (Blitter.cpp): the five blend operations (copy, alpha test, alpha blend, add colour,
// multiply colour), the fill/outline/line/circle and blit loop templates over them, drawImage (the body of the
// software Graphics::drawImage), engine::Stopwatch and the engine::BlitTimer profilers.
#pragma once

#include <windows.h>

namespace engine
{
	struct IntRect;
	struct Matrix3;
	struct Pixel;
	struct PixelBuffer;
	struct Point;
	class Surface;
	struct Vector2;
	class WaveEffect;

	// ---- Profiling -------------------------------------------------------------------------------------------------

	// An accumulating QueryPerformanceCounter stopwatch (Dean Wyant's CPerfTimer): m_time is the count at start()
	// while running and minus the counts accumulated so far while stopped. Not an engine::Object: a plain polymorphic
	// class (0x10 bytes). Its constructors are inlined everywhere (vtable store, then init() or the copy).
	class Stopwatch
	{
	public:
		Stopwatch()
		{
			init();
		}

		Stopwatch(const Stopwatch& other)
		{
			if (&other != this)
			{
				other.lock();
				lock();
				m_time = other.m_time;
				unlock();
				other.unlock();
			}
		}

		virtual ~Stopwatch();								// slot 0

		virtual void lock() const;							// slot 1: 0x4D0470 (folded): empty (a thread-safe timer's hook)
		virtual void unlock() const;						// slot 2: 0x4D0470 (folded): empty

		void init();										// the first call measures s_frequency and s_overhead; m_time = 0
		void start();										// continues from the accumulated time
		void stop();
		double getElapsedSeconds() const;					// accumulated time, counting a running interval up to now

															// +0x04 (padding: m_time is 8-aligned)
		__int64 m_time;										// +0x08

		static LARGE_INTEGER s_frequency;					// 0x540518 QueryPerformanceFrequency (1 if it fails), set by the first init()
		static __int64 s_overhead;							// 0x540520 minus the counts a start() and stop() cost (m_time after the first init()'s pair)
	};

	// A Stopwatch that also counts the pixels drawn while it ran; implicit destructor. Five file-level instances.
	class BlitTimer : public Stopwatch
	{
	public:
		BlitTimer();

		__int64 m_pixelCount;								// +0x10
	};

	extern BlitTimer s_drawImageTimer;						// 0x540480 times every drawImage<> blit; m_pixelCount: pixels drawn
	extern BlitTimer s_blitTimer2;							// 0x540498 constructed and destroyed only
	extern BlitTimer s_blitTimer3;							// 0x5404B0 constructed and destroyed only
	extern BlitTimer s_blitTimer4;							// 0x5404C8 constructed and destroyed only
	extern BlitTimer s_scaleFilteredTimer;					// 0x5404E0 times blitScaledFiltered<>; reset every frame

	// Every frame (Win32Application::tick): s_scaleFilteredTimer's time and pixel count back to 0.
	void resetFilteredBlitStats();
	// Pixels per second of this frame's filtered stretches (Win32Application::getCountPerSecond); 0 without time.
	double getFilteredBlitRate();

	// ---- Blend operations: how a source pixel (or a colour) is drawn onto a destination pixel ----------------------
	// Built by SoftwareGraphics::drawImage (colour mode 1 -> AddColorBlend, 2 -> MulColorBlend, else alpha type 2 or an
	// opacity other than 1 -> AlphaBlend, alpha type 1 -> AlphaTestBlend, else CopyBlend) and its fills and outlines.
	// operator() draws *src onto *dst; it is inlined into every loop (no out-of-line copy), defined in Blitter.cpp.

	// opaque copy
	struct CopyBlend
	{
		void operator()(Pixel* dst, const Pixel* src) const;
	};

	// copies the source pixels whose alpha > threshold (alphaTest images)
	struct AlphaTestBlend
	{
		AlphaTestBlend(unsigned char threshold);

		void operator()(Pixel* dst, const Pixel* src) const;

		unsigned char threshold;							// +0x00 the image's alpha threshold
	};

	// alpha blend with weight (alpha * source alpha) >> 4 (MMX); a copy when both are 255, nothing when either is 0
	struct AlphaBlend
	{
		AlphaBlend(unsigned char alpha);

		void operator()(Pixel* dst, const Pixel* src) const;	// an __asm block that leaves mm0-mm3 and mm5 changed

		int alpha;											// +0x00 opacity * 255
	};

	// colour mode 1: a = mul255(alpha, source alpha); each channel += mul255(a, colour + source - destination),
	// clamped to 0..255; destination alpha = a + alpha - mul255(a, alpha)
	struct AddColorBlend
	{
		AddColorBlend(int red, int green, int blue, unsigned char alpha);	// 0x4C1E30 (folded with MulColorBlend's)

		void operator()(Pixel* dst, const Pixel* src) const;

		int red;											// +0x00 0..255
		int green;											// +0x04
		int blue;											// +0x08
		unsigned char alpha;								// +0x0C opacity * 255
															// +0x0D (padding)
	};

	// colour mode 2: as AddColorBlend with mul255(colour, source) as the target of each channel
	struct MulColorBlend
	{
		MulColorBlend(int red, int green, int blue, unsigned char alpha);	// 0x4C1E30 (folded with AddColorBlend's)

		void operator()(Pixel* dst, const Pixel* src) const;

		int red;											// +0x00 0..255
		int green;											// +0x04
		int blue;											// +0x08
		unsigned char alpha;								// +0x0C opacity * 255
															// +0x0D (padding)
	};

	// One Bresenham interpolation of drawPolygon: value goes from `from` to `to` in count steps (an edge's x, u or v
	// between two vertices, or u or v along a span). Only a local of drawPolygon, always inlined (0x18 bytes).
	struct EdgeStepper
	{
		void init(int from, int to, int steps)
		{
			index = 0;
			value = from;
			if (from > to)
			{
				delta = from - to;
				direction = -1;
			}
			else
			{
				delta = to - from;
				direction = 1;
			}
			count = steps;
			error = steps;
		}

		void step()
		{
			error += delta;
			while (error > count)
			{
				value += direction;
				error -= count;
			}
			++index;
		}

		int index;											// +0x00 steps taken
		int value;											// +0x04
		int delta;											// +0x08 |to - from|
		int count;											// +0x0C steps to the end
		int error;											// +0x10 starts at count
		int direction;										// +0x14 1 or -1
	};

	// |a - b| < 2.38418579e-07f: defined in SoftwareGraphics.cpp (0x4BF8C0), inlined in drawImage and fillRect.
	bool floatEquals(float a, float b);

	// ---- Fills, outline, line and circle (SoftwareGraphics), each pixel drawn with blend -----------------------------

	// every pixel of dst: 0x4C2190 CopyBlend, 0x4C21F0 AlphaBlend
	template <class Blend>
	void fillRect(const PixelBuffer& dst, const Pixel& color, const Blend& blend);

	// top and bottom rows, left and right columns of dst: 0x4C1F30 AlphaBlend
	template <class Blend>
	void drawRectOutline(const PixelBuffer& dst, const Pixel& color, const Blend& blend);

	// Bresenham between the truncated end points: 0x4C3FC0 AlphaBlend
	template <class Blend>
	void drawLine(const PixelBuffer& dst, const Pixel& color, const Vector2& from, const Vector2& to,
		const Blend& blend);

	// midpoint circle of radius (int)radius: 0x4C2890 AlphaBlend
	template <class Blend>
	void drawCircle(const PixelBuffer& dst, const Pixel& color, const Vector2& center, float radius,
		const Blend& blend);

	// the eight symmetric points (x, y) around center, each bounds-checked: 0x4C22E0 AlphaBlend
	template <class Blend>
	void plotCirclePoints(const PixelBuffer& dst, const Pixel& color, int x, int y, const Point& center,
		const Blend& blend);

	// ---- Blits of src onto dst (both locked by drawImage) ------------------------------------------------------------
	// flip: bit 0 mirrors x, bit 1 mirrors y. fracX, fracY: the sub-pixel offset in 1/255ths. Every loop ends with emms.

	// 1:1, the smaller of both sizes: 0x4C2CB0 AlphaTestBlend, 0x4C30A0 AlphaBlend, 0x4C3870 MulColorBlend,
	// 0x4C5E70 AddColorBlend
	template <class Blend>
	void blit(const PixelBuffer& dst, const PixelBuffer& src, const Blend& blend);

	// 0x4C1E90: one memcpy when both pitches equal the width, else one per row
	template <>
	void blit<CopyBlend>(const PixelBuffer& dst, const PixelBuffer& src, const CopyBlend& blend);

	// 1:1 at a fractional position, 2x2 bilinear mix (MMX, 1000-pixel row buffer): 0x4C4230 CopyBlend,
	// 0x4C47C0 AlphaTestBlend, 0x4C4DB0 AlphaBlend, 0x4C55D0 AddColorBlend, 0x4C66F0 MulColorBlend
	template <class Blend>
	void blitSubpixel(const PixelBuffer& dst, const PixelBuffer& src, int fracX, int fracY, const Blend& blend);

	// 1:1 with row r shifted right by the wave (type 1) at row r + y, neighbours interpolated: 0x4C44B0 CopyBlend,
	// 0x4C4A80 AlphaTestBlend, 0x4C51B0 AlphaBlend, 0x4C5B80 AddColorBlend, 0x4C3590 MulColorBlend
	template <class Blend>
	void blitWave(const PixelBuffer& dst, const PixelBuffer& src, const Blend& blend, const WaveEffect* wave, int y);

	// nearest-neighbour stretch of all of src onto all of dst: 0x4C4680 CopyBlend, 0x4C4C60 AlphaTestBlend,
	// 0x4C5400 AlphaBlend, 0x4C6490 AddColorBlend, 0x4C6C80 MulColorBlend
	template <class Blend>
	void blitScaled(const PixelBuffer& dst, const PixelBuffer& src, int flip, const Blend& blend);

	// bilinear (8.8 fixed point, MMX) stretch, timed by s_scaleFilteredTimer: 0x4C2930 CopyBlend,
	// 0x4C2D30 AlphaTestBlend, 0x4C31B0 AlphaBlend, 0x4C6010 AddColorBlend, 0x4C3A00 MulColorBlend.
	// Pointers: it returns (after starting the timer) when either buffer is NULL. fracX and fracY are unused.
	template <class Blend>
	void blitScaledFiltered(const PixelBuffer* dst, const PixelBuffer* src, int fracX, int fracY, int flip,
		const Blend& blend);

	// texture-mapped convex polygon (dstPoints on dst, srcPoints on src): 0x4C6EE0 CopyBlend,
	// 0x4C77E0 AlphaTestBlend, 0x4C8100 AlphaBlend, 0x4C8A90 AddColorBlend, 0x4C94E0 MulColorBlend
	template <class Blend>
	void drawPolygon(const PixelBuffer& dst, const PixelBuffer& src, const Vector2* dstPoints, const Point* srcPoints,
		int count, const Blend& blend);

	// src's corners through transform, then drawPolygon: 0x4C9F50 CopyBlend, 0x4CA030 AlphaTestBlend,
	// 0x4CA110 AlphaBlend, 0x4CA1F0 AddColorBlend, 0x4CA2D0 MulColorBlend
	template <class Blend>
	void blitRotated(const PixelBuffer& dst, const PixelBuffer& src, const Matrix3& transform, const Blend& blend);

	// The software drawImage: srcRect of src (moved by its pivot, scaled, rotated by angle radians) at (x, y) on dst,
	// clipped to clip; smoothFlags bit 2 selects the filtered stretch. Picks the blit above and times it with
	// s_drawImageTimer: 0x4CA3B0 CopyBlend, 0x4CAB60 AlphaTestBlend, 0x4CB310 AlphaBlend, 0x4CBAC0 AddColorBlend,
	// 0x4CC270 MulColorBlend
	template <class Blend>
	void drawImage(Surface* dst, const IntRect& clip, Surface* src, const IntRect& srcRect, float x, float y,
		float scaleX, float scaleY, float angle, unsigned char smoothFlags, const Blend& blend, const WaveEffect* wave);
}

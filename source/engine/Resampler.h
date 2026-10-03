// engine::Resampler, Dale Schumacher's filtered rescaling of one 8-bit channel ("General Filtered Image Rescaling",
// Graphics Gems III) with the global contribution list turned into a member, and its seven filter kernels.
#pragma once

#include "RefPtr.h"
#include "ResampleFilter.h"

namespace engine
{
	// The kernels: out-of-line constructors (ResampleFilter's inlined), implicit destructors, no members (0x14 bytes).

	// Hermite 2|t|^3 - 3|t|^2 + 1 (filter type 0)
	class HermiteFilter : public ResampleFilter
	{
	public:
		HermiteFilter();

		// ResampleFilter
		virtual double getSupport();			// 0x4CF4B0 (folded): 1
		virtual double apply(double t);
	};

	// box: 1 for -0.5 < t <= 0.5 (type 1)
	class BoxFilter : public ResampleFilter
	{
	public:
		BoxFilter();

		// ResampleFilter
		virtual double getSupport();			// 0.5
		virtual double apply(double t);
	};

	// triangle 1 - |t| (type 2)
	class TriangleFilter : public ResampleFilter
	{
	public:
		TriangleFilter();

		// ResampleFilter
		virtual double getSupport();			// 0x4CF4B0 (folded): 1
		virtual double apply(double t);
	};

	// bell, the quadratic B-spline (type 3)
	class BellFilter : public ResampleFilter
	{
	public:
		BellFilter();

		// ResampleFilter
		virtual double getSupport();			// 1.5
		virtual double apply(double t);
	};

	// cubic B-spline (type 4)
	class BSplineFilter : public ResampleFilter
	{
	public:
		BSplineFilter();

		// ResampleFilter
		virtual double getSupport();			// 0x4CF850 (folded): 2
		virtual double apply(double t);
	};

	// Lanczos3 sinc(t) * sinc(t / 3) (type 5)
	class Lanczos3Filter : public ResampleFilter
	{
	public:
		Lanczos3Filter();

		// ResampleFilter
		virtual double getSupport();			// 3
		virtual double apply(double t);
	};

	// Mitchell-Netravali cubic with B = C = 1/3 (type 6, the one MemoryImage::scale uses)
	class MitchellFilter : public ResampleFilter
	{
	public:
		MitchellFilter();

		// ResampleFilter
		virtual double getSupport();			// 0x4CF850 (folded): 2
		virtual double apply(double t);
	};

	// Not an Object (no vtable): a local of MemoryImage::scale, which zooms each byte plane of its pixels. Implicit
	// destructor (releases m_filter).
	class Resampler
	{
	public:
		// One 8-bit channel of an image (Schumacher's Image with a pixel stride): pixel (x, y) is
		// data[y * rowStride + x * pixelStride].
		struct Image
		{
			int width;							// +0x00
			int height;							// +0x04
			int pixelStride;					// +0x08 bytes from one pixel to the next (4 for a channel of BGRA pixels)
			int rowStride;						// +0x0C bytes from one row to the next
			unsigned char* data;				// +0x10
		};

		// A source pixel's weight in a destination pixel (Schumacher's CONTRIB); calloc'ed arrays, 16 bytes each.
		struct Contribution
		{
			int pixel;							// +0x00
												// +0x04 (padding: weight is 8-aligned)
			double weight;						// +0x08
		};

		// The contributions to one destination pixel (Schumacher's CLIST).
		struct ContributionList
		{
			int n;								// +0x00
			Contribution* p;					// +0x04
		};

		explicit Resampler(int filterType);

		void zoom(Image* dst, const Image* src);

		ContributionList* m_contributions;		// +0x00 one list per destination column, then per row, while zoom() runs; not initialised by the constructor
		RefPtr<ResampleFilter> m_filter;		// +0x04 new filter of the constructor's type (0 Hermite .. 6 Mitchell); NULL for other types
	};
}

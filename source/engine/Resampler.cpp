// engine::Resampler and its filter kernels: Dale Schumacher's "General Filtered Image Rescaling" (Graphics Gems
// III) on one 8-bit channel of an image, with the global contribution list turned into a member.
#include <math.h>
#include <stdlib.h>

#include "Resampler.h"

namespace engine
{
	// Schumacher's put_pixel cache and constants: variables in the original's initialised data, not literals.
	static int s_putPixelY = -1;							// 0x5313E0 row of the cached pointer
	static double s_mitchellB = 1.0 / 3.0;					// 0x5313E8 Mitchell filter B
	static double s_mitchellC = 1.0 / 3.0;					// 0x5313F0 Mitchell filter C
	static double s_pi = 3.14159265358979323846;			// 0x5313F8 used by sinc
	static unsigned char* s_putPixelRow = NULL;				// 0x540510 start of the cached row
	static Resampler::Image* s_putPixelImage = NULL;		// 0x540514 image of the cached row

	// Schumacher's new_image, free_image, get_row, get_column and put_pixel (all inlined in zoom), with a pixel stride.

	static Resampler::Image* newImage(int width, int height)
	{
		Resampler::Image* image;
		if ((image = (Resampler::Image*)malloc(sizeof(Resampler::Image))) != NULL
			&& (image->data = (unsigned char*)calloc(height, width)) != NULL)
		{
			image->width = width;
			image->height = height;
			image->pixelStride = 1;
			image->rowStride = width;
		}
		return image;
	}

	static void freeImage(Resampler::Image* image)
	{
		free(image->data);
		free(image);
	}

	static void getRow(unsigned char* row, const Resampler::Image* image, int y)
	{
		if (y < 0 || y >= image->height)
			return;
		int d = image->pixelStride;
		const unsigned char* p = image->data + y * image->rowStride;
		for (int i = image->width; i-- > 0; p += d)
			*row++ = *p;
	}

	static void getColumn(unsigned char* column, const Resampler::Image* image, int x)
	{
		if (x < 0 || x >= image->width)
			return;
		int d = image->rowStride;
		const unsigned char* p = image->data + x * image->pixelStride;
		for (int i = image->height; i-- > 0; p += d)
			*column++ = *p;
	}

	static unsigned char putPixel(Resampler::Image* image, int x, int y, unsigned char data)
	{
		if (x < 0 || x >= image->width || y < 0 || y >= image->height)
			return 0;
		if (s_putPixelImage != image || s_putPixelY != y)
		{
			s_putPixelImage = image;
			s_putPixelY = y;
			s_putPixelRow = image->data + y * image->rowStride;
		}
		return s_putPixelRow[x * image->pixelStride] = data;
	}

	// inlined in Lanczos3Filter::apply
	static double sinc(double x)
	{
		x *= s_pi;
		if (x != 0)
			return sin(x) / x;
		return 1.0;
	}

	// 0x4CF2C0
	HermiteFilter::HermiteFilter()
	{
	}

	// 0x4CF4B0 (folded)
	double HermiteFilter::getSupport()
	{
		return 1.0;
	}

	// 0x4CF330
	double HermiteFilter::apply(double t)
	{
		// f(t) = 2|t|^3 - 3|t|^2 + 1, -1 <= t <= 1
		if (t < 0.0)
			t = -t;
		if (t < 1.0)
			return (2.0 * t - 3.0) * t * t + 1.0;
		return 0.0;
	}

	// 0x4CF380
	BoxFilter::BoxFilter()
	{
	}

	// 0x4CF3F0
	double BoxFilter::getSupport()
	{
		return 0.5;
	}

	// 0x4CF400
	double BoxFilter::apply(double t)
	{
		if (t > -0.5 && t <= 0.5)
			return 1.0;
		return 0.0;
	}

	// 0x4CF440
	TriangleFilter::TriangleFilter()
	{
	}

	// 0x4CF4B0 (folded)
	double TriangleFilter::getSupport()
	{
		return 1.0;
	}

	// 0x4CF4C0
	double TriangleFilter::apply(double t)
	{
		if (t < 0.0)
			t = -t;
		if (t < 1.0)
			return 1.0 - t;
		return 0.0;
	}

	// 0x4CF500
	BellFilter::BellFilter()
	{
	}

	// 0x4CF570
	double BellFilter::getSupport()
	{
		return 1.5;
	}

	// 0x4CF580
	double BellFilter::apply(double t)
	{
		// box (*) box (*) box
		if (t < 0)
			t = -t;
		if (t < 0.5)
			return 0.75 - t * t;
		if (t < 1.5)
		{
			t = t - 1.5;
			return 0.5 * (t * t);
		}
		return 0.0;
	}

	// 0x4CF5E0
	BSplineFilter::BSplineFilter()
	{
	}

	// 0x4CF850 (folded)
	double BSplineFilter::getSupport()
	{
		return 2.0;
	}

	// 0x4CF650
	double BSplineFilter::apply(double t)
	{
		// box (*) box (*) box (*) box
		double tt;
		if (t < 0)
			t = -t;
		if (t < 1)
		{
			tt = t * t;
			return 0.5 * tt * t - tt + 2.0 / 3.0;
		}
		else if (t < 2)
		{
			t = 2 - t;
			return 1.0 / 6.0 * (t * t * t);
		}
		return 0.0;
	}

	// 0x4CF6C0
	Lanczos3Filter::Lanczos3Filter()
	{
	}

	// 0x4CF730
	double Lanczos3Filter::getSupport()
	{
		return 3.0;
	}

	// 0x4CF740
	double Lanczos3Filter::apply(double t)
	{
		if (t < 0)
			t = -t;
		if (t < 3.0)
			return sinc(t) * sinc(t * (1.0 / 3.0));	// the original multiplies by 1/3 (0x50BC28)
		return 0.0;
	}

	// 0x4CF7E0
	MitchellFilter::MitchellFilter()
	{
	}

	// 0x4CF850 (folded)
	double MitchellFilter::getSupport()
	{
		return 2.0;
	}

	// 0x4CF860
	double MitchellFilter::apply(double t)
	{
		double tt = t * t;
		if (t < 0)
			t = -t;
		// the original's order: the constant term before the tt term, 18 subtracted last, times 1/6 (0x50BC08)
		if (t < 1.0)
		{
			t = ((12.0 - 9.0 * s_mitchellB - 6.0 * s_mitchellC) * (t * tt))
				+ (6.0 - 2 * s_mitchellB)
				+ ((12.0 * s_mitchellB + 6.0 * s_mitchellC - 18.0) * tt);
			return t * (1.0 / 6.0);
		}
		else if (t < 2.0)
		{
			t = ((-1.0 * s_mitchellB - 6.0 * s_mitchellC) * (t * tt))
				+ ((6.0 * s_mitchellB + 30.0 * s_mitchellC) * tt)
				+ ((-12.0 * s_mitchellB - 48.0 * s_mitchellC) * t)
				+ (8.0 * s_mitchellB + 24 * s_mitchellC);
			return t * (1.0 / 6.0);
		}
		return 0.0;
	}

	// 0x4CF990
	void Resampler::zoom(Image* dst, const Image* src)
	{
		double fwidth = m_filter->getSupport();		// filter width (support)
		int i, j, k;								// loop variables
		int n;										// pixel number
		double center;								// filter calculation variables
		int left, right;
		double width, fscale, weight;
		unsigned char* raster;						// a row or column of pixels

		// create intermediate image to hold horizontal zoom
		Image* tmp = newImage(dst->width, src->height);
		double xscale = (double)dst->width / (double)src->width;
		double yscale = (double)dst->height / (double)src->height;

		// pre-calculate filter contributions for a row
		m_contributions = (ContributionList*)calloc(dst->width, sizeof(ContributionList));
		// The original divides by multiplying with reciprocals: width = fwidth * (1 / scale), center = i * (1 / scale)
		// in both branches, and weight * (1 / fscale) before and after apply; see the instructions at 0x4CFA53,
		// at 0x4CFA97, 0x4CFB10, 0x4CFB1C and 0x4CFBE5 (the column pass alike).
		if (xscale < 1.0)
		{
			width = fwidth * (1.0 / xscale);
			fscale = 1.0 / xscale;
			for (i = 0; i < dst->width; ++i)
			{
				m_contributions[i].n = 0;
				m_contributions[i].p = (Contribution*)calloc((int)(width * 2 + 1), sizeof(Contribution));
				center = (double)i * (1.0 / xscale);
				left = (int)ceil(center - width);
				right = (int)floor(center + width);
				for (j = left; j <= right; ++j)
				{
					weight = center - (double)j;
					weight = m_filter->apply(weight * (1.0 / fscale)) * (1.0 / fscale);
					if (j < 0)
						n = -j;
					else if (j >= src->width)
						n = (src->width - j) + src->width - 1;
					else
						n = j;
					k = m_contributions[i].n++;
					m_contributions[i].p[k].pixel = n;
					m_contributions[i].p[k].weight = weight;
				}
			}
		}
		else
		{
			for (i = 0; i < dst->width; ++i)
			{
				m_contributions[i].n = 0;
				m_contributions[i].p = (Contribution*)calloc((int)(fwidth * 2 + 1), sizeof(Contribution));
				center = (double)i * (1.0 / xscale);
				left = (int)ceil(center - fwidth);
				right = (int)floor(center + fwidth);
				for (j = left; j <= right; ++j)
				{
					weight = center - (double)j;
					weight = m_filter->apply(weight);
					if (j < 0)
						n = -j;
					else if (j >= src->width)
						n = (src->width - j) + src->width - 1;
					else
						n = j;
					k = m_contributions[i].n++;
					m_contributions[i].p[k].pixel = n;
					m_contributions[i].p[k].weight = weight;
				}
			}
		}

		// apply filter to zoom horizontally from src to tmp
		raster = (unsigned char*)calloc(src->width, sizeof(unsigned char));
		for (k = 0; k < tmp->height; ++k)
		{
			getRow(raster, src, k);
			for (i = 0; i < tmp->width; ++i)
			{
				weight = 0.0;
				for (j = 0; j < m_contributions[i].n; ++j)
					weight += raster[m_contributions[i].p[j].pixel] * m_contributions[i].p[j].weight;
				putPixel(tmp, i, k, (unsigned char)(weight < 0 ? 0 : weight > 255 ? 255 : weight));
			}
		}
		free(raster);

		// free the memory allocated for horizontal filter weights
		for (i = 0; i < tmp->width; ++i)
			free(m_contributions[i].p);
		free(m_contributions);

		// pre-calculate filter contributions for a column
		m_contributions = (ContributionList*)calloc(dst->height, sizeof(ContributionList));
		if (yscale < 1.0)
		{
			width = fwidth * (1.0 / yscale);
			fscale = 1.0 / yscale;
			for (i = 0; i < dst->height; ++i)
			{
				m_contributions[i].n = 0;
				m_contributions[i].p = (Contribution*)calloc((int)(width * 2 + 1), sizeof(Contribution));
				center = (double)i * (1.0 / yscale);
				left = (int)ceil(center - width);
				right = (int)floor(center + width);
				for (j = left; j <= right; ++j)
				{
					weight = center - (double)j;
					weight = m_filter->apply(weight * (1.0 / fscale)) * (1.0 / fscale);
					if (j < 0)
						n = -j;
					else if (j >= tmp->height)
						n = (tmp->height - j) + tmp->height - 1;
					else
						n = j;
					k = m_contributions[i].n++;
					m_contributions[i].p[k].pixel = n;
					m_contributions[i].p[k].weight = weight;
				}
			}
		}
		else
		{
			for (i = 0; i < dst->height; ++i)
			{
				m_contributions[i].n = 0;
				m_contributions[i].p = (Contribution*)calloc((int)(fwidth * 2 + 1), sizeof(Contribution));
				center = (double)i * (1.0 / yscale);
				left = (int)ceil(center - fwidth);
				right = (int)floor(center + fwidth);
				for (j = left; j <= right; ++j)
				{
					weight = center - (double)j;
					weight = m_filter->apply(weight);
					if (j < 0)
						n = -j;
					else if (j >= tmp->height)
						n = (tmp->height - j) + tmp->height - 1;
					else
						n = j;
					k = m_contributions[i].n++;
					m_contributions[i].p[k].pixel = n;
					m_contributions[i].p[k].weight = weight;
				}
			}
		}

		// apply filter to zoom vertically from tmp to dst
		raster = (unsigned char*)calloc(tmp->height, sizeof(unsigned char));
		for (k = 0; k < dst->width; ++k)
		{
			getColumn(raster, tmp, k);
			for (i = 0; i < dst->height; ++i)
			{
				weight = 0.0;
				for (j = 0; j < m_contributions[i].n; ++j)
					weight += raster[m_contributions[i].p[j].pixel] * m_contributions[i].p[j].weight;
				putPixel(dst, k, i, (unsigned char)(weight < 0 ? 0 : weight > 255 ? 255 : weight));
			}
		}
		free(raster);

		// free the memory allocated for vertical filter weights
		for (i = 0; i < dst->height; ++i)
			free(m_contributions[i].p);
		free(m_contributions);

		freeImage(tmp);
	}

	// 0x4D02E0
	Resampler::Resampler(int filterType)
	{
		switch (filterType)
		{
		case 0:
			m_filter = new HermiteFilter();
			break;
		case 1:
			m_filter = new BoxFilter();
			break;
		case 2:
			m_filter = new TriangleFilter();
			break;
		case 3:
			m_filter = new BellFilter();
			break;
		case 4:
			m_filter = new BSplineFilter();
			break;
		case 5:
			m_filter = new Lanczos3Filter();
			break;
		case 6:
			m_filter = new MitchellFilter();
			break;
		}
	}
}

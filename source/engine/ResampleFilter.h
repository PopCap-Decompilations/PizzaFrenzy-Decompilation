// engine::ResampleFilter: the kernel of engine::Resampler (Dale Schumacher's "General Filtered Image Rescaling",
// Graphics Gems III): its support radius and its value at a distance.
#pragma once

#include "Object.h"

namespace engine
{
	// Abstract; implicit constructor (inlined in each filter's constructor) and destructor. Its vtable (the folded
	// destructor 0x44A0A0 and two pure slots) is byte-identical to InputStream's, so /OPT:ICF merged them (0x5053EC).
	// Layout: Object +0x00, then the vtordisp and Interface (0x14 bytes).
	class ResampleFilter : public Object
	{
	public:
		virtual double getSupport() = 0;		// slot 1: the kernel's radius
		virtual double apply(double t) = 0;		// slot 2: the kernel's value at t
	};
}

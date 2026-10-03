// engine::OutputStream: an abstract byte sink.
#pragma once

#include "Interface.h"

namespace engine
{
	// Two pure slots, constructor always inlined. Implemented by XmlWriter (at +0x0C, so raw text can be written
	// inside an element) and by FileOutputStream (at +0x0C).
	class OutputStream : public virtual Interface
	{
	public:
		virtual void close() = 0;								// slot 0
		virtual void write(const char* data, int length) = 0;	// slot 1
	};
}

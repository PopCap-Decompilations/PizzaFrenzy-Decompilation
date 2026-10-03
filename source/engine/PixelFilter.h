// engine::PixelFilter: a per-pixel colour filter (one abstract slot).
#pragma once

#include "Interface.h"

namespace engine
{
	// The per-pixel callback of the images' filtering (Bitmap); implemented by HsvFilter. Its vtable (one _purecall)
	// was merged into 0x503A70 with Animator's, ActionListener's and Runnable's. x and y are a guess (HsvFilter does
	// not use them).
	class PixelFilter : public virtual Interface
	{
	public:
		virtual void filter(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a,
			unsigned char* outR, unsigned char* outG, unsigned char* outB, unsigned char* outA) = 0;	// slot 0
	};
}

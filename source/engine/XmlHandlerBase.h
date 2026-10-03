// engine::XmlHandlerBase: the memberless XML handler that the stack-aware handlers and the manifest parsers derive from.
#pragma once

#include "XmlDefaultHandler.h"

namespace engine
{
	// XmlDefaultHandler +0x00 with nothing added (no new virtuals, no overrides: its XmlHandler at +0x0C keeps the
	// default vtable 0x502E88), then the vtordisp (+0x14) and the Interface subobject (+0x18): 0x1C bytes. Base of
	// XmlHandlerStack, XmlElementHandler, ParticleSystemLoader, ScreenLayoutParser, SoundManifestHandler and
	// SplatManifestHandler. Its constructor is always inlined (implicit); the destructor is user-declared (it
	// stores the vtables) and inline: the derived destructors inline it.
	class XmlHandlerBase : public XmlDefaultHandler
	{
	public:
		// 0x46BD50 (copy emitted in another object: the destructor is inline)
		virtual ~XmlHandlerBase()
		{
		}
	};
}

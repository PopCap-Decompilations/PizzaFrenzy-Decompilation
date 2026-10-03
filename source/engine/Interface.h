// engine::Interface: the root of every interface of the engine. Reference counting only (COM-like); every
// class inherits it virtually through engine::Object, so an object has exactly one Interface subobject.
#pragma once

namespace engine
{
	class Interface
	{
	public:
		virtual void addRef() = 0;			// slot 0
		virtual void release() = 0;			// slot 1
		virtual long getRefCount() = 0;		// slot 2
	};
}

// engine::ButtonListener: receives a Button's mouse events with the command string it was added with.
#pragma once

#include <string>

namespace engine
{
	// Called by Button::fireMouseEnter..fireClick (Button slots 84-88) with each listener's command. Implemented by
	// ScreenLayout (+0x144), RadioGroup (+0x128) and several game screens (the empty default that ignores the
	// argument is the folded 0x496410). A plain interface of 4 bytes, its vfptr only: no engine::Interface base (the
	// implementers' constructors store just its vtable at the subobject, and their next base or member follows 4
	// bytes later). Its abstract vtable (five _purecall slots) was merged into XmlHandler's (0x502EA0); constructor
	// always inlined, no destructor.
	class ButtonListener
	{
	public:
		virtual void onMouseEnter(std::string command) = 0;	// slot 0
		virtual void onMouseLeave(std::string command) = 0;	// slot 1
		virtual void onMouseDown(std::string command) = 0;		// slot 2
		virtual void onMouseUp(std::string command) = 0;		// slot 3
		virtual void onClick(std::string command) = 0;			// slot 4: released inside a pressed button
	};
}

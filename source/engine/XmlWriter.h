// engine::XmlWriter: writes indented XML ("<name attr=... >", "/>", "</name>") to a wrapped OutputStream.
#pragma once

#include <deque>
#include <string>

#include "Object.h"
#include "OutputStream.h"
#include "Point.h"
#include "Range.h"
#include "RefPtr.h"

namespace engine
{
	// Itself an OutputStream (at +0x0C), so raw text can be written inside an element; made by the pizza editor to
	// save pizzas. Every tag and attribute is formatted with sprintf into the XmlWriter.cpp buffer s_xmlBuffer
	// (0x532F88). Object +0x00, OutputStream +0x0C, members from +0x14, then the vtordisp (+0x34) and the Interface
	// subobject (+0x38): 0x3C bytes. MSVC groups overloaded virtuals and reverses their order, so the
	// writeAttribute overloads are declared in the reverse of their slot order.
	class XmlWriter : public Object, public OutputStream
	{
	public:
		XmlWriter(const char* fileName);
		virtual ~XmlWriter();

		virtual void startElement(const std::string& name);								// slot 1
		virtual void endElement();															// slot 2
		virtual void writeAttribute(const std::string& name, const std::string& value);	// slot 11: "%s"
		virtual void writeAttribute(const std::string& name, int value);					// slot 10: "%d"
		virtual void writeAttribute(const std::string& name, float value);				// slot 9: "%f"
		virtual void writeAttribute(const std::string& name, bool value);					// slot 8: "true"/"false"
		virtual void writeAttribute(const std::string& name, const Point& value);			// slot 7: "%d,%d"
		virtual void writeAttribute(const std::string& name, const Vector2& value);		// slot 6: "%f,%f"
		virtual void writeAttribute(const std::string& name, char value);					// slot 5: "%c"
		virtual void writeAttribute(const std::string& name, const IntRange& value);		// slot 4: "(%d,%d)"
		virtual void writeAttribute(const std::string& name, const Range& value);			// slot 3: "(%f,%f)"
		virtual void writeTimeAttribute(const std::string& name, float seconds);			// slot 12: "%d:%d" minutes, seconds

		virtual void close();																// slot 0 (engine::OutputStream)
		virtual void write(const char* data, int length);									// slot 1 (engine::OutputStream)

		void writeIndent();

		std::deque<std::string> m_elementStack;	// +0x14 names of the open elements
		bool m_startTagOpen;					// +0x28 "<name " written, ">" or "/>" pending
		int m_depth;							// +0x2C indentation level (tabs)
		RefPtr<OutputStream> m_stream;			// +0x30 wrapped file stream (FileOutputStream's OutputStream part)
	};
}

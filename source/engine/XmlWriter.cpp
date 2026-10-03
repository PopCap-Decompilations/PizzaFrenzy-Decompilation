#include "XmlWriter.h"

#include <stdio.h>
#include <string.h>

#include <deque>
#include <string>

#include "FileOutputStream.h"
#include "Point.h"
#include "Range.h"

namespace engine
{
	// every tag and attribute is formatted here before it is written (0x532F88)
	static char s_xmlBuffer[256];

	// 0x476C90: one tab per open element
	void XmlWriter::writeIndent()
	{
		for (int i = 0; i < m_depth; ++i)
		{
			sprintf(s_xmlBuffer, "\t");
			m_stream->write(s_xmlBuffer, strlen(s_xmlBuffer));
		}
	}

	// 0x476CF0
	void XmlWriter::writeAttribute(const std::string& name, const std::string& value)
	{
		sprintf(s_xmlBuffer, "%s=\"%s\" ", name.c_str(), value.c_str());
		m_stream->write(s_xmlBuffer, strlen(s_xmlBuffer));
	}

	// 0x476D60
	void XmlWriter::writeAttribute(const std::string& name, int value)
	{
		sprintf(s_xmlBuffer, "%s=\"%d\" ", name.c_str(), value);
		m_stream->write(s_xmlBuffer, strlen(s_xmlBuffer));
	}

	// 0x476DC0
	void XmlWriter::writeAttribute(const std::string& name, float value)
	{
		sprintf(s_xmlBuffer, "%s=\"%f\" ", name.c_str(), value);
		m_stream->write(s_xmlBuffer, strlen(s_xmlBuffer));
	}

	// 0x476E20
	void XmlWriter::writeAttribute(const std::string& name, bool value)
	{
		sprintf(s_xmlBuffer, "%s=\"%s\" ", name.c_str(), value ? "true" : "false");
		m_stream->write(s_xmlBuffer, strlen(s_xmlBuffer));
	}

	// 0x476E90
	void XmlWriter::writeAttribute(const std::string& name, const Point& value)
	{
		sprintf(s_xmlBuffer, "%s=\"%d,%d\" ", name.c_str(), value.x, value.y);
		m_stream->write(s_xmlBuffer, strlen(s_xmlBuffer));
	}

	// 0x476EF0
	void XmlWriter::writeAttribute(const std::string& name, const Vector2& value)
	{
		sprintf(s_xmlBuffer, "%s=\"%f,%f\" ", name.c_str(), value.x, value.y);
		m_stream->write(s_xmlBuffer, strlen(s_xmlBuffer));
	}

	// 0x476F60
	void XmlWriter::writeAttribute(const std::string& name, char value)
	{
		sprintf(s_xmlBuffer, "%s=\"%c\" ", name.c_str(), value);
		m_stream->write(s_xmlBuffer, strlen(s_xmlBuffer));
	}

	// 0x476FC0
	void XmlWriter::writeAttribute(const std::string& name, const IntRange& value)
	{
		sprintf(s_xmlBuffer, "%s=\"(%d,%d)\" ", name.c_str(), value.min, value.max);
		m_stream->write(s_xmlBuffer, strlen(s_xmlBuffer));
	}

	// 0x477020
	void XmlWriter::writeAttribute(const std::string& name, const Range& value)
	{
		sprintf(s_xmlBuffer, "%s=\"(%f,%f)\" ", name.c_str(), value.min, value.max);
		m_stream->write(s_xmlBuffer, strlen(s_xmlBuffer));
	}

	// 0x477090: minutes:seconds
	void XmlWriter::writeTimeAttribute(const std::string& name, float seconds)
	{
		int time = (int)seconds;
		sprintf(s_xmlBuffer, "%s=\"%d:%d\" ", name.c_str(), time / 60, time % 60);
		m_stream->write(s_xmlBuffer, strlen(s_xmlBuffer));
	}

	// 0x477110
	void XmlWriter::close()
	{
		if (m_stream)
		{
			m_stream->close();
			m_stream = 0;
		}
	}

	// 0x477140: raw text inside the current element. An open start tag is closed with ">", but m_startTagOpen
	// stays set, as in the original.
	void XmlWriter::write(const char* data, int length)
	{
		if (m_startTagOpen)
		{
			sprintf(s_xmlBuffer, ">\n");
			m_stream->write(s_xmlBuffer, strlen(s_xmlBuffer));
			++m_depth;
		}
		m_stream->write(data, length);
	}

	// 0x477460
	XmlWriter::~XmlWriter()
	{
		close();
	}

	// 0x477520: "/>" while the start tag is still open, else the end tag
	void XmlWriter::endElement()
	{
		if (m_startTagOpen)
		{
			sprintf(s_xmlBuffer, "/>\n");
			m_stream->write(s_xmlBuffer, strlen(s_xmlBuffer));
			m_startTagOpen = false;
		}
		else
		{
			std::string name = m_elementStack.back();
			--m_depth;
			writeIndent();
			sprintf(s_xmlBuffer, "</%s>\n", name.c_str());
			m_stream->write(s_xmlBuffer, strlen(s_xmlBuffer));
		}
		m_elementStack.pop_back();
	}

	// 0x4776C0
	XmlWriter::XmlWriter(const char* fileName)
	{
		m_stream = new FileOutputStream(fileName);
		m_startTagOpen = false;
		m_depth = 0;
	}

	// 0x477800: an open start tag is closed first; the new one stays open for the attributes
	void XmlWriter::startElement(const std::string& name)
	{
		if (m_startTagOpen)
		{
			sprintf(s_xmlBuffer, ">\n");
			m_stream->write(s_xmlBuffer, strlen(s_xmlBuffer));
			++m_depth;
		}
		m_elementStack.push_back(name);
		writeIndent();
		sprintf(s_xmlBuffer, "<%s ", name.c_str());
		m_stream->write(s_xmlBuffer, strlen(s_xmlBuffer));
		m_startTagOpen = true;
	}
}

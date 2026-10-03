// engine::FileOutputStream: an output stream over a stdio file (the XML writer's file).
#pragma once

#include <stdio.h>

#include "Object.h"
#include "OutputStream.h"

namespace engine
{
	// Created by XmlWriter's constructor (0x4776C0). The file is opened "w+t"; the constructor throws
	// Exception("Unable to open file") when it cannot be opened. Layout: Object +0x00, OutputStream +0x0C,
	// m_file +0x14, then the vtordisp and Interface (0x20 bytes).
	class FileOutputStream : public Object, public OutputStream
	{
	public:
		FileOutputStream(const char* fileName);
		// closes the file if it is still open
		virtual ~FileOutputStream();

		// OutputStream
		virtual void close();
		virtual void write(const char* data, int length);

		FILE* m_file;							// +0x14 0 once closed
	};
}

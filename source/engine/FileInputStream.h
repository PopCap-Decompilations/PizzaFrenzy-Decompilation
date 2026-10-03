// engine::FileInputStream: an engine::SeekableInputStream over a stdio FILE opened "rb" (the application's
// openNativeFile).
#pragma once

#include <stdio.h>

#include <string>

#include "InputStream.h"

namespace engine
{
	// Layout: SeekableInputStream +0x00, m_file +0x0C, then the vtordisp and Interface (0x18 bytes).
	class FileInputStream : public SeekableInputStream
	{
	public:
		FileInputStream();
		virtual ~FileInputStream();

		// InputStream
		virtual int read(void* buffer, int size);
		virtual bool close();

		// SeekableInputStream
		virtual bool seek(int offset, int origin);
		virtual int tell();

		void open(const std::string& directory, const std::string& fileName);

		FILE* m_file;							// +0x0C fopen(directory + fileName, "rb"); not initialised by the constructor
	};
}

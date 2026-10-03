#include "FileOutputStream.h"

#include "Exception.h"

namespace engine
{
	// 0x4935C0
	void FileOutputStream::close()
	{
		if (m_file)
		{
			fclose(m_file);
			m_file = NULL;
		}
	}

	// 0x4935E0
	void FileOutputStream::write(const char* data, int length)
	{
		fwrite(data, 1, length, m_file);
	}

	// 0x493600
	FileOutputStream::~FileOutputStream()
	{
		if (m_file)
		{
			fclose(m_file);
			m_file = NULL;
		}
	}

	// 0x493670
	FileOutputStream::FileOutputStream(const char* fileName)
	{
		m_file = fopen(fileName, "w+t");
		if (!m_file)
			throw Exception("Unable to open file");
	}
}

#include "FileInputStream.h"

#include "Application.h"
#include "Exception.h"

namespace engine
{
	// 0x4CDFC0
	bool FileInputStream::close()
	{
		if (m_file == NULL)
			return false;
		bool closed = fclose(m_file) == 0;
		m_file = NULL;
		return closed;
	}

	// 0x4CDFF0
	int FileInputStream::read(void* buffer, int size)
	{
		return (int)fread(buffer, 1, size, m_file);
	}

	// 0x4CE010
	bool FileInputStream::seek(int offset, int origin)
	{
		return fseek(m_file, offset, origin) == 0;
	}

	// 0x4CE030
	int FileInputStream::tell()
	{
		return ftell(m_file);
	}

	// 0x4CE040
	FileInputStream::~FileInputStream()
	{
		if (m_file)
		{
			fclose(m_file);
			m_file = NULL;
		}
	}

	// 0x4CE0B0
	FileInputStream::FileInputStream()
	{
	}

	// 0x4CE1E0
	void FileInputStream::open(const std::string& directory, const std::string& fileName)
	{
		std::string path = directory;
		path += fileName;
		m_file = fopen(path.c_str(), "rb");
		if (m_file == NULL)
		{
			getApplication()->logSystem("Failed to open file: %s\n", path.c_str());
			throw Exception();
		}
	}
}

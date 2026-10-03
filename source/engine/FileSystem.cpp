#include "FileSystem.h"

#include <algorithm>
#include <string>

#include "Application.h"
#include "ArchiveLoader.h"
#include "Exception.h"
#include "InputStream.h"
#include "InputStreamReader.h"

namespace engine
{
	// 0x483E90
	void FileSystem::setUseDisk(int useDisk)
	{
		m_useDisk = useDisk;
	}

	// 0x483EA0
	SeekableInputStream* FileSystem::openDiskFile(const char* path)
	{
		return getApplication()->openNativeFile(path);
	}

	// 0x483F40
	Reader* FileSystem::openDiskReader(const char* path)
	{
		SeekableInputStream* stream = openDiskFile(path);
		if (stream)
			return new InputStreamReader(stream);
		return 0;
	}

	// 0x483FB0
	SeekableInputStream* FileSystem::openArchiveFile(const char* path)
	{
		std::string name(path);
		std::replace(name.begin(), name.end(), '\\', '/');
		for (std::vector<ArchiveLoader*>::iterator it = m_archives.begin(); it != m_archives.end(); ++it)
		{
			if ((*it)->hasFile(name))
				return (*it)->getInputStream(name);
		}
		getApplication()->logSystem("Unable to find file in archive: %s\n", name.c_str());
		return 0;
	}

	// 0x4840F0
	bool FileSystem::diskFileExists(const char* path)
	{
		return getApplication()->nativeFileExists(path);
	}

	// 0x484180
	bool FileSystem::exists(const char* path)
	{
		std::string name(path);
		std::replace(name.begin(), name.end(), '\\', '/');
		if (m_useDisk)
			return diskFileExists(path);
		for (std::vector<ArchiveLoader*>::iterator it = m_archives.begin(); it != m_archives.end(); ++it)
		{
			if ((*it)->hasFile(name))
				return true;
		}
		return false;
	}

	// 0x484320
	SeekableInputStream* FileSystem::getInputStream(const char* path)
	{
		if (m_useDisk)
			return openDiskFile(path);
		return openArchiveFile(path);
	}

	// 0x484340
	Reader* FileSystem::getReader(const char* path)
	{
		SeekableInputStream* stream = m_useDisk ? openDiskFile(path) : openArchiveFile(path);
		if (stream)
			return new InputStreamReader(stream);
		return 0;
	}

	// 0x4845B0
	FileSystem::FileSystem()
		: m_useDisk(0)
	{
	}

	// 0x4846B0: the archive keeps the reference taken here (FileSystem never releases its archives)
	bool FileSystem::addArchive(const char* file)
	{
		ArchiveLoader* archive = new ArchiveLoader();
		archive->addRef();
		try
		{
			archive->openArchive(file);
		}
		catch (Exception e)
		{
			archive->release();
			return false;
		}
		m_archives.push_back(archive);
		return true;
	}
}

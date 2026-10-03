#include "ArchiveLoader.h"

#include "Application.h"
#include "ArchiveInputStream.h"
#include "ArchiveToc.h"
#include "Exception.h"
#include "InputStream.h"
#include "StreamUtil.h"

namespace engine
{
	// 0x494E40
	ArchiveLoader::ArchiveLoader()
		: m_tocOffset(0)
	{
	}

	// 0x494ED0
	ArchiveLoader::~ArchiveLoader()
	{
		m_toc = 0;
	}

	// 0x494F90
	bool ArchiveLoader::hasFile(const std::string& name) const
	{
		return m_toc->hasEntry(name);
	}

	// 0x494FC0
	void ArchiveLoader::readToc()
	{
		std::string error;
		if (!m_file)
		{
			error = "ArchiveLoader::readToc - file not open\n";
			throw Exception(error);
		}
		try
		{
			m_file->seek(m_tocOffset, 0);
			m_toc = new ArchiveToc;
			m_toc->read(m_file);
		}
		catch (...)
		{
			error = "ArchiveLoader::readToc - unable to read TOC\n";
			throw Exception(error);
		}
	}

	// 0x495100: seeks only when the file is not already at offset
	int ArchiveLoader::Read(void* buffer, int offset, int size)
	{
		if (!m_file)
			throw Exception("ArchiveLoader::Read - attempted read from closed archive\n");
		if (m_file->tell() != offset)
			m_file->seek(offset, 0);
		return m_file->read(buffer, size);
	}

	// 0x4951B0
	SeekableInputStream* ArchiveLoader::getInputStream(const std::string& name)
	{
		ArchiveTocEntry* entry = m_toc->findEntry(name);
		if (!entry)
			throw Exception("ArchiveLoader::getInputStream - no stream found\n");
		return new ArchiveInputStream(this, entry);
	}

	// 0x495280: opens the file and checks its header (magic 'SAFF', version 1.0; this part is the original's
	// openReader, inlined), then reads the table of contents
	void ArchiveLoader::openArchive(const std::string& path)
	{
		std::string error;
		// outside the try: the original calls it in the outer EH state and enters the try after it (0x4952CD)
		Application* application = getApplication();
		try
		{
			m_file = application->openNativeFile(path);
		}
		catch (...)
		{
			error = "ArchiveLoader::openArchive: unable to open file '" + path + "' \n";
			throw Exception(error);
		}

		int magic = 0;
		unsigned short minor = 0;
		unsigned short major = 0;
		readInt32(m_file, &magic);
		readInt16(m_file, &major);
		readInt16(m_file, &minor);
		if (magic != 0x53414646)
		{
			error = "ArchiveLoader::openReader: invalid magic number\n";
			m_file = 0;
			throw Exception(error);
		}
		if (major != 1 || minor != 0)
		{
			error = "ArchiveLoader::openReader: invalid version number";
			m_file = 0;
			throw Exception(error);
		}
		readInt32(m_file, &m_tocOffset);
		readToc();
	}
}

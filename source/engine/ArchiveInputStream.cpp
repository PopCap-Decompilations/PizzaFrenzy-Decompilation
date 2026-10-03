#include "ArchiveInputStream.h"

#include <stdio.h>
#include <string.h>

#include "ArchiveLoader.h"
#include "ArchiveTocEntry.h"
#include "Exception.h"

namespace engine
{
	// 0x499870: no upper bound; origins other than SEEK_SET, SEEK_CUR and SEEK_END are absolute archive offsets.
	// Any successful seek turns off the digest check.
	bool ArchiveInputStream::seek(int offset, int origin)
	{
		unsigned int position;
		switch (origin)
		{
		case SEEK_SET:
			position = m_start + offset;
			break;
		case SEEK_CUR:
			position = m_position + offset;
			break;
		case SEEK_END:
			position = m_end + offset;
			break;
		default:
			position = offset;
			break;
		}
		if (position < m_start)
			return false;
		m_position = position;
		m_verify = false;
		return true;
	}

	// 0x4998C0
	int ArchiveInputStream::tell()
	{
		return m_position - m_start;
	}

	// 0x4998D0: the keyed MD5 of the entry starts with the archive's magic ('SAFF') as its seed
	ArchiveInputStream::ArchiveInputStream(ArchiveLoader* loader, ArchiveTocEntry* entry)
		: m_verify(true)
	{
		m_loader = loader;
		unsigned int offset = entry->getOffset();
		m_start = offset;
		m_end = offset + entry->getSize();
		m_position = offset;
		memcpy(m_expectedDigest, entry->getDigest(), 16);
		MD5Init(&m_md5, 0x53414646);
	}

	// 0x499A50
	int ArchiveInputStream::read(void* buffer, int size)
	{
		if (!m_loader)
			throw Exception("Attempt to read from closed stream\n");
		if (m_position >= m_end)
			return 0;
		if (m_position + size > m_end)
			size = m_end - m_position;
		int bytesRead = m_loader->Read(buffer, m_position, size);
		if (m_verify)
			MD5Update(&m_md5, (unsigned char*)buffer, bytesRead);
		m_position += bytesRead;
		return bytesRead;
	}

	// 0x499B30: when verifying, reads the rest of the entry and compares its digest
	bool ArchiveInputStream::close()
	{
		if (m_verify)
		{
			unsigned char buffer[128];
			while (m_position < m_end)
				read(buffer, 128);
			MD5Final(&m_md5);
			if (memcmp(m_md5.digest, m_expectedDigest, 16))
				throw Exception("Invalid Archive");
		}
		m_loader = 0;
		return true;
	}

	// 0x499C40
	ArchiveInputStream::~ArchiveInputStream()
	{
		close();
	}
}

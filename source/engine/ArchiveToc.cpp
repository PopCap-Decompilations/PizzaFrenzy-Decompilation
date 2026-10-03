#include "ArchiveToc.h"

#include <ctype.h>
#include <string.h>

#include <algorithm>
#include <string>

#include "ArchiveTocEntry.h"
#include "Exception.h"
#include "InputStream.h"
#include "StreamUtil.h"
#include "md5.h"

namespace engine
{
	// the name of the entry being read (nameLength bytes including the terminating zero; nothing checks the
	// length against the buffer)
	static char s_tocNameBuffer[260];

	// 0x498510
	void ArchiveToc::readTocHeader(SeekableInputStream* file)
	{
		unsigned short major = 0;
		unsigned short minor = 0;
		readInt16(file, &major);
		readInt16(file, &minor);
		if (major != 1 || minor != 0)
			throw Exception("ArchiveToc::readTocHeader - bad version number\n");
		memset(m_digest, 0, sizeof(m_digest));
		file->read(m_digest, 16);
	}

	// 0x4986F0
	ArchiveTocEntry* ArchiveToc::findEntry(const std::string& name) const
	{
		std::string key(name);
		std::transform(key.begin(), key.end(), key.begin(), tolower);
		std::unordered_map<std::string, ArchiveTocEntry*>::const_iterator it = m_entries.find(key);
		if (it == m_entries.end())
			return 0;
		return it->second;
	}

	// 0x4987F0
	bool ArchiveToc::hasEntry(const std::string& name) const
	{
		std::string key(name);
		std::transform(key.begin(), key.end(), key.begin(), tolower);
		return m_entries.find(key) != m_entries.end();
	}

	// 0x498E10
	ArchiveToc::ArchiveToc()
	{
	}

	// 0x498EA0
	ArchiveToc::~ArchiveToc()
	{
		for (std::unordered_map<std::string, ArchiveTocEntry*>::iterator it = m_entries.begin(); it != m_entries.end(); ++it)
			delete it->second;
		m_entries.clear();
	}

	// 0x4993D0
	void ArchiveToc::addEntry(const std::string& name, ArchiveTocEntry* entry)
	{
		std::string key(name);
		std::transform(key.begin(), key.end(), key.begin(), tolower);
		m_entries.insert(std::make_pair(key, entry));
	}

	// 0x499540: the TOC header, then the entries; everything after the TOC digest is hashed (keyed MD5) and must
	// match it
	void ArchiveToc::read(SeekableInputStream* file)
	{
		readTocHeader(file);

		MD5_CTX md5;
		MD5Init(&md5, 0x53414646);

		int count = 0;
		readInt32(file, &count);
		MD5Update(&md5, (unsigned char*)&count, 4);
		if (!count)
			throw Exception("No entries in TOC");

		for (int i = 0; i < count; i++)
		{
			int offset = 0;
			int size = 0;
			unsigned short nameLength = 0;
			unsigned char digest[16];

			readInt32(file, &offset);
			MD5Update(&md5, (unsigned char*)&offset, 4);
			readInt32(file, &size);
			MD5Update(&md5, (unsigned char*)&size, 4);
			file->read(digest, 16);
			MD5Update(&md5, digest, 16);
			readInt16(file, &nameLength);
			MD5Update(&md5, (unsigned char*)&nameLength, 2);
			file->read(s_tocNameBuffer, nameLength);
			MD5Update(&md5, (unsigned char*)s_tocNameBuffer, nameLength);

			std::string entryName(s_tocNameBuffer);
			addEntry(entryName, new ArchiveTocEntry(offset, size, digest));
		}

		MD5Final(&md5);
		if (memcmp(m_digest, md5.digest, 16))
			throw Exception("Archive TOC - validation failed\n");
	}
}

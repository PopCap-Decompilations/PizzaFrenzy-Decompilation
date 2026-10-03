// engine::ArchiveInputStream: the input stream over one entry of a .saf archive (with InputStream.h, where
// engine::InputStream and engine::SeekableInputStream are declared).
#pragma once

#include "InputStream.h"
#include "RefPtr.h"
#include "md5.h"

namespace engine
{
	class ArchiveLoader;
	class ArchiveTocEntry;

	// Created by ArchiveLoader::getInputStream. Reads through the loader and hashes what it reads; close() drains
	// the rest of the entry and checks the entry's keyed MD5 unless the stream seeked. InputStream's and
	// SeekableInputStream's constructors are inlined in its constructor. Layout: SeekableInputStream +0x00,
	// members +0x0C, then the vtordisp and Interface (0xA0 bytes).
	class ArchiveInputStream : public SeekableInputStream
	{
	public:
		// MD5Init(&m_md5, 0x53414646)
		ArchiveInputStream(ArchiveLoader* loader, ArchiveTocEntry* entry);
		// calls close(), which throws on a digest mismatch
		virtual ~ArchiveInputStream();

		// InputStream
		virtual int read(void* buffer, int size);
		virtual bool close();

		// SeekableInputStream: seek has no upper bound, and origins other than 0-2 are absolute offsets
		virtual bool seek(int offset, int origin);
		virtual int tell();

		unsigned int m_start;					// +0x0C entry offset in the archive
		unsigned int m_end;						// +0x10 m_start + entry size
		unsigned int m_position;				// +0x14 absolute archive position
		RefPtr<ArchiveLoader> m_loader;			// +0x18 0 once closed
		MD5_CTX m_md5;							// +0x1C keyed MD5 of the bytes read
		unsigned char m_expectedDigest[16];		// +0x84 the entry's digest
		bool m_verify;							// +0x94 true until the first seek
	};
}

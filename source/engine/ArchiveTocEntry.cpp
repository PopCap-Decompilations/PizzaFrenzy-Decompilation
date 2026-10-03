#include "ArchiveTocEntry.h"

#include <string.h>

namespace engine
{
	// 0x49B8B0
	const unsigned char* ArchiveTocEntry::getDigest() const
	{
		return m_digest;
	}

	// 0x49B8C0
	int ArchiveTocEntry::getSize() const
	{
		return m_size;
	}

	// 0x4BE760 (folded)
	int ArchiveTocEntry::getOffset() const
	{
		return m_offset;
	}

	// 0x49B8D0
	ArchiveTocEntry::ArchiveTocEntry(int offset, int size, const unsigned char* digest)
		: m_offset(offset), m_size(size)
	{
		memcpy(m_digest, digest, 16);
	}
}

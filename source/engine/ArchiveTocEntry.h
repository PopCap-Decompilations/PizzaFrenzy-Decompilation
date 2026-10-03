// engine::ArchiveTocEntry: one entry of a .saf archive's table of contents.
#pragma once

#include <string>

namespace engine
{
	// No vtable. Created by ArchiveToc::read and deleted by ~ArchiveToc; implicit destructor (0x34 bytes).
	class ArchiveTocEntry
	{
	public:
		// m_name stays empty
		ArchiveTocEntry(int offset, int size, const unsigned char* digest);

		const unsigned char* getDigest() const;
		int getSize() const;
		// body folded with MemoryImage::getHeight (0x4BE760)
		int getOffset() const;

		std::string m_name;						// +0x00 constructed empty, never set
		unsigned char m_digest[16];				// +0x1C keyed MD5 of the entry's bytes
		int m_offset;							// +0x2C absolute offset in the archive
		int m_size;								// +0x30
	};
}

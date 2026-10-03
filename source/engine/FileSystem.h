// engine::FileSystem: the application's file system (.saf archives, or loose files on disk).
#pragma once

#include <vector>

#include "Object.h"

namespace engine
{
	class ArchiveLoader;
	class Reader;
	class SeekableInputStream;

	// Held by the application (+0x148), whose slots 8-15 forward to it. Implicit destructor (0x484680 frees the
	// archive vector without releasing the archives). Layout: Object +0x00, m_archives +0x0C, m_useDisk +0x1C, then
	// the vtordisp and Interface (0x28 bytes).
	class FileSystem : public Object
	{
	public:
		FileSystem();

		// application slot 8: opens a .saf archive and adds it to the search list
		bool addArchive(const char* file);
		// application slot 9
		bool exists(const char* path);
		// application slot 10 (does not use this)
		bool diskFileExists(const char* path);
		// application slot 11
		void setUseDisk(int useDisk);
		// application slot 12
		SeekableInputStream* getInputStream(const char* path);
		// application slot 13
		Reader* getReader(const char* path);
		// application slot 14 (does not use this)
		SeekableInputStream* openDiskFile(const char* path);
		// application slot 15
		Reader* openDiskReader(const char* path);
		SeekableInputStream* openArchiveFile(const char* path);

		std::vector<ArchiveLoader*> m_archives;	// +0x0C added by addArchive (addRef'd once, never released)
		int m_useDisk;							// +0x1C nonzero: loose files through the application instead of the archives
	};
}

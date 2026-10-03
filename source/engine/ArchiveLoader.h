// engine::ArchiveLoader: an open .saf archive (Game\PizzaFrenzy.saf) and its table of contents.
#pragma once

#include <string>

#include "Object.h"
#include "RefPtr.h"

namespace engine
{
	class ArchiveToc;
	class SeekableInputStream;

	// Created by FileSystem::addArchive. openArchive checks the header (magic 0x53414646 "FFAS", version 1.0,
	// TOC offset) and reads the TOC; getInputStream creates an ArchiveInputStream per entry. Layout: Object +0x00,
	// members +0x0C, then the vtordisp and Interface (0x20 bytes).
	class ArchiveLoader : public Object
	{
	public:
		ArchiveLoader();
		// m_toc = 0 first, then the members
		virtual ~ArchiveLoader();

		// m_toc->hasEntry(name)
		bool hasFile(const std::string& name) const;
		// throws "ArchiveLoader::readToc - file not open\n"; seeks to the TOC and reads it
		void readToc();
		// reads size bytes at offset of the archive file (seeks only if tell() differs); throws without a file
		int Read(void* buffer, int offset, int size);
		// new ArchiveInputStream for the entry (FileSystem::openArchiveFile returns it as it is); throws
		// "ArchiveLoader::getInputStream - no stream found\n"
		SeekableInputStream* getInputStream(const std::string& name);
		// opens the file through Application slot 33 and checks the header (the inlined openReader), then readToc()
		void openArchive(const std::string& path);

		RefPtr<ArchiveToc> m_toc;				// +0x0C
		RefPtr<SeekableInputStream> m_file;		// +0x10 the archive file (Application slot 33)
		int m_tocOffset;						// +0x14 from the file header
	};
}

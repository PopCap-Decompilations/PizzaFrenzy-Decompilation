// engine::ArchiveToc: the table of contents of a .saf archive.
#pragma once

#include <string>
#include <unordered_map>

#include "Object.h"

namespace engine
{
	class ArchiveTocEntry;
	class SeekableInputStream;

	// Entries by lowercased name, read and verified (keyed MD5) by read(); the destructor deletes the entries.
	// Created by ArchiveLoader::readToc. Layout: Object +0x00, members +0x0C, then the vtordisp and Interface
	// (0x4C bytes).
	class ArchiveToc : public Object
	{
	public:
		ArchiveToc();
		virtual ~ArchiveToc();

		// version 1.0 (else "ArchiveToc::readTocHeader - bad version number\n"), then the TOC digest
		void readTocHeader(SeekableInputStream* file);
		// the lowercased name's entry, 0 if absent
		ArchiveTocEntry* findEntry(const std::string& name) const;
		bool hasEntry(const std::string& name) const;
		// the key is the lowercased name
		void addEntry(const std::string& name, ArchiveTocEntry* entry);
		// readTocHeader, then the entry records; throws "No entries in TOC" and "Archive TOC - validation failed\n"
		void read(SeekableInputStream* file);

		// VS2003's stdext::hash_map<std::string, ArchiveTocEntry*>: <hash_map> no longer exists in the modern
		// library, and unordered_map is its standard replacement (only insert, find and iteration are used)
		std::unordered_map<std::string, ArchiveTocEntry*> m_entries;	// +0x0C keys lowercased
		unsigned char m_digest[16];				// +0x34 TOC digest from the TOC header
	};
}

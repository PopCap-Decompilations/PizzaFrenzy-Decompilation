#include "MemoryTracker.h"

namespace engine
{
	bool g_memoryTrackerActive;
	MemoryTracker g_memoryTracker;

	// 0x497720
	MemoryTracker::MemoryTracker()
	{
		m_unused = 0;
		m_count = 0;
		m_defaultFile = "unknown file";
		m_file = "unknown file";
		m_line = -1;
		g_memoryTrackerActive = true;
	}

	// 0x497770
	MemoryTracker::~MemoryTracker()
	{
		g_memoryTrackerActive = false;
		debugLog(1, "%d memory allocations outstanding.\n", m_count);
	}

	// 0x4977E0
	void MemoryTracker::setSource(const char* file, int line)
	{
		g_memoryTracker.m_file = file;
		g_memoryTracker.m_line = line;
	}

	// 0x49B6D0
	MemoryTracker::Allocation::Allocation()
		: m_pointer(0), m_file(0), m_line(-1)
	{
	}

	// 0x49B6F0
	MemoryTracker::Allocation::~Allocation()
	{
		if (m_pointer)
			debugLog(1, "%s(%d) : warning XXXX: mem not deleted\n", m_file, m_line);
	}

	// 0x4D0470 (folded)
	void debugLog(int level, const char* format, ...)
	{
	}
}

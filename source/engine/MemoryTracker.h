// engine::MemoryTracker (the allocation tracker g_memoryTracker) and engine::debugLog, the engine's debug facilities.
#pragma once

namespace engine
{
	// Debug-new allocation tracker: the engine's allocation sites call setSource(__FILE__, __LINE__) before new,
	// but nothing records allocations in this build, so only the destructor's report remains. No vtable
	// (0xC494 bytes); g_memoryTracker is the only instance (dynamic initializer and atexit destructor).
	class MemoryTracker
	{
	public:
		// one allocation slot (12 bytes); the destructor warns about a pointer still set
		struct Allocation
		{
			Allocation();
			~Allocation();

			void* m_pointer;					// +0x00 0
			const char* m_file;					// +0x04 0
			int m_line;							// +0x08 -1
		};

		// m_defaultFile and m_file "unknown file", m_line -1, m_count 0; g_memoryTrackerActive = true
		MemoryTracker();
		// g_memoryTrackerActive = false; logs the outstanding count
		~MemoryTracker();

		// the source position of the next allocation: g_memoryTracker.m_file and m_line
		static void setSource(const char* file, int line);

		const char* m_defaultFile;				// +0x0000 "unknown file"
		const char* m_file;						// +0x0004 set by setSource
		int m_line;								// +0x0008 set by setSource
		Allocation m_allocations[4192];			// +0x000C
		int m_count;							// +0xC48C outstanding allocations
		int m_unused;							// +0xC490 zeroed by the constructor, never read
	};

	// set by MemoryTracker's constructor, cleared by its destructor
	extern bool g_memoryTrackerActive;
	extern MemoryTracker g_memoryTracker;

	// The debug log: a cdecl variadic function whose body is empty in this build (it is the folded empty function
	// 0x4D0470), so only the calls and their arguments remain.
	void debugLog(int level, const char* format, ...);
}

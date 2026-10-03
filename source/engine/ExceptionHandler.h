// engine::ExceptionHandler: the crash reporter (Matt Pietrek's MSJExceptionHandler, "Under the Hood", MSJ, with
// instance data, a minidump and a message box) and its global instance, whose <exe>.log the application logs to.
#pragma once

#include <stdarg.h>

#include <windows.h>
#include <dbghelp.h>

namespace engine
{
	// No vtable; 0x22C bytes. Constructed by the static initializer of g_exceptionHandler (0x4F89E0); its
	// destructor exists only inlined in the atexit thunk (0x4F8CA0).
	class ExceptionHandler
	{
	public:
		// IMAGEHLP.DLL and DBGHELP.DLL functions, loaded with GetProcAddress
		typedef BOOL (__stdcall* SymInitializeProc)(HANDLE, LPSTR, BOOL);
		typedef BOOL (__stdcall* SymCleanupProc)(HANDLE);
		typedef BOOL (__stdcall* StackWalkProc)(DWORD, HANDLE, HANDLE, LPSTACKFRAME, LPVOID, PREAD_PROCESS_MEMORY_ROUTINE,
			PFUNCTION_TABLE_ACCESS_ROUTINE, PGET_MODULE_BASE_ROUTINE, PTRANSLATE_ADDRESS_ROUTINE);
		typedef LPVOID (__stdcall* SymFunctionTableAccessProc)(HANDLE, DWORD);
		typedef DWORD (__stdcall* SymGetModuleBaseProc)(HANDLE, DWORD);
		typedef BOOL (__stdcall* SymGetSymFromAddrProc)(HANDLE, DWORD, PDWORD, PIMAGEHLP_SYMBOL);
		typedef BOOL (__stdcall* MiniDumpWriteDumpProc)(HANDLE, DWORD, HANDLE, MINIDUMP_TYPE,
			PMINIDUMP_EXCEPTION_INFORMATION, PMINIDUMP_USER_STREAM_INFORMATION, PMINIDUMP_CALLBACK_INFORMATION);

		ExceptionHandler();
		~ExceptionHandler();

		static ExceptionHandler* getInstance();
		static LONG WINAPI unhandledExceptionFilter(EXCEPTION_POINTERS* info);

		// getExceptionString and getLogicalAddress are thiscall members that do not use this (their callers load ecx);
		// formatMessage (this unused too), vprintf and printf are cdecl members, with this on the stack.
		const char* getExceptionString(DWORD code);
		bool getLogicalAddress(void* address, char* moduleName, DWORD length, DWORD& section, DWORD& offset);
		char* formatMessage(const char* format, ...);
		void write(const char* text);
		bool initImagehlpFunctions();
		void openLogFile();
		HRESULT __cdecl vprintf(const char* format, va_list args);
		HRESULT printf(const char* format, ...);
		void writeMiniDump(EXCEPTION_POINTERS* info);
		void intelStackWalk(CONTEXT* context);
		void imagehlpStackWalk(CONTEXT* context);
		void generateExceptionReport(EXCEPTION_POINTERS* info);
		LONG handleException(EXCEPTION_POINTERS* info);

		char m_logFileName[MAX_PATH];							// +0x000 <exe path>.log
		char m_dumpFileName[MAX_PATH];							// +0x104 <exe path>.dmp
		HANDLE m_logFile;										// +0x208 0 when not open (tested against 0, not INVALID_HANDLE_VALUE)
		LPTOP_LEVEL_EXCEPTION_FILTER m_previousFilter;			// +0x20C returned by SetUnhandledExceptionFilter
		SymInitializeProc m_symInitialize;						// +0x210
		SymCleanupProc m_symCleanup;							// +0x214
		StackWalkProc m_stackWalk;								// +0x218
		SymFunctionTableAccessProc m_symFunctionTableAccess;	// +0x21C
		SymGetModuleBaseProc m_symGetModuleBase;				// +0x220
		SymGetSymFromAddrProc m_symGetSymFromAddr;				// +0x224
		MiniDumpWriteDumpProc m_miniDumpWriteDump;				// +0x228 not initialised by the constructor
	};
}

extern engine::ExceptionHandler g_exceptionHandler;

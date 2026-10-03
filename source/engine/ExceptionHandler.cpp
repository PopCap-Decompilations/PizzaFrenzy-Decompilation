#include <mbstring.h>
#include <string.h>

#include <windows.h>
#include <strsafe.h>

#include "ExceptionHandler.h"

#include "Application.h"

namespace engine
{
	// 0x4C0C10
	ExceptionHandler* ExceptionHandler::getInstance()
	{
		return &g_exceptionHandler;
	}

	// 0x4C0C20
	const char* ExceptionHandler::getExceptionString(DWORD code)
	{
		switch (code)
		{
		case EXCEPTION_ACCESS_VIOLATION:
			return "ACCESS_VIOLATION";
		case EXCEPTION_DATATYPE_MISALIGNMENT:
			return "DATATYPE_MISALIGNMENT";
		case EXCEPTION_BREAKPOINT:
			return "BREAKPOINT";
		case EXCEPTION_SINGLE_STEP:
			return "SINGLE_STEP";
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			return "ARRAY_BOUNDS_EXCEEDED";
		case EXCEPTION_FLT_DENORMAL_OPERAND:
			return "FLT_DENORMAL_OPERAND";
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			return "FLT_DIVIDE_BY_ZERO";
		case EXCEPTION_FLT_INEXACT_RESULT:
			return "FLT_INEXACT_RESULT";
		case EXCEPTION_FLT_INVALID_OPERATION:
			return "FLT_INVALID_OPERATION";
		case EXCEPTION_FLT_OVERFLOW:
			return "FLT_OVERFLOW";
		case EXCEPTION_FLT_STACK_CHECK:
			return "FLT_STACK_CHECK";
		case EXCEPTION_FLT_UNDERFLOW:
			return "FLT_UNDERFLOW";
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			return "INT_DIVIDE_BY_ZERO";
		case EXCEPTION_INT_OVERFLOW:
			return "INT_OVERFLOW";
		case EXCEPTION_PRIV_INSTRUCTION:
			return "PRIV_INSTRUCTION";
		case EXCEPTION_IN_PAGE_ERROR:
			return "IN_PAGE_ERROR";
		case EXCEPTION_ILLEGAL_INSTRUCTION:
			return "ILLEGAL_INSTRUCTION";
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:
			return "NONCONTINUABLE_EXCEPTION";
		case EXCEPTION_STACK_OVERFLOW:
			return "STACK_OVERFLOW";
		case EXCEPTION_INVALID_DISPOSITION:
			return "INVALID_DISPOSITION";
		case EXCEPTION_GUARD_PAGE:
			return "GUARD_PAGE";
		case EXCEPTION_INVALID_HANDLE:
			return "INVALID_HANDLE";
		}
		// not one of the known exceptions: NTDLL.DLL's message table
		static char s_exceptionString[512];
		FormatMessageA(FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_HMODULE, GetModuleHandleA("NTDLL.DLL"), code, 0,
			s_exceptionString, sizeof(s_exceptionString), 0);
		return s_exceptionString;
	}

	// 0x4C0EC0
	bool ExceptionHandler::getLogicalAddress(void* address, char* moduleName, DWORD length, DWORD& section, DWORD& offset)
	{
		MEMORY_BASIC_INFORMATION memoryInfo;
		if (!VirtualQuery(address, &memoryInfo, sizeof(memoryInfo)))
		{
			return false;
		}
		DWORD module = (DWORD)memoryInfo.AllocationBase;
		if (!GetModuleFileNameA((HMODULE)module, moduleName, length))
		{
			return false;
		}
		IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)module;
		IMAGE_NT_HEADERS* ntHeader = (IMAGE_NT_HEADERS*)(module + dosHeader->e_lfanew);
		IMAGE_SECTION_HEADER* sectionHeader = IMAGE_FIRST_SECTION(ntHeader);
		DWORD rva = (DWORD)address - module;
		for (unsigned int i = 0; i < ntHeader->FileHeader.NumberOfSections; i++, sectionHeader++)
		{
			DWORD sectionStart = sectionHeader->VirtualAddress;
			DWORD sectionEnd = sectionStart + max(sectionHeader->SizeOfRawData, sectionHeader->Misc.VirtualSize);
			if (rva >= sectionStart && rva <= sectionEnd)
			{
				section = i + 1;
				offset = rva - sectionStart;
				return true;
			}
		}
		return false;
	}

	// 0x4C0F70
	char* ExceptionHandler::formatMessage(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		char* message;
		return FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_STRING, format, 0, 0, (LPSTR)&message, 0,
			&args) != 0 ? message : 0;
	}

	// 0x4C0FB0
	void ExceptionHandler::write(const char* text)
	{
		if (m_logFile != 0)
		{
			DWORD written;
			WriteFile(m_logFile, text, lstrlenA(text), &written, 0);
		}
	}

	// 0x4C0FF0
	bool ExceptionHandler::initImagehlpFunctions()
	{
		HMODULE imagehlp = LoadLibraryA("IMAGEHLP.DLL");
		if (imagehlp == 0)
		{
			return false;
		}
		m_symInitialize = (SymInitializeProc)GetProcAddress(imagehlp, "SymInitialize");
		if (m_symInitialize == 0)
		{
			return false;
		}
		m_symCleanup = (SymCleanupProc)GetProcAddress(imagehlp, "SymCleanup");
		if (m_symCleanup == 0)
		{
			return false;
		}
		m_stackWalk = (StackWalkProc)GetProcAddress(imagehlp, "StackWalk");
		if (m_stackWalk == 0)
		{
			return false;
		}
		m_symFunctionTableAccess = (SymFunctionTableAccessProc)GetProcAddress(imagehlp, "SymFunctionTableAccess");
		if (m_symFunctionTableAccess == 0)
		{
			return false;
		}
		m_symGetModuleBase = (SymGetModuleBaseProc)GetProcAddress(imagehlp, "SymGetModuleBase");
		if (m_symGetModuleBase == 0)
		{
			return false;
		}
		m_symGetSymFromAddr = (SymGetSymFromAddrProc)GetProcAddress(imagehlp, "SymGetSymFromAddr");
		if (m_symGetSymFromAddr == 0)
		{
			return false;
		}
		return m_symInitialize(GetCurrentProcess(), 0, TRUE) != FALSE;
	}

	// 0x4C10A0
	void ExceptionHandler::openLogFile()
	{
		if (m_logFile != 0)
		{
			return;
		}
		m_logFile = CreateFileA(m_logFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_FLAG_WRITE_THROUGH, 0);
		if (m_logFile != 0)
		{
			char* buffer = getTextBuffer();
			int size = getTextBufferSize();
			SYSTEMTIME time;
			GetSystemTime(&time);
			write("Log file created at\n");
			GetDateFormatA(LOCALE_SYSTEM_DEFAULT, LOCALE_NOUSEROVERRIDE | DATE_LONGDATE, &time, 0, buffer, size);
			write(buffer);
			write("\n");
			GetTimeFormatA(LOCALE_SYSTEM_DEFAULT, LOCALE_NOUSEROVERRIDE, &time, 0, buffer, size);
			write(buffer);
			write("\n\n");
		}
	}

	// 0x4C1200
	HRESULT __cdecl ExceptionHandler::vprintf(const char* format, va_list args)
	{
		char* buffer = getTextBuffer();
		HRESULT result = StringCchVPrintfA(buffer, getTextBufferSize(), format, args);
		write(buffer);
		return result;
	}

	// 0x4C1270
	HRESULT ExceptionHandler::printf(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		return vprintf(format, args);
	}

	// 0x4C1290
	void ExceptionHandler::writeMiniDump(EXCEPTION_POINTERS* info)
	{
		HANDLE file = 0;
		HMODULE dbghelp = LoadLibraryA("DBGHELP.DLL");
		if (dbghelp != 0
			&& (m_miniDumpWriteDump = (MiniDumpWriteDumpProc)GetProcAddress(dbghelp, "MiniDumpWriteDump")) != 0)
		{
			file = CreateFileA(m_dumpFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
				FILE_FLAG_WRITE_THROUGH | FILE_ATTRIBUTE_NORMAL, 0);
			if (file != 0)
			{
				MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
				exceptionInfo.ThreadId = GetCurrentThreadId();
				exceptionInfo.ExceptionPointers = info;
				exceptionInfo.ClientPointers = FALSE;
				m_miniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file, MiniDumpWithDataSegs, &exceptionInfo, 0,
					0);
				CloseHandle(file);
				printf("Mini-dump written to %s\n", m_dumpFileName);
			}
			else
			{
				printf("Could not create file for mini-dump %s.\n", m_dumpFileName);
			}
		}
		else
		{
			write("DBGHELP.DLL or its exported procs not found.\n");
		}
		char* message;
		if (file != 0)
		{
			message = formatMessage("We're sorry.  An unexpected internal error has forced the game to exit. \n\n"
				"Error information has been save in the following files \n\n%1\n%2\n\n"
				"You can help us determine the cause of the problem!\n"
				"Please e-mail this file to support@sproutgames.com,\n"
				"along with as detailed a description as possible of\n"
				"what you were doing when it crashed.", m_dumpFileName, m_logFileName);
		}
		else
		{
			message = formatMessage("We're sorry.  An unexpected internal error has forced the game to exit. \n\n"
				"Error information has been saved in a file called \n\n%1\n\n"
				"You can help us determine the cause of the problem!\n"
				"Please e-mail this file to support@sproutgames.com,\n"
				"along with as detailed a description as possible of\n"
				"what you were doing when it crashed.", m_logFileName);
		}
		if (message != 0)
		{
			MessageBoxA(0, message, "Exception", MB_ICONERROR);
			LocalFree(message);
		}
	}

	// 0x4C13C0
	void ExceptionHandler::intelStackWalk(CONTEXT* context)
	{
		write("\nCall stack:\n");
		write("Address   Frame     Logical addr  Module\n");
		DWORD pc = context->Eip;
		DWORD* frame = (DWORD*)context->Ebp;
		DWORD* previousFrame;
		do
		{
			char moduleName[MAX_PATH] = "";
			DWORD section = 0;
			DWORD offset = 0;
			getLogicalAddress((void*)pc, moduleName, sizeof(moduleName), section, offset);
			printf("%08X  %08X  %04X:%08X %s\n", pc, frame, section, offset, moduleName);
			pc = frame[1];
			previousFrame = frame;
			frame = (DWORD*)frame[0];		// the next higher frame
			if ((DWORD)frame & 3)
			{
				break;
			}
			if (frame <= previousFrame)
			{
				break;
			}
			if (IsBadWritePtr(frame, sizeof(void*) * 2))
			{
				break;
			}
		} while (1);
	}

	// 0x4C1550
	void ExceptionHandler::imagehlpStackWalk(CONTEXT* context)
	{
		write("\nCall stack:\n");
		write("Address   Frame\n");
		STACKFRAME stackFrame;
		memset(&stackFrame, 0, sizeof(stackFrame));
		stackFrame.AddrPC.Offset = context->Eip;
		stackFrame.AddrPC.Mode = AddrModeFlat;
		stackFrame.AddrStack.Offset = context->Esp;
		stackFrame.AddrStack.Mode = AddrModeFlat;
		stackFrame.AddrFrame.Offset = context->Ebp;
		stackFrame.AddrFrame.Mode = AddrModeFlat;
		while (1)
		{
			if (!m_stackWalk(IMAGE_FILE_MACHINE_I386, GetCurrentProcess(), GetCurrentThread(), &stackFrame, context, 0,
				m_symFunctionTableAccess, m_symGetModuleBase, 0))
			{
				break;
			}
			if (stackFrame.AddrFrame.Offset == 0)
			{
				break;
			}
			printf("%08X  %08X  ", stackFrame.AddrPC.Offset, stackFrame.AddrFrame.Offset);
			BYTE symbolBuffer[sizeof(IMAGEHLP_SYMBOL) + 512];
			IMAGEHLP_SYMBOL* symbol = (IMAGEHLP_SYMBOL*)symbolBuffer;
			symbol->SizeOfStruct = sizeof(symbolBuffer);
			symbol->MaxNameLength = 512;
			DWORD displacement = 0;
			if (m_symGetSymFromAddr(GetCurrentProcess(), stackFrame.AddrPC.Offset, &displacement, symbol))
			{
				printf("%hs+%X\n", symbol->Name, displacement);
			}
			else
			{
				char moduleName[MAX_PATH] = "";
				DWORD section = 0;
				DWORD offset = 0;
				getLogicalAddress((void*)stackFrame.AddrPC.Offset, moduleName, sizeof(moduleName), section, offset);
				printf("%04X:%08X %s\n", section, offset, moduleName);
			}
		}
	}

	// 0x4C1780
	void ExceptionHandler::generateExceptionReport(EXCEPTION_POINTERS* info)
	{
		write("//=====================================================\n");
		EXCEPTION_RECORD* record = info->ExceptionRecord;
		printf("Exception code: %08X %s\n", record->ExceptionCode, getExceptionString(record->ExceptionCode));
		// uninitialised in the original too: printed as they are when getLogicalAddress fails (a fault address outside
		// every module's sections), only on such a crash
		char moduleName[MAX_PATH];
		DWORD section;
		DWORD offset;
		getLogicalAddress(record->ExceptionAddress, moduleName, sizeof(moduleName), section, offset);
		printf("Fault address:  %08X %02X:%08X %s\n", record->ExceptionAddress, section, offset, moduleName);
		CONTEXT* context = info->ContextRecord;
		write("\nRegisters:\n");
		printf("EAX:%08X\nEBX:%08X\nECX:%08X\nEDX:%08X\nESI:%08X\nEDI:%08X\n", context->Eax, context->Ebx, context->Ecx,
			context->Edx, context->Esi, context->Edi);
		printf("CS:EIP:%04X:%08X\n", context->SegCs, context->Eip);
		printf("SS:ESP:%04X:%08X  EBP:%08X\n", context->SegSs, context->Esp, context->Ebp);
		printf("DS:%04X  ES:%04X  FS:%04X  GS:%04X\n", context->SegDs, context->SegEs, context->SegFs, context->SegGs);
		printf("Flags:%08X\n", context->EFlags);
		if (!initImagehlpFunctions())
		{
			write("IMAGEHLP.DLL or its exported procs not found.\n");
			intelStackWalk(context);
			return;
		}
		imagehlpStackWalk(context);
		m_symCleanup(GetCurrentProcess());
		write("\n");
	}

	// 0x4C19A0
	LONG ExceptionHandler::handleException(EXCEPTION_POINTERS* info)
	{
		writeMiniDump(info);
		if (m_logFile != 0)
		{
			generateExceptionReport(info);
		}
		if (m_previousFilter != 0)
		{
			write("calling previous filter\n");
			return m_previousFilter(info);
		}
		write("returning EXCEPTION_EXECUTE_HANDLER\n");
		return EXCEPTION_EXECUTE_HANDLER;
	}

	// 0x4C1A40
	LONG WINAPI ExceptionHandler::unhandledExceptionFilter(EXCEPTION_POINTERS* info)
	{
		return g_exceptionHandler.handleException(info);
	}

	// 0x4C1A50
	ExceptionHandler::ExceptionHandler()
		: m_logFile(0), m_symInitialize(0), m_symCleanup(0), m_stackWalk(0), m_symFunctionTableAccess(0),
		m_symGetModuleBase(0), m_symGetSymFromAddr(0)
	{
		m_previousFilter = SetUnhandledExceptionFilter(unhandledExceptionFilter);
		SetErrorMode(SEM_NOGPFAULTERRORBOX);
		// <exe path>.log and <exe path>.dmp: the extension after the last '.' replaced when it has 3 characters or more
		GetModuleFileNameA(0, m_logFileName, MAX_PATH);
		char* dot = (char*)_mbsrchr((unsigned char*)m_logFileName, '.');
		if (dot != 0)
		{
			dot++;
			if (strlen(dot) >= 3)
			{
				StringCchCopyA(dot, MAX_PATH - (dot - m_logFileName), "log");
			}
		}
		GetModuleFileNameA(0, m_dumpFileName, MAX_PATH);
		dot = (char*)_mbsrchr((unsigned char*)m_dumpFileName, '.');
		if (dot != 0)
		{
			dot++;
			if (strlen(dot) >= 3)
			{
				StringCchCopyA(dot, MAX_PATH - (dot - m_dumpFileName), "dmp");
			}
		}
		openLogFile();
	}

	// Inlined in g_exceptionHandler's atexit thunk (at 4F8CA0); no out-of-line copy.
	ExceptionHandler::~ExceptionHandler()
	{
		if (m_logFile != 0)
		{
			CloseHandle(m_logFile);
			m_logFile = 0;
		}
		SetUnhandledExceptionFilter(m_previousFilter);
	}
}

// Constructed by the static initializer at 4F89E0, destroyed by its atexit thunk at 4F8CA0.
engine::ExceptionHandler g_exceptionHandler;

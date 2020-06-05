#pragma once

#include <iostream>
#include <Windows.h>
#include <Psapi.h>
#include <DbgHelp.h>

#pragma comment(lib,"Dbghelp.lib")

class CDumpMaker
{
public:
	CDumpMaker() {

		_DumpCount = 0;

		_invalid_parameter_handler oldHandler;
		_invalid_parameter_handler newHandler;

		newHandler = MyInvalidParameterHandler;

		oldHandler = _set_invalid_parameter_handler(newHandler);
		_CrtSetReportMode(_CRT_WARN, 0);
		_CrtSetReportMode(_CRT_ASSERT, 0);
		_CrtSetReportMode(_CRT_ERROR, 0);
		_CrtSetReportHook(CustomReportHook);

		_set_purecall_handler(MyPureCallHandler);
		SetHandlerDump();	
	}

	
	static void Crash() {

		int *p = NULL;
		*p = 0;
	}

	static long WINAPI MyExceptionFilter(__in PEXCEPTION_POINTERS pExceptionPtr) {

		int iWorkingMemory = 0;
		SYSTEMTIME stNowTime;

		long DumpCount = InterlockedIncrement(&_DumpCount);

		//
		// 현재 프로세스의 메모리 사용량을 얻어온다.
		//
		HANDLE hProcess = 0;
		PROCESS_MEMORY_COUNTERS pmc;

		hProcess = GetCurrentProcess();
		
		if (hProcess == NULL)
		{
			return 0;
		}

		if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
		{
			iWorkingMemory = (int)(pmc.WorkingSetSize / 1024 / 1024);
		}

		CloseHandle(hProcess);

		// 현재 날짜와 시간을 알아온다.

		WCHAR filename[MAX_PATH];
		GetLocalTime(&stNowTime);
		wsprintf(filename, L"Dump_%d%02d_%02d.%02d.%02d_%d_%dMB.dmp", stNowTime.wYear, stNowTime.wMonth, stNowTime.wDay, stNowTime.wHour, stNowTime.wMinute, stNowTime.wSecond, DumpCount, iWorkingMemory);
		wprintf(L"\n\n\n !!! Crash Error !!! %d.%d.%d/ %d:%d:%d\n", stNowTime.wYear, stNowTime.wMonth, stNowTime.wDay, stNowTime.wHour, stNowTime.wMinute, stNowTime.wSecond);
		wprintf(L"Now Save Dump File...\n");

		HANDLE hDumpFile = ::CreateFile(filename,
			GENERIC_WRITE,
			FILE_SHARE_WRITE,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if (hDumpFile != INVALID_HANDLE_VALUE)
		{
			_MINIDUMP_EXCEPTION_INFORMATION miniDumpExceptionInformation;
			miniDumpExceptionInformation.ThreadId = GetCurrentThreadId();
			miniDumpExceptionInformation.ExceptionPointers = pExceptionPtr;
			miniDumpExceptionInformation.ClientPointers = true;

			MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, 
				MiniDumpWithFullMemory, &miniDumpExceptionInformation, NULL, NULL);

			CloseHandle(hDumpFile);
			wprintf(L"Crash Dump Save Finish!!!\n");
		}
		return EXCEPTION_EXECUTE_HANDLER;
	}

	static void SetHandlerDump() {

		SetUnhandledExceptionFilter(MyExceptionFilter);
	}

	static void MyInvalidParameterHandler(const wchar_t *Expression, const wchar_t *Function,
		const wchar_t *File, unsigned int Line, uintptr_t pReserved) {

		Crash();
	}

	static int CustomReportHook(int iRepostType, char* msg, int* returnValue)
	{
		Crash();
		return true;
	}

	static void MyPureCallHandler(void)
	{
		Crash();
	}

	static long _DumpCount;
};

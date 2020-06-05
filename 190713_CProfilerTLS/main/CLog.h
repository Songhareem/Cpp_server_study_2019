#pragma once
#include <iostream>
#include <Windows.h>
#include <string>
#include <strsafe.h>
#include <locale>

using namespace std;

enum en_LOG_LEVEL
{
	LOG_DEBUG = 0,
	LOG_WARNING,
	LOG_ERROR,
	LOG_SYSTEM
};

class CLog
{
private:
	CLog()
	{
		setlocale(LC_ALL, "");
		InitializeSRWLock(&fileLocker);
	}
	~CLog()
	{

	}
public:
	static CLog* GetInstance()
	{
		return &log;
	}

	void Log(const WCHAR* szType, en_LOG_LEVEL level, const WCHAR* szStringFormat, ...)
	{
		if (level < this->logLevel) { return; }

		WCHAR buf[512] = { 0, };
		va_list arg;
		int count;
		wstring buffer;

		buffer += L"[";
		buffer += szType;
		buffer += L"] [";
		buffer += TimeToWstring();
		buffer += L" / ";
		buffer += LevelToWstring(level);
		buffer += L" / ";
		buffer += CountToWstring(InterlockedIncrement(&logCount));
		buffer += L"] ";

		va_start(arg, szStringFormat);
		count = StringCchVPrintfW(buf, 512, szStringFormat, arg);
		va_end(arg);

		buffer += buf;
		buffer += L"\n";

		wcout << buffer << endl;

		
		SaveLogFile(szType, buffer);
		
	}
	void LogHex(const WCHAR* szType, en_LOG_LEVEL level, const WCHAR* szLog, BYTE* pByte, int iByteLen) {

		if (level < this->logLevel) { return; }

		WCHAR buf[10] = { 0, };
		va_list arg;
		int count;
		wstring buffer;

		buffer += L"[";
		buffer += szType;
		buffer += L"] [";
		buffer += TimeToWstring();
		buffer += L" / ";
		buffer += LevelToWstring(level);
		buffer += L" / ";
		buffer += CountToWstring(InterlockedIncrement(&logCount));
		buffer += L"] ";
		buffer += szLog;
		buffer += L"\n";
		
		buffer += L" > ";
		buffer += L"0x ";
		for(int i=0; i<iByteLen; i++) {
			StringCbPrintfW(buf, 512, L"%02x ", pByte[i]);
			buffer += buf;
		}
		buffer += L"\n";

		wcout << buffer << endl;
	
		SaveLogFile(szType, buffer);
	}

	void LogSessionKey(WCHAR* szType, en_LOG_LEVEL LogLevel, WCHAR* szLog, BYTE* pSessionKey);

	void SetLogLevel(en_LOG_LEVEL lv)
	{
		logLevel = lv;
	}

	void SetDirectory(const WCHAR* dir)
	{
		int ret = _wmkdir(dir);
		if (ret == ENOENT)
		{
			printf("Error CLog->SetDirectory() : ENOENT\n");
			return;
		}
		wmemset(saveDirectory, 0, MAX_PATH);
		wmemcpy(saveDirectory, dir, MAX_PATH);
	}

private:
	//======================================================
	// 내부 함수
	//======================================================
	void SaveLogFile(const WCHAR* type, wstring buffer)
	{
		wstring temp = GetFileName(type);
		const WCHAR* fileName = temp.c_str();

		AcquireSRWLockExclusive(&fileLocker);

		if (_wfopen_s(&fp, fileName, L"at, ccs=UTF-16LE") == 0)
		{
			fputws(buffer.c_str(), fp);
			fclose(fp);
		}

		ReleaseSRWLockExclusive(&fileLocker);
	}

	wstring GetFileName(const WCHAR* type)
	{
		WCHAR buf[64] = { 0, };
		SYSTEMTIME st;
		GetLocalTime(&st);
		StringCchPrintfW(buf, 64, L"%04d%02d_", st.wYear, st.wMonth);

		wstring fileName = saveDirectory;
		fileName += buf;
		fileName += type;
		fileName += L".txt";
		
		return fileName;
	}

	wstring TimeToWstring()
	{
		wstring ret;
		WCHAR buf[512] = { 0, };
		SYSTEMTIME st;
		GetLocalTime(&st);

		StringCchPrintfW(buf, 512, L"%04d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		ret = buf;
		return ret;
	}

	wstring LevelToWstring(en_LOG_LEVEL level)
	{
		wstring buffer;
		switch (level)
		{
		case LOG_DEBUG:
			buffer = L"DEBUG";
			break;
		case LOG_WARNING:
			buffer = L"WARNING";
			break;
		case LOG_ERROR:
			buffer = L"ERROR";
			break;
		case LOG_SYSTEM:
			buffer = L"SYSTEM";
			break;
		default:
			buffer = L"";
			break;
		}
		return buffer;
	}

	wstring CountToWstring(long count)
	{
		wstring ret;
		WCHAR buf[10];
		StringCchPrintfW(buf, 10, L"%08d", count);
		ret = buf;
		return ret;
	}

private:
	long logCount = 0;
	en_LOG_LEVEL logLevel = LOG_DEBUG;
	WCHAR saveDirectory[MAX_PATH] = L"";
	static CLog log;
	FILE* fp = nullptr;
	SRWLOCK fileLocker;
};


//#define LOG(type,level,format,...) pLog->Log(type,level,format,__VA_ARGS__) 

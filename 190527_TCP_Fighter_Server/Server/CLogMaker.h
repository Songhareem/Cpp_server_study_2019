//
// 현재 파일을 생성할때마다 초기화 됨
//

#pragma once

#ifndef __LOG_MAKER__
#define __LOG_MAKER__

#include <cstdio>
#include <Windows.h>
#include <time.h>
#include <stdarg.h>

#define dfLOG_LEVEL_DEBUG		0
#define dfLOG_LEVEL_WARNING		1
#define dfLOG_LEVEL_ERROR		2
#define dfLOG_LEVEL_SYSTEM		3

//#define _Log(LogLevel, fmt, ...)					\
//do {												\
//	if(_iLogLevel <= LogLevel) {					\
//													\
//		wsprintf(_szTransBuff, fmt, __VA_ARGS__);	\
//	}												\
//													\
//}while(0)											\

class CLogMaker
{
	enum eLog{

		eMAX_TEXT		= 10,
		eMAX_TIME		= 50,
		eMAX_DATE		= 50,
		eMAX_FILE_NAME	= 200,
		eMAX_TRANS		= 800,
		eMAX_LOG		= 1024,
	};

public:
	~CLogMaker();

	static CLogMaker *GetInstance() {

		static CLogMaker g_Log;
		return &g_Log;
	}

	// 파일 생성
	bool MakeLogFile(const char *szFileName);

	// 로그 남기기
	bool Log(int LogLevel, const char *ftm, ...);

private:
	CLogMaker();
	
private:
	int _iLogLevel = dfLOG_LEVEL_DEBUG;
	WCHAR _szTransBuff[eMAX_TRANS];
	char _szLogBuff[eMAX_LOG];
	char _szFileName[eMAX_FILE_NAME];
};


#endif
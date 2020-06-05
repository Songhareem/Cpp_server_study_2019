#include "pch.h"
#include "CLogMaker.h"

CLogMaker::CLogMaker()
{
}


CLogMaker::~CLogMaker()
{
}

bool CLogMaker::MakeLogFile(const char *szFileName) {

	time_t CurrTime;
	struct tm Time;
	CurrTime = time(NULL);
	localtime_s(&Time ,&CurrTime);

	char Date[eMAX_DATE];

	sprintf_s(Date, "%02d_%02d_%02d.txt", Time.tm_year - 100, Time.tm_mon + 1, Time.tm_mday);
	sprintf_s(_szFileName, "%s_%s",szFileName, Date);

	FILE *fp = NULL;
	fopen_s(&fp, _szFileName, "w");
	if(fp == NULL) {
		//printf("Fail_LogFileMake\n");
		return false;
	}
	fclose(fp);

	return true;
}

bool CLogMaker::Log(int LogLevel, const char *fmt, ...) {

	if (_iLogLevel > LogLevel)
		return false;

	if (fmt == NULL)
		return false;

	time_t CurrTime;
	struct tm Time;
	CurrTime = time(NULL);
	localtime_s(&Time, &CurrTime);

	char TimeSet[eMAX_TIME];
	sprintf_s(TimeSet, eMAX_TIME, "%02d:%02d:%02d", Time.tm_hour, Time.tm_min, Time.tm_sec);

	char LogContent[eMAX_TRANS];

	va_list argptr;
	va_start(argptr, fmt);
	vsnprintf_s(LogContent, eMAX_TRANS, fmt, argptr);
	va_end(argptr);

	int LogLen = 0;
	LogLen = sprintf_s(_szLogBuff, "%s [ %s ]\n", LogContent, TimeSet);

	FILE *fp = NULL;
	fopen_s(&fp, _szFileName, "a");
	if (fp == NULL) {
		//printf("Fail_LogFileOpen\n");
		return false;
	}

	fwrite(_szLogBuff, LogLen, 1, fp);
	fclose(fp);

	return true;
}
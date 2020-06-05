
// Log 남기는 기능 만들기
// text 파일명은 "Log_년월일"
// text 내부에는 ( 시간:분:초 Context ) 남김
// 편한 사용을 위해 싱글톤으로 제작

#include "stdafx.h"
#include "CLogMaker.h"
#include <iostream>
#include <time.h>

CLogMaker::CLogMaker()
{
	sprintf_s(_LogName, "%s", "Log_");
}


CLogMaker::~CLogMaker()
{
}

char *CLogMaker::MakeErrContext(char *ErrContext, int ErrCode) {

	memset(_ErrLog, 0, sizeof(_ErrLog));
	
	if(ErrCode == NULL)
		sprintf_s(_ErrLog, "%s", ErrContext);
	else 
		sprintf_s(_ErrLog, "%s : ErrCode(%d)", ErrContext, ErrCode);

	return _ErrLog;
}
void CLogMaker::Rename(char *LogName) {

	sprintf_s(_LogName, "%s", LogName);
}

int CLogMaker::WriteLog(char *ErrLog) {

	// ----------------------------------------
	// 오늘 날자 생성 및 버퍼(nameBuf)에 저장
	// ----------------------------------------
	char nameBuf[30] = { 0, };
	char yearBuf[20] = { 0, };
	char monthBuf[10] = { 0, };
	char dayBuf[10] = { 0, };
	char text[] = ".txt";

	time_t now = time(NULL);
	tm today;
	localtime_s(&today, &now);

	sprintf_s(yearBuf, "%d_", today.tm_year + 1900);
	if (today.tm_mon + 1 < 10)
		sprintf_s(monthBuf, "0%d_", today.tm_mon + 1);
	else
		sprintf_s(monthBuf, "%d_", today.tm_mon + 1);
	if (today.tm_mday < 10)
		sprintf_s(dayBuf, "0%d", today.tm_mday);
	else
		sprintf_s(dayBuf, "%d", today.tm_mday);

	sprintf_s(nameBuf, "%s", _LogName);
	strcat_s(nameBuf, yearBuf);
	strcat_s(nameBuf, monthBuf);
	strcat_s(nameBuf, dayBuf);
	strcat_s(nameBuf, text);

	// -------------------------------------------------------
	// File 이름 지정 및 현재시간에 로그(받은 버퍼값) 출력
	// -------------------------------------------------------

	FILE *fp = NULL;
	char LogBuf[MAX_LOG];

	char hourBuf[20] = { 0, };
	char minBuf[10] = { 0, };
	char secBuf[10] = { 0, };

	fopen_s(&fp, nameBuf, "a");

	if (today.tm_hour < 10)
		sprintf_s(hourBuf, "ERROR_TIME : 0%d:", today.tm_hour);
	else
		sprintf_s(hourBuf, "ERROR_TIME : %d:", today.tm_hour);
	if (today.tm_min < 10)
		sprintf_s(minBuf, "0%d:", today.tm_min);
	else
		sprintf_s(minBuf, "%d:", today.tm_min);
	if (today.tm_sec < 10)
		sprintf_s(secBuf, "0%d ]\n", today.tm_sec);
	else
		sprintf_s(secBuf, "%d ] \n", today.tm_sec);

	sprintf_s(LogBuf, "[ ERROR_CONTEXT : %s    |    ", ErrLog);
	strcat_s(LogBuf, hourBuf);
	strcat_s(LogBuf, minBuf);
	strcat_s(LogBuf, secBuf);

	int size;

	for (size = 0; size < MAX_LOG; size++) {

		if (LogBuf[size] == '\n')
			break;
	}

	fwrite(LogBuf, size + 1, 1, fp);

	fclose(fp);

	return 0;
}

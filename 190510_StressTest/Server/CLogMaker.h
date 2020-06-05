#pragma once

#ifndef	__LOGMAKER__
#define __LOGMAKER__

#include <Windows.h>

#define MAX_LOG  300
#define ERR_LOG  60
#define MAX_NAME 30

class CLogMaker
{
public:
	static CLogMaker *GetInstance() { return &Log; };
	int  WriteLog(char *ErrLog);
	void Rename(char *LogName);
	char *MakeErrContext(char *ErrContext, int ErrCode);

private:
	CLogMaker();
	~CLogMaker();

private:
	static CLogMaker Log;
	char _LogName[MAX_NAME];
	char _ErrLog[ERR_LOG];
};

#endif
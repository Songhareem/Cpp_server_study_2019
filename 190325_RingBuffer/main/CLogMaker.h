#pragma once

#ifndef	__LOGMAKER__
#define __LOGMAKER__

#include <Windows.h>

class CLogMaker
{
public:
	static CLogMaker *GetInstance() { return &Log; };
	int writeLog(char *ErrLog);

private:
	CLogMaker();
	~CLogMaker();

	static CLogMaker Log;
};

#endif
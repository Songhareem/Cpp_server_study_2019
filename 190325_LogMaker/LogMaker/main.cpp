
#include "pch.h"
#include "CLogMaker.h"
#include <iostream>
#include <time.h>
#include <Windows.h>

// ------------------------------
// LogMaker 사용 예시
// ------------------------------

CLogMaker CLogMaker::Log;

int main()
{
	CLogMaker::GetInstance()->writeLog((char *)"ErrFirst");
	Sleep(10000);
	CLogMaker::GetInstance()->writeLog((char *)"ErrSecond");
	Sleep(10000);
	CLogMaker::GetInstance()->writeLog((char *)"ErrThird");

	return 0;
}

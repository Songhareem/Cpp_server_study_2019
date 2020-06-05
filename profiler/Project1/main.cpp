
#include <iostream>
#include <Windows.h>
#include "CProfile.h"

#ifndef CPROFILE_H
#define PRO_BEGIN(TagName) 
#define PRO_END(TagName)
#endif

using namespace std;

int main() {
	
	while (1) {
	
		PRO_BEGIN(L"Test");
		//ProfileBegin((WCHAR *)L"Test");
		for (int i = 0; i < 10; i++) {
			PRO_BEGIN(L"Test2");
			//ProfileBegin((WCHAR *)L"Test2");
			for (int j = 0; j < 10; j++) {
				Sleep(100);
			}
			//ProfileEnd((WCHAR *)L"Test2");
			PRO_END(L"Test2");
		}
		//ProfileEnd((WCHAR *)L"Test");
		PRO_END(L"Test");
	}

	return 0;
}

/*
	FILE *file;
	_wfopen_s(&file,L"test.txt", L"wb");
	WORD mark = 0xFEFF;
	fwrite(&mark, sizeof(WORD), 1, file);
	fwprintf(file, L"123 가나다\r\n");
	fwprintf(file, L"라마바 456");
	fclose(file);
*/
/*
__int64 frequency;
	__int64 BeginTime;
	__int64 EndTime;

	QueryPerformanceFrequency((LARGE_INTEGER *)&frequency);

	cout << frequency << endl;

	//
	QueryPerformanceCounter((LARGE_INTEGER *)&BeginTime);
	Sleep(1000);
	QueryPerformanceCounter((LARGE_INTEGER *)&EndTime);
	//

	cout << BeginTime << " " << EndTime << endl;

	__int64 elapsed = EndTime - BeginTime;
	double duringtime = (double)elapsed / (double)frequency;

	cout << duringtime << endl;
*/
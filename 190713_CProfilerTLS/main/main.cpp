
#include "CProfilerTLS.h"
#include "Profiler.h"
#include <iostream>
#include <process.h>

using namespace std;

bool bServerRun = true;
CProfilerTLS ProfileTLS;

void func1();
void func2();
void func3();

unsigned int __stdcall Thread1(void *arg) {

	printf("Start Thread[%d]\n", GetCurrentThreadId());

	while (bServerRun) {

		ProfileTLS.ProfileBegin("func1");
		func1();
		ProfileTLS.ProfileEnd("func1");

		ProfileTLS.ProfileBegin("func2");
		func2();
		ProfileTLS.ProfileEnd("func2");

		ProfileTLS.ProfileBegin("func3");
		func3();
		ProfileTLS.ProfileEnd("func3");

		Sleep(1);
	}

	printf("End Thread[%d]\n", GetCurrentThreadId());

	return 0;
}

unsigned int __stdcall Thread2(void *arg) {

	printf("Start Thread[%d]\n", GetCurrentThreadId());

	while (bServerRun) {

		ProfileTLS.ProfileBegin("func1");
		func1();
		ProfileTLS.ProfileEnd("func1");

		ProfileTLS.ProfileBegin("func2");
		func2();
		ProfileTLS.ProfileEnd("func2");

		ProfileTLS.ProfileBegin("func3");
		func3();
		ProfileTLS.ProfileEnd("func3");

		Sleep(1);
	}

	printf("End Thread[%d]\n", GetCurrentThreadId());

	return 0;
}

int main() {

	HANDLE hThread[4];
	DWORD dwID = 0;
	hThread[0] = (HANDLE)_beginthreadex(NULL, 0, Thread1, NULL, 1, (unsigned int *)&dwID);
	hThread[1] = (HANDLE)_beginthreadex(NULL, 0, Thread1, NULL, 1, (unsigned int *)&dwID);
	hThread[2] = (HANDLE)_beginthreadex(NULL, 0, Thread2, NULL, 1, (unsigned int *)&dwID);
	hThread[3] = (HANDLE)_beginthreadex(NULL, 0, Thread2, NULL, 1, (unsigned int *)&dwID);

	while (1) {

		// save
		if(GetAsyncKeyState(VK_CONTROL) & 0x0001) {

			ProfileTLS.ProfileDataOutText("TLSfile.txt");
		}

		// End
		if (GetAsyncKeyState(VK_F2)) {
			bServerRun = false;
			Sleep(5000);
			break;
		}
	}

	return 0;
}

void func1()
{
	int a, b, c;
	for (int i = 0; i < 1000; i++)
	{
		a = i;
		b = a;
		c = b;
	}
}
void func2()
{
	int a, b, c;
	for (int i = 0; i < 10000; i++)
	{
		a = i;
		b = a;
		c = b;
	}
}
void func3()
{
	int a, b, c;
	for (int i = 0; i < 100000; i++)
	{
		a = i;
		b = a;
		c = b;
	}
}
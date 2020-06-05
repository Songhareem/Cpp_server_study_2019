#include <iostream>
#include "CLog.h"


using namespace std;

CLog CLog::log;
int main()
{
	CLog* pLog = CLog::GetInstance();
	pLog->SetLogLevel(LOG_DEBUG);
	pLog->SetDirectory(L"log\\");
	int n = 10;
	//for(int i=0;i<4;i++)
	//	pLog->Log(L"ITEM",(en_LOG_LEVEL)i,L"a는 %d입니다",n);
	//LOG(L"ITEM", LOG_DEBUG, L"a는 %d입니다\n", a);
	

	BYTE byte[10] = { 1,2,3,4,5,6,7,8,9,10 };
	pLog->LogHex(L"ITEM", en_LOG_LEVEL::LOG_DEBUG, L"DATA Content",byte, 10);

	//WCHAR tmp[10] = { 0, };
	//wstring wstr;

	//for (int i = 0; i < 10; i++)
	//	printf("0x%02x ",byte[i]);
	//printf("\n");
	//for (int i = 0; i < 10; i++) {
	//	//wsprintfW(tmp, L"0x%02x ", byte[i]);
	//	StringCchPrintfW(tmp, 10, L"0x%02x ", byte[i]);
	//	wstr += tmp;
	//}
	////wprintf(L"%s", tmp);

	//wcout << wstr << endl;

	return 0;

}
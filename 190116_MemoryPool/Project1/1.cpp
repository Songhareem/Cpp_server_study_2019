
#include <iostream>
#include <cstring>
#include "CMyNew.h"
//#include <ctime>

using namespace std;

//*
class TEST {

public:
	
	unsigned int x;

	TEST(){
		x = 1;
	}
	~TEST()
	{
		//cout << "save" << endl;
	}
};
//*/

int main() {
	
	{
		//TEST *test = new TEST[3];
		
		//delete test;

		TEST *test2 = new TEST;

		delete[] test2;
		
	}

	return 0;
}
/*
	char fileName[] = "test.txt";
	FILE *logFile;
	errno_t errCheck;
	char tmp[50];

	errCheck = fopen_s(&logFile, fileName, "a");
	if (errCheck) {
		printf("%s", "file open error\n");
	}
	sprintf_s(tmp, "ARRAY   [0x%08x] [%08d byte] %s : %dline\n", info[i].ptr, info[i].size, info[i].file, info[i].line);
	fwrite(tmp, sizeof(tmp), 1, logFile);
	fclose(logFile);
*/
/*
test *p = new test[20];
test *p2 = p;

p2->x = 1;
p2++;
p2->x = 2;

cout << endl;

delete[] p;

cout << endl;
*/

/* tm test_code
	
	char fileName[50];
	int len;

	time_t base_time = time(NULL);
	struct tm base_date_local;

	localtime_s(&base_date_local, &base_time);

	sprintf_s(fileName, "Alloc_%d%d%d_%d%d%d.txt", (base_date_local.tm_year - 100),
													(base_date_local.tm_mon + 1),
													base_date_local.tm_yday,
													base_date_local.tm_hour,
													base_date_local.tm_min,
													base_date_local.tm_sec);

	cout << base_date_local.tm_year + 1900 << endl;
	cout << base_date_local.tm_mon + 1 << endl;
	cout << base_date_local.tm_mday << endl;
	cout << base_date_local.tm_hour << endl;
	cout << base_date_local.tm_min << endl;
	cout << base_date_local.tm_sec << endl;
*/
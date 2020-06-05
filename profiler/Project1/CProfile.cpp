
#include <iostream>
#include <conio.h>
#include <Windows.h>

using namespace std;

// Ref
// http://egloos.zum.com/sweeper/v/2996847
// http://egloos.zum.com/NeoTrinity/v/943773
// https://arisri.tistory.com/entry/CC-unicode-%EB%AC%B8%EC%9E%90%EC%97%B4-%EC%B2%98%EB%A6%AC-%ED%95%A8%EC%88%98 �����ڵ� ����Լ�
// https://six605.tistory.com/309 �����ڵ� �ؽ�Ʈ���� ���

#define MAX_LIST 100

typedef struct
{
	WCHAR			szName[64];			// �������� ���� �̸�.
	bool            iFlag;              // Begin : true, End : false, Default : false;
	
	__int64			iTotalTime;			// ��ü ���ð� ī���� Time.	(��½� ȣ��ȸ���� ������ ��� ����)
	__int64			iMin[2];			// �ּ� ���ð� ī���� Time.	(�ʴ����� ����Ͽ� ���� / [0] �����ּ� [1] ���� �ּ� [2])
	__int64			iMax[2];			// �ִ� ���ð� ī���� Time.	(�ʴ����� ����Ͽ� ���� / [0] �����ִ� [1] ���� �ִ� [2])
	__int64			iStartTime;			// �������� ���� ���� �ð�.
	__int64			iCall;				// ���� ȣ�� Ƚ��.

} PROFILE_SAMPLE;

PROFILE_SAMPLE profile[MAX_LIST];
int g_fileCnt = 0;

__int64 g_frequency = 0;

void saveProfile(void);
void ProfileReset(void);

void ProfileBegin(WCHAR *szName) {
	
	//cout << "Begin" << endl;

	setlocale(LC_ALL, "korean");
	
	// �ʱ� 1ȸ ����
	if (g_frequency == 0) QueryPerformanceFrequency((LARGE_INTEGER *)&g_frequency);

	// data ����
	bool data = false;
	int i;

	// szName �� �´� ������ ã��
	for (i = 0; i < g_fileCnt; i++) {

		// 0 �̸� ���� ����
		if (wcscmp(szName, profile[i].szName) == 0) {
			data = true;
			break;
		}
	}

	switch (data) {
	
	case true:
		if (profile[i].iFlag == true) {
			wprintf(L"%s, BEGIN * 2\n", profile[i].szName);
			return;
		}
		else if(profile[i].iFlag == false) {
			QueryPerformanceCounter((LARGE_INTEGER *)&profile[i].iStartTime);
			profile[i].iFlag = true;
		}
		break;

	case false:
		// make szName
		wsprintf(profile[g_fileCnt].szName, szName);

		// init profile data
		profile[g_fileCnt].iTotalTime = 0;
		for (int i = 0; i < 2; i++) profile[g_fileCnt].iMin[i] = 0x7fffffffffffffff;
		for (int i = 0; i < 2; i++) profile[g_fileCnt].iMax[i] = 0;
		QueryPerformanceCounter((LARGE_INTEGER *)&profile[i].iStartTime);
		profile[g_fileCnt].iCall = 0;
		profile[g_fileCnt].iFlag = true;

		g_fileCnt++;
		break;
	} 
}

void ProfileEnd(WCHAR *szName) {

	//cout << "End" << endl;

	bool data = false;
	int i;

	// szName �� �´� ������ ã��
	for (i = 0; i < g_fileCnt; i++) {

		// 0 �̸� ���� ����
		if (wcscmp(szName, profile[i].szName) == 0) {
			data = true;
			break;
		}
	}

	switch (data) {
	
	case true:
		if (profile[i].iFlag == false && profile[i].iStartTime == 0) {
			wprintf(L"%s, End * 2\n", profile[i].szName);
			return;
		}
		else if(profile[i].iFlag == true) {
			// �ð� üũ, totalTime �ݿ�
			__int64 iEndTime;
			__int64 runTime;
			QueryPerformanceCounter((LARGE_INTEGER *)&iEndTime);
			runTime = iEndTime - profile[i].iStartTime;
			profile[i].iTotalTime += runTime;
			
			// MIN / MAX ��� - �� �۰ų� �� ū���� 1�� �̷�
			__int64 *Min0 = &profile[i].iMin[0];
			__int64 *Min1 = &profile[i].iMin[1];
			__int64 *Max0 = &profile[i].iMax[0];
			__int64 *Max1 = &profile[i].iMax[1];

			if (*Min0 > runTime) {
				*Min0 = runTime;
				if (*Min0 < *Min1) std::swap(*Min0, *Min1);
			}

			if (*Max0 < runTime) {
				*Max0 = runTime;
				if (*Max0 > *Max1) std::swap(*Max0, *Max1);
			}

			profile[i].iCall++;

			// startTime = 0 ���� �ʱ�ȭ
			profile[i].iStartTime = 0;
			profile[i].iFlag = false;

			// ���� ���
			__int64 minCall = 0x7fffffffffffffff;
			for (int j = 0; j < g_fileCnt; j++) {

				if (minCall > profile[j].iCall) minCall = profile[j].iCall;
			}
			if (profile[i].iCall == minCall) {
				ProfileReset();
				saveProfile();
			}

		}
		break;

	case false:
		wprintf(L"USE ProfileBegin(%s) first\n",szName);
		break;
	}
}

void ProfileDataOutText(WCHAR *szFileName) { 

	FILE *fp;
	
	double avr;
	double min;
	double max;
	__int64 sumMin;
	__int64 sumMax;

	// init output txt
	setlocale(LC_ALL, "korean");

	_wfopen_s(&fp, szFileName, L"wb");
	if (fp == NULL) {
		printf("fopen error\n");
		return;
	}
	WORD mark = 0xFEFF;
	fwrite(&mark, sizeof(WORD), 1, fp);

	// output data
	fwprintf(fp, L"---------------------------------------------------------------------------------\r\n");
	fwprintf(fp, L"|          Name  |       Average  |          Min   |          Max   |      Call |\r\n");
	fwprintf(fp, L"---------------------------------------------------------------------------------\r\n");

	for (int i = 0; i < g_fileCnt; i++) {
		fwprintf(fp, L"%16s |", profile[i].szName);
		if (profile[i].iCall <= 100) {
			
			min = (double)profile[i].iMin[1] / (double)g_frequency;
			max = (double)profile[i].iMax[1] / (double)g_frequency;

			avr = ((double)profile[i].iTotalTime / (double)g_frequency) / profile[i].iCall;
			fwprintf(fp, L"%11.4lf ��s |", avr);
			fwprintf(fp, L"%11.4lf ��s |", min);
			fwprintf(fp, L"%11.4lf ��s |", max);
			fwprintf(fp, L"%11d ", (int)profile[i].iCall);
		}
		else if (profile[i].iCall > 100) {
			
			min = (double)profile[i].iMin[1] / (double)g_frequency;
			max = (double)profile[i].iMax[1] / (double)g_frequency;
			sumMin = profile[i].iMin[0] + profile[i].iMin[1];
			sumMax = profile[i].iMax[0] + profile[i].iMax[1];
			profile[i].iTotalTime -= sumMin + sumMax;
			
			avr = ((double)profile[i].iTotalTime / (double)g_frequency) / profile[i].iCall;
			fwprintf(fp, L"%11.4lf ��s |", avr);
			fwprintf(fp, L"%11.4lf ��s |", min);
			fwprintf(fp, L"%11.4lf ��s |", max);
			fwprintf(fp, L"%11d ", (int)profile[i].iCall-4);
		}
		fwprintf(fp, L"\r\n");
	}
	fwprintf(fp, L"---------------------------------------------------------------------------------\r\n");
	fclose(fp);
}

// ���� ������ ��� END�� �����ؾ��ҵ�?
void saveProfile() {

	static int i = 0;
	cout << i << " ";

	if (GetAsyncKeyState(VK_SPACE) & 0x8001) {

		ProfileDataOutText((WCHAR *)L"Profile.txt");
		cout << "out txt" << endl;
	}
	else cout << "not cnt" << endl;

	return;
}

void ProfileReset(void) {
	
	if (GetAsyncKeyState(VK_SHIFT) & 0x8001) {

		for (int i = 0; i < g_fileCnt; i++) {

			profile[i].iFlag = false;
			profile[i].iTotalTime = 0;
			profile[i].iMin[0] = profile[i].iMin[1] = 0x7fffffffffffffff;
			profile[i].iMax[0] = profile[i].iMax[1] = 0;
			profile[i].iCall = 0;
		}
	}
}

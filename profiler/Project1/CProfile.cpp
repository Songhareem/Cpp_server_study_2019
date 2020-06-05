
#include <iostream>
#include <conio.h>
#include <Windows.h>

using namespace std;

// Ref
// http://egloos.zum.com/sweeper/v/2996847
// http://egloos.zum.com/NeoTrinity/v/943773
// https://arisri.tistory.com/entry/CC-unicode-%EB%AC%B8%EC%9E%90%EC%97%B4-%EC%B2%98%EB%A6%AC-%ED%95%A8%EC%88%98 유니코드 사용함수
// https://six605.tistory.com/309 유니코드 텍스트파일 출력

#define MAX_LIST 100

typedef struct
{
	WCHAR			szName[64];			// 프로파일 샘플 이름.
	bool            iFlag;              // Begin : true, End : false, Default : false;
	
	__int64			iTotalTime;			// 전체 사용시간 카운터 Time.	(출력시 호출회수로 나누어 평균 구함)
	__int64			iMin[2];			// 최소 사용시간 카운터 Time.	(초단위로 계산하여 저장 / [0] 가장최소 [1] 다음 최소 [2])
	__int64			iMax[2];			// 최대 사용시간 카운터 Time.	(초단위로 계산하여 저장 / [0] 가장최대 [1] 다음 최대 [2])
	__int64			iStartTime;			// 프로파일 샘플 실행 시간.
	__int64			iCall;				// 누적 호출 횟수.

} PROFILE_SAMPLE;

PROFILE_SAMPLE profile[MAX_LIST];
int g_fileCnt = 0;

__int64 g_frequency = 0;

void saveProfile(void);
void ProfileReset(void);

void ProfileBegin(WCHAR *szName) {
	
	//cout << "Begin" << endl;

	setlocale(LC_ALL, "korean");
	
	// 초기 1회 실행
	if (g_frequency == 0) QueryPerformanceFrequency((LARGE_INTEGER *)&g_frequency);

	// data 유무
	bool data = false;
	int i;

	// szName 과 맞는 데이터 찾기
	for (i = 0; i < g_fileCnt; i++) {

		// 0 이면 같은 문자
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

	// szName 과 맞는 데이터 찾기
	for (i = 0; i < g_fileCnt; i++) {

		// 0 이면 같은 문자
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
			// 시간 체크, totalTime 반영
			__int64 iEndTime;
			__int64 runTime;
			QueryPerformanceCounter((LARGE_INTEGER *)&iEndTime);
			runTime = iEndTime - profile[i].iStartTime;
			profile[i].iTotalTime += runTime;
			
			// MIN / MAX 계산 - 더 작거나 더 큰값은 1로 미룸
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

			// startTime = 0 으로 초기화
			profile[i].iStartTime = 0;
			profile[i].iFlag = false;

			// 파일 출력
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
			fwprintf(fp, L"%11.4lf μs |", avr);
			fwprintf(fp, L"%11.4lf μs |", min);
			fwprintf(fp, L"%11.4lf μs |", max);
			fwprintf(fp, L"%11d ", (int)profile[i].iCall);
		}
		else if (profile[i].iCall > 100) {
			
			min = (double)profile[i].iMin[1] / (double)g_frequency;
			max = (double)profile[i].iMax[1] / (double)g_frequency;
			sumMin = profile[i].iMin[0] + profile[i].iMin[1];
			sumMax = profile[i].iMax[0] + profile[i].iMax[1];
			profile[i].iTotalTime -= sumMin + sumMax;
			
			avr = ((double)profile[i].iTotalTime / (double)g_frequency) / profile[i].iCall;
			fwprintf(fp, L"%11.4lf μs |", avr);
			fwprintf(fp, L"%11.4lf μs |", min);
			fwprintf(fp, L"%11.4lf μs |", max);
			fwprintf(fp, L"%11d ", (int)profile[i].iCall-4);
		}
		fwprintf(fp, L"\r\n");
	}
	fwprintf(fp, L"---------------------------------------------------------------------------------\r\n");
	fclose(fp);
}

// 가장 마더인 블록 END시 실행해야할듯?
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

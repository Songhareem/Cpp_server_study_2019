
#include "Profiler.h"

//Profiler profiler;

Profiler::Profiler()
{
	QueryPerformanceFrequency(&freq);
}

Profiler::~Profiler()
{
}

void Profiler::ProfileBegin(const char * tag)
{
	SAMPLE* pSample;

	if (dwIndex >= dfMAX_SAMPLE) {
		
		//printf("Tag Full\n");
		return;
	}

	for (DWORD dw = 0; dw < dwIndex; dw++) {

		pSample = &ProfileSample[dw];
		if (strcmp(pSample->tagName, tag) == 0)
		{
			QueryPerformanceCounter(&pSample->startTime);
			return;
		}
	}

	pSample = &ProfileSample[dwIndex++];
	memset(pSample->tagName, 0, 64);
	memcpy(pSample->tagName, tag, 64);
	pSample->callCount = 0;
	pSample->totalTime = 0;
	pSample->minTime[0] = 0;
	pSample->minTime[1] = 0;
	pSample->maxTime[0] = 0;
	pSample->maxTime[1] = 0;
	
	QueryPerformanceCounter(&pSample->startTime);
}

void Profiler::ProfileEnd(const char * tag)
{
	SAMPLE* pSample;
	LARGE_INTEGER end;
	QueryPerformanceCounter(&end);
	__int64 delay;

	for (DWORD dw=0; dw<dwIndex; dw++)
	{
		pSample = &ProfileSample[dw];
		if (strcmp(pSample->tagName,tag)==0)
		{
			delay = (end.QuadPart - pSample->startTime.QuadPart) / (freq.QuadPart / 1000000);
			if (delay < 0)
			{
				return;
			}
			pSample->totalTime += delay;
			pSample->callCount++;
			
			if (delay < pSample->minTime[0] && delay > 0 || pSample->minTime[0] == 0)
			{
				pSample->minTime[0] = delay;
				return;
			}
			if ( delay < pSample->minTime[1] && delay > 0 || pSample->minTime[0] == 0)
			{
				pSample->minTime[1] = delay;
				return;
			}
			if (delay > pSample->maxTime[0])
			{
				pSample->maxTime[0] = delay;
				return;
			}
			if (delay > pSample->maxTime[1])
			{
				pSample->maxTime[1] = delay;
				return;
			}
			return;
		}
	}
	//printf("시작된 프로파일러가 없습니다 : %s", tag);
}

void Profiler::ProfileDataOutText(const char * filename)
{
	FILE* fp;
	__int64 total;
	double average;
	double min;
	double max;
	__int64 call;
	fopen_s(&fp, filename, "a");
	if (fp == NULL)
	{
		//파일 오픈 실패
		return;
	}
	fprintf(fp, "--------------------------------------------------------------------------------\n");
	fprintf(fp, "\tName\t|\tAverage\t|\tMin\t|\tMax\t|\tCall\t|\n");
	fprintf(fp, "--------------------------------------------------------------------------------\n");
	
	SAMPLE *pSample;
	for (DWORD dw=0; dw<dwIndex; dw++)
	{
		pSample = &ProfileSample[dw];
		total = pSample->totalTime;
		call = pSample->callCount;
		if (call > 4)
		{
			total -= (pSample->minTime[0] + pSample->minTime[1] + pSample->maxTime[0] + pSample->maxTime[1]);
			call -= 4;
		}
		average = (double)total / (double)call;
		min = (double)(pSample->minTime[0] + pSample->minTime[1]) / 2;
		max = (double)(pSample->maxTime[0] + pSample->maxTime[1]) / 2;

		fprintf(fp, "\t%s | \t%f | \t%f | \t%f | \t%ld\n", pSample->tagName, average, min, max, call);

	}
	fprintf(fp, "--------------------------------------------------------------------------------\n");
	fclose(fp);
}

void Profiler::ProfileDataOutText(const char * filename, DWORD dwThreadID)
{
	FILE* fp;
	__int64 total;
	double average;
	double min;
	double max;
	__int64 call;
	fopen_s(&fp, filename, "a");
	if (fp == NULL)
	{
		//파일 오픈 실패
		return;
	}
	fprintf(fp, "-------------------------------------------------------------------------------------------------\n");
	fprintf(fp, "|\tThreadID\t|\tName\t|\tAverage\t|\tMin\t|\tMax\t|\tCall\t|\n");
	fprintf(fp, "-------------------------------------------------------------------------------------------------\n");

	SAMPLE *pSample;
	for (DWORD dw = 0; dw < dwIndex; dw++)
	{
		pSample = &ProfileSample[dw];
		total = pSample->totalTime;
		call = pSample->callCount;
		if (call > 4)
		{
			total -= (pSample->minTime[0] + pSample->minTime[1] + pSample->maxTime[0] + pSample->maxTime[1]);
			call -= 4;
		}
		average = (double)total / (double)call;
		min = (double)(pSample->minTime[0] + pSample->minTime[1]) / 2;
		max = (double)(pSample->maxTime[0] + pSample->maxTime[1]) / 2;

		fprintf(fp, "\t%d | \t%s | \t%f | \t%f | \t%f | \t%d\n", dwThreadID, pSample->tagName, average, min, max, call);
	}
	fprintf(fp, "-------------------------------------------------------------------------------------------------\n");
	fclose(fp);
}

void Profiler::ProfileReset(void)
{
	dwIndex = 0;
}

//void ProfileBegin(const char * tag)
//{
//	profiler.ProfileBegin(tag);
//	//ProfileBegin(tag);
//}
//
//void ProfileEnd(const char * tag)
//{
//	profiler.ProfileEnd(tag);
//}
//
//void ProfileDataOutText(const char * filename)
//{
//	profiler.ProfileDataOutText(filename);
//}
//
//void ProfileReset(void)
//{
//	profiler.ProfileReset();
//}

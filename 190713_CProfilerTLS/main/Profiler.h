
#ifndef _PROFILER_
#define _PROFILER_

//#define PRO_BEGIN(TagName) ProfileBegin(TagName)
//#define PRO_END(TagName) ProfileEnd(TagName)

#define dfMAX_SAMPLE	50

#include <iostream>
#include <Windows.h>

typedef struct st_SAMPLE
{
	char tagName[64];
	LARGE_INTEGER startTime;
	__int64 totalTime;
	__int64 minTime[2];
	__int64 maxTime[2];
	__int64 callCount;
	st_SAMPLE()
	{
		totalTime = 0;
		minTime[0] = 0;
		minTime[1] = 0;
		maxTime[0] = 0;
		maxTime[1] = 0;
		callCount = 0;

		//printf("asdasdasd\n");
	}

} SAMPLE;

class Profiler
{
public:

	Profiler();
	~Profiler();

	void ProfileBegin(const char *tag);
	void ProfileEnd(const char *tag);
	void ProfileReset(void);
	void ProfileDataOutText(const char *filename);
	void ProfileDataOutText(const char *filename, DWORD dwThreadID);

	SAMPLE ProfileSample[dfMAX_SAMPLE];
	DWORD  dwIndex = 0;
	LARGE_INTEGER freq;

};

//void ProfileBegin(const char *tag);
//void ProfileEnd(const char *tag);
//void ProfileDataOutText(const char *filename);
//void ProfileReset(void);

#endif // !_PROFILER_




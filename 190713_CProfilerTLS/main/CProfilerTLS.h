#pragma once

#include "Profiler.h"
#include <Windows.h>

#define dfMAX_THREAD	50

#define BEGIN(tag)		ProfileBegin(tag)
#define END(tag)		ProfileEnd(tag)

class CProfilerTLS
{
private:
	typedef struct st_THREAD_SAMPLE {

		DWORD		dwThreadID;
		Profiler	Profile;

	} THREAD_SAMPLE;

public:
	CProfilerTLS();
	~CProfilerTLS();

	void ProfileBegin(const char *tag);
	void ProfileEnd(const char *tag);
	void ProfileDataOutText(const char *filename);

private:
	THREAD_SAMPLE	ThreadSample[dfMAX_THREAD];
	DWORD			dwIndex = 0;
	DWORD			tlsIndex = 0;

	SRWLOCK			SrwProfile;
};


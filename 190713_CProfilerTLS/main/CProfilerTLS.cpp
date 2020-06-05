#include "CProfilerTLS.h"


CProfilerTLS::CProfilerTLS()
{
	tlsIndex = TlsAlloc();
	InitializeSRWLock(&SrwProfile);
}

CProfilerTLS::~CProfilerTLS()
{
	TlsFree(tlsIndex);
}

void CProfilerTLS::ProfileBegin(const char * tag)
{
	THREAD_SAMPLE *tlsPtr = (THREAD_SAMPLE *)TlsGetValue(tlsIndex);
	if (tlsPtr == NULL) {

		AcquireSRWLockExclusive(&SrwProfile);
		if (dwIndex >= dfMAX_THREAD) {
			printf("Can't Make More Profiler. if you want, change define 'dfMAX_THREAD' value\n");
			return;
		}
		TlsSetValue(tlsIndex, &ThreadSample[dwIndex++]);
		ReleaseSRWLockExclusive(&SrwProfile);

		tlsPtr = (THREAD_SAMPLE *)TlsGetValue(tlsIndex);
		tlsPtr->dwThreadID = GetCurrentThreadId();
	}

	tlsPtr->Profile.ProfileBegin(tag);
}

void CProfilerTLS::ProfileEnd(const char * tag)
{
	THREAD_SAMPLE *tlsPtr = (THREAD_SAMPLE *)TlsGetValue(tlsIndex);
	if (tlsPtr == NULL) {
		return;
	}

	tlsPtr->Profile.ProfileEnd(tag);
}

void CProfilerTLS::ProfileDataOutText(const char * filename)
{
	static int Cnt = 0;
	printf("%d\n", Cnt++);

	for(DWORD dw=0; dw<dwIndex; dw++) {
		DWORD dwThreadID = ThreadSample[dw].dwThreadID;
		ThreadSample[dw].Profile.ProfileDataOutText(filename, dwThreadID);
	}
}

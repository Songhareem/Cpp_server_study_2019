
#ifndef __SOCKAPI__
#define __SOCKAPI__

#pragma once
class CSocketAPI
{
public:
	CSocketAPI();
	~CSocketAPI();

	BOOL DomainToIP(WCHAR *szDomain, IN_ADDR *pAddr);
};

#endif
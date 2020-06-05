
#include "pch.h"
#include "CSocketAPI.h"

CSocketAPI::CSocketAPI()
{
}


CSocketAPI::~CSocketAPI()
{
}

BOOL CSocketAPI::DomainToIP(WCHAR *szDomain, IN_ADDR *pAddr) {

	ADDRINFOW		*pAddrInfo;
	ADDRINFOW		*pForRelease;
	SOCKADDR_IN		*pSockAddr;

	if (GetAddrInfo(szDomain, L"0", NULL, &pAddrInfo) != 0) {

		return false;
	}

	pForRelease = pAddrInfo;
	int iIndex = 0;
	while (1) {

		pSockAddr = (SOCKADDR_IN *)pAddrInfo->ai_addr;
		pAddr[iIndex] = pSockAddr->sin_addr;

		pAddrInfo = pAddrInfo->ai_next;
		if (pAddrInfo == NULL)
			break;
		iIndex++;
	}

	//pSockAddr = (SOCKADDR_IN *)pAddrInfo->ai_addr;
	//*pAddr = pSockAddr->sin_addr;
	FreeAddrInfo(pForRelease);

	return true;
}
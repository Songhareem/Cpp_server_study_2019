
#include "pch.h"
#include "Network.h"

BOOL DomainToIP(const WCHAR *szDomain, IN_ADDR *pAddr)
{
	ADDRINFOW *pAddrInfo;
	SOCKADDR_IN *pSockAddr;

	if (GetAddrInfo(szDomain, L"0", NULL, &pAddrInfo))
		return FALSE;

	pSockAddr = (SOCKADDR_IN *)pAddrInfo->ai_addr;
	*pAddr = pSockAddr->sin_addr;
	FreeAddrInfo(pAddrInfo);
	return TRUE;
}

void err_display(const WCHAR *msg, BOOL isExit)
{
	int err = WSAGetLastError();
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
	);
	WCHAR title[64];
	if (isExit) {
		swprintf(title, 64, L"%s (%d)", msg, err);
		MessageBox(NULL, (LPCTSTR)lpMsgBuf, title, MB_ICONERROR);
	}
	else
		wprintf(L"[%s] (%d) %s\n", msg, err, (WCHAR *)lpMsgBuf);
	LocalFree(lpMsgBuf);
	if (isExit)	exit(1);
}
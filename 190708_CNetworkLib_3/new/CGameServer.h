#pragma once
#include "CNetworkLib.h"

#define ClientLock(Srw)		AcquireSRWLockExclusive(&Srw)
#define ClientUnlock(Srw)	ReleaseSRWLockExclusive(&Srw)

typedef struct st_CLIENT {

	DWORD		dwSessionID;
	SOCKADDR_IN	clientAddr;

} CLIENT;

class CGameServer :
	public CNetworkLib
{
public:
	CGameServer();
	virtual ~CGameServer();

	virtual int OnRecv(DWORD dwSessionID, CPacket *pPack);
	virtual int OnSend(DWORD dwSessionID);
	virtual int OnClientJoin(DWORD dwSessionID, SOCKADDR_IN clientAddr);
	virtual int OnClientLeave(DWORD dwSessionID);
	virtual int OnError(DWORD dwError);

private:

	// CGameServer Initialize
	int GameInit();

	CLIENT *CreateClient(DWORD dwSessionID, SOCKADDR_IN clientAddr);

	bool   DeleteClient(DWORD dwSessionID);

	CLIENT *FindClient(DWORD dwSessionID);

private:

	// 클라이언트 맵
	map<DWORD, CLIENT *> _ClientMap;

	// 클라이언트 Lock
	SRWLOCK _SrwClient;
};


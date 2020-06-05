#pragma once
#include "CNetworkLib.h"

#define ClientLock(Srw)		AcquireSRWLockExclusive(&Srw)
#define ClientUnlock(Srw)	ReleaseSRWLockExclusive(&Srw)

typedef struct st_CLIENT {

	SESSION_ID	SessionID;
	SOCKADDR_IN	clientAddr;

} CLIENT;

class CGameServer :
	public CNetworkLib
{
public:
	CGameServer();
	virtual ~CGameServer();

	virtual int OnRecv(SESSION_ID SessionID, CPacket *pPack);
	virtual int OnSend(SESSION_ID SessionID);
	virtual int OnClientJoin(SESSION_ID SessionID, SOCKADDR_IN clientAddr);
	virtual int OnClientLeave(SESSION_ID SessionID);
	virtual int OnError(DWORD dwError);

private:

	// CGameServer Initialize
	int GameInit();

	CLIENT *CreateClient(SESSION_ID SessionID, SOCKADDR_IN clientAddr);

	bool   DeleteClient(SESSION_ID SessionID);

	CLIENT *FindClient(SESSION_ID SessionID);

private:

	// 클라이언트 맵
	map<SESSION_ID, CLIENT *> _ClientMap;

	// 클라이언트 Lock
	SRWLOCK _SrwClient;
};


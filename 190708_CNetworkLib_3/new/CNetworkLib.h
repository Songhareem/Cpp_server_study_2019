#pragma once
#pragma comment(lib,"ws2_32")

#include <WS2tcpip.h>
#include <Windows.h>
#include <iostream>
#include <locale.h>
#include <process.h>
#include <map>
#include "CRingBuffer.h"
#include "CPacket.h"

using namespace std;

#define dfSERVER_PORT		6000
#define dfMAX_PACK_SIZE		2000
#define	dfMAX_WORKER		10
#define dfMAX_ACCEPT		1
#define dfMAX_THREAD		(dfMAX_WORKER + dfMAX_ACCEPT)

#define SessionLock(Srw)	AcquireSRWLockExclusive(&Srw)
#define SessionUnlock(Srw)	ReleaseSRWLockExclusive(&Srw)

typedef struct st_SESSION {

	DWORD		dwID = 0;
	SOCKET		socket = INVALID_SOCKET;
	SOCKADDR_IN sockAddr;
	OVERLAPPED	sendOverlap;
	OVERLAPPED	recvOverlap;
	CRingBuffer SendQ;
	CRingBuffer RecvQ;
	int			IOCount = 0;
	long		lSendFlag = false;
	DWORD		dwSendByte = 0;

} SESSION;

typedef struct st_HEADER {

	short shLen;

} HEADER;

class CNetworkLib
{
	
public:
	CNetworkLib();
	virtual ~CNetworkLib();

	//GetNetMaxThread
	DWORD GetNetMaxThread();

	//GetNetThreadHandle
	HANDLE *GetThreadHandle();

	// SendPacket
	int SendPacket(DWORD dwSessionID, CPacket *pPack);

	// Disconnect
	int Disconnect(DWORD dwSessionID);

private:
	//AcceptThread
	static unsigned int __stdcall AcceptThread(void *arg);
	
	//WorkerThread
	static unsigned int __stdcall WorkerThread(void *arg);

	//CNet Initialize
	int NetInit();
	
	// RecvPost
	int RecvPost(SESSION *pSession);

	// SendPost
	int SendPost(SESSION *pSession);

	// ReleaseSession
	bool ReleaseSession(SESSION *pSession);

	// FindSession
	SESSION *FindSession(DWORD dwSessionID);

protected:

	// 순수 가상함수
	virtual int OnRecv(DWORD dwSessionID, CPacket *pPack) = 0;
	virtual int OnSend(DWORD dwSessionID) = 0;
	virtual int OnClientJoin(DWORD dwSessionID, SOCKADDR_IN ClientAddr) = 0;
	virtual int OnClientLeave(DWORD dwSessionID) = 0;
	virtual int OnError(DWORD dwError) = 0;				

private:
	//listenSock
	SOCKET _listenSock = INVALID_SOCKET;

	//ServerAddr
	SOCKADDR_IN _ServerAddr;
	
	//SessionID
	DWORD _dwSessionID = 0;

	//SessionMap
	map<DWORD, SESSION *>_SessionMap;

	//Session srw
	SRWLOCK	_SrwSession;

	// Completion Port
	HANDLE _hNetCP = NULL;

	//ThreadHandle
	HANDLE _hNetThread[dfMAX_THREAD] = { NULL, };
};

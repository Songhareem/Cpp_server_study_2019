#pragma once
#pragma comment(lib,"ws2_32")

#include <WS2tcpip.h>
#include <Windows.h>
#include <iostream>
#include <map>
#include "CRingBuffer.h"
#include "CPacket.h"

using namespace std;

#define dfSERVER_PORT		6000
#define dfMAX_PACK_SIZE		2000
#define	dfMAX_WORKER		8
#define dfMAX_ACCEPT		1
#define dfMAX_THREAD		(dfMAX_WORKER + dfMAX_ACCEPT)

#define SessionLock(Srw)	AcquireSRWLockExclusive(&Srw)
#define SessionUnlock(Srw)	ReleaseSRWLockExclusive(&Srw)

typedef unsigned __int64 SESSION_ID;

typedef short HEADER;

typedef struct st_SESSION {

	SESSION_ID	SessionID;
	SOCKET		socket;
	SOCKADDR_IN sockAddr;
	OVERLAPPED	sendOverlap;
	OVERLAPPED	recvOverlap;
	CRingBuffer SendQ;
	CRingBuffer RecvQ;
	int			IOCount = 0;
	long		lSendFlag = false;
	DWORD		dwSendByte = 0;

} SESSION;

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
	int SendPacket(SESSION_ID SessionID, CPacket *pPack);

	// Disconnect
	int Disconnect(SESSION_ID SessionID);

private:
	//AcceptThread
	static unsigned int __stdcall AcceptThread(void *arg);
	
	//WorkerThread
	static unsigned int __stdcall WorkerThread(void *arg);

	//CNet Initialize
	int NetInit();
	
	// Make Crash
	void Crash();

	// RecvPost
	int RecvPost(SESSION *pSession);

	// SendPost
	int SendPost(SESSION *pSession);

	// CreateSession
	SESSION *CreateSession(SOCKET clientSock, SOCKADDR_IN clientAddr);

	// ReleaseSession
	bool ReleaseSession(SESSION_ID SessionID);

	// ReleaseSession
	bool ReleaseSession(SESSION *pSession);

	// FindSession
	SESSION *FindSession(SESSION_ID SessionID);

protected:

	// 순수 가상함수
	virtual int OnRecv(SESSION_ID SessionID, CPacket *pPack) = 0;
	virtual int OnSend(SESSION_ID SessionID) = 0;
	virtual int OnClientJoin(SESSION_ID SessionID, SOCKADDR_IN ClientAddr) = 0;
	virtual int OnClientLeave(SESSION_ID SessionID) = 0;
	virtual int OnError(DWORD dwError) = 0;				

private:
	//listenSock
	SOCKET _listenSock = INVALID_SOCKET;

	//ServerAddr
	SOCKADDR_IN _ServerAddr;
	
	//SessionID
	SESSION_ID _SessionID = 0;

	//SessionMap
	map<SESSION_ID, SESSION *>_SessionMap;

	//Session srw
	SRWLOCK	_SrwSession;

	// Completion Port
	HANDLE _hNetCP = NULL;

	// AcceptThreadHandle
	HANDLE _hAcceptThread = NULL;

	// WorkerThreadHandle
	HANDLE _hWorkerThread[dfMAX_THREAD] = { NULL, };
};

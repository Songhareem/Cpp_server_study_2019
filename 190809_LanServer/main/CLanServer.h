#pragma once
#pragma comment(lib,"ws2_32")

#include <WS2tcpip.h>
#include <Windows.h>
#include <iostream>
#include <map>
#include "CRingBuffer.h"
#include "CPacket.h"
#include "CLockFreeStack.h"
#include "CLockFreeQ.h"

using namespace std;

#define dfSERVER_PORT		6000
#define	dfMAX_WORKER		8
#define dfMAX_ACCEPT		1
#define dfMAX_THREAD		(dfMAX_WORKER + dfMAX_ACCEPT)
#define dfMAX_SESSION		100
#define dfMAX_PACK_ARR		100

typedef unsigned __int64 SESSION_ID;

typedef short HEADER;

typedef struct st_SESSION {

	SESSION_ID	SessionID;
	SOCKET		socket;
	SOCKADDR_IN sockAddr;
	OVERLAPPED	sendOverlap;
	OVERLAPPED	recvOverlap;
	long		lRefCount = 0;
	long		lSendFlag = false;
	DWORD		dwSendPackNum = 0;
	long		lUsed = false;
	CRingBuffer	RecvQ;
	CLockFreeQ<CPacket *>  SendQ;
	CPacket		*pPackArr[dfMAX_PACK_ARR];

} SESSION;

class CLanServer
{
	
public:
	CLanServer();
	virtual ~CLanServer();

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

	//SessionSize
	long	_lSessionSize = 0;

	// SessionDinamicArray
	SESSION _SessionArr[dfMAX_SESSION];

	// NonUsedSessionIndexStack
	CLockFreeStack<DWORD> _IndexStack;

	// Completion Port
	HANDLE _hNetCP = NULL;

	// AcceptThreadHandle
	HANDLE _hAcceptThread = NULL;

	// WorkerThreadHandle
	HANDLE _hWorkerThread[dfMAX_THREAD] = { NULL, };
};

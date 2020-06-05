
#include "CGameServer.h"
#include <process.h>

CGameServer::CGameServer()
{
	GameInit();
}

CGameServer::~CGameServer()
{
	// ���� Ŭ���̾�Ʈ �ִ��� Ȯ��
	printf("\nClientMap Size : %d\n", (int)_ClientMap.size());
	Sleep(1000);
}

int CGameServer::OnRecv(SESSION_ID SessionID, CPacket * pPack)
{
	// ��� type Ȯ��

	// PacketProc(type,pPack) �۾��� (�Ƹ�?) -> switch(type)

	// init
	CPacket Pack(2000);
	Pack.Clear();

	HEADER header = pPack->GetPacketSize();

	// Payload
	__int64 i64Data = 0;
	*pPack >> i64Data;

	//Pack.Enqueue((char *)&Header, sizeof(HEADER));
	Pack << header;
	Pack << i64Data;

	SendPacket(SessionID, &Pack);
	
	return 0;
}

int CGameServer::OnSend(SESSION_ID SessionID)
{
	//printf("save\n");

	return 0;
}

int CGameServer::OnClientJoin(SESSION_ID SessionID, SOCKADDR_IN ClientAddr)
{
	CreateClient(SessionID, ClientAddr);

	return 0;
}

int CGameServer::OnClientLeave(SESSION_ID SessionID)
{
	DeleteClient(SessionID);

	return 0;
}

int CGameServer::OnError(DWORD dwError)
{
	// NetLib���� GetLastError �� DWORD �� �޾Ƽ� Ŭ��� ����
	
	return 0;
}

int CGameServer::GameInit()
{
	InitializeSRWLock(&_SrwClient);

	return 0;
}

CLIENT* CGameServer::CreateClient(SESSION_ID SessionID, SOCKADDR_IN clientAddr)
{
	CLIENT *pClient = new CLIENT;
	if (pClient == NULL) {
		// Log Error
		printf("Memory Full\n");
	}

	pClient->SessionID = SessionID;
	pClient->clientAddr = clientAddr;

	ClientLock(_SrwClient);
	_ClientMap.insert(make_pair(pClient->SessionID, pClient));
	ClientUnlock(_SrwClient);

	return pClient;
}

bool CGameServer::DeleteClient(SESSION_ID SessionID)
{
	bool bRet = false;
	// Test ���� �ڵ�
	CLIENT *pClient = FindClient(SessionID);
	if (pClient == NULL) {

		printf("Client NULL\n");
		return bRet;
	}

	ClientLock(_SrwClient);
	_ClientMap.erase(SessionID);
	delete pClient;
	ClientUnlock(_SrwClient);

	return bRet;
}

CLIENT * CGameServer::FindClient(SESSION_ID SessionID)
{
	CLIENT *pClient = NULL;

	ClientLock(_SrwClient);
	map<SESSION_ID, CLIENT *>::iterator ClientIDIter = _ClientMap.begin();
	map<SESSION_ID, CLIENT *>::iterator ClientIDIterEnd = _ClientMap.end();

	for (; ClientIDIter != ClientIDIterEnd; ++ClientIDIter) {

		if (SessionID == (*ClientIDIter).first) {

			pClient = (*ClientIDIter).second;
			break;
		}
	}
	ClientUnlock(_SrwClient);
	
	return pClient;
}

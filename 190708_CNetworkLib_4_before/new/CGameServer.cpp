
#include "CGameServer.h"

CGameServer::CGameServer()
{
	GameInit();
}

CGameServer::~CGameServer()
{
	// 남은 클라이언트 있는지 확인
	printf("\nClientMap Size : %d\n", (int)_ClientMap.size());
	Sleep(1000);
}

int CGameServer::OnRecv(DWORD dwSessionID, CPacket * pPack)
{
	SendPacket(dwSessionID, pPack);
	
	return 0;
}

int CGameServer::OnSend(DWORD dwSessionID)
{
	return 0;
}

int CGameServer::OnClientJoin(DWORD dwSessionID, SOCKADDR_IN ClientAddr)
{
	CreateClient(dwSessionID, ClientAddr);

	return 0;
}

int CGameServer::OnClientLeave(DWORD dwSessionID)
{
	DeleteClient(dwSessionID);

	return 0;
}

int CGameServer::OnError(DWORD dwError)
{
	// NetLib에서 GetLastError 로 DWORD 값 받아서 클라로 전달
	
	return 0;
}

int CGameServer::GameInit()
{
	InitializeSRWLock(&_SrwClient);

	return 0;
}

CLIENT* CGameServer::CreateClient(DWORD dwSessionID, SOCKADDR_IN clientAddr)
{
	CLIENT *pClient = new CLIENT;
	if (pClient == NULL) {
		// Log Error
		printf("Memory Full\n");
	}

	pClient->dwSessionID = dwSessionID;
	pClient->clientAddr = clientAddr;

	ClientLock(_SrwClient);
	_ClientMap.insert(make_pair(pClient->dwSessionID, pClient));
	ClientUnlock(_SrwClient);

	return pClient;
}

bool CGameServer::DeleteClient(DWORD dwSessionID)
{
	bool bRet = false;

	ClientLock(_SrwClient);
	map<DWORD, CLIENT *>::iterator ClientIDIter = _ClientMap.begin();
	map<DWORD, CLIENT *>::iterator ClientIDIterEnd = _ClientMap.end();

	for (; ClientIDIter != ClientIDIterEnd; ++ClientIDIter) {

		if (dwSessionID == (*ClientIDIter).first) {

			_ClientMap.erase(ClientIDIter);
			bRet = true;
			break;
		}
	}
	ClientUnlock(_SrwClient);

	return bRet;
}

CLIENT * CGameServer::FindClient(DWORD dwSessionID)
{
	ClientLock(_SrwClient);
	map<DWORD, CLIENT *>::iterator ClientIDIter = _ClientMap.begin();
	map<DWORD, CLIENT *>::iterator ClientIDIterEnd = _ClientMap.end();

	for (; ClientIDIter != ClientIDIterEnd; ++ClientIDIter) {

		if (dwSessionID == (*ClientIDIter).first) {

			return (*ClientIDIter).second;
		}
	}
	ClientUnlock(_SrwClient);
	
	return NULL;
}

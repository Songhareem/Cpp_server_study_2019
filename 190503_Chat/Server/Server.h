// 
// select ����?
// select (maxfdpl, ReadSet, WriteSet, ExceptSet, st_timeval)
// 
// select return ��?
// 1) ���� �� ��ȯ �� : �غ�� fd ����
// 2) �ð� ����� ��ȯ �� : 0
// 3) ���� �� ��ȯ �� : -1
// 
// select ù��° ���� maxfdpl?
// maxfdpl : 
// 
// ExceptSet : ���� �߻� �ߴ��� Ȯ��
//
// timeout �� NULL �� �� : ������ ��ٸ���. FD �� �ϳ��� �غ�ǰų� ��ȣ�� ���� ����� ���ܵȴ�.
// timeout �� 0 �� �� : ���� ��ٸ��� �ʴ´�.���� ���� FD ���¸� Ȯ���� ��� ���δ�.
// timeout �� 0 �� �ƴ� �� : ������ sec �� usec ��ŭ ��ٸ���. FD �� �ϳ��� �غ�ǰų� �ð��� ����Ǹ� ��ȯ�ȴ�.
//
//
// string ��ü�� ���Ͽ�
// string ��ü �ʱ�ȭ
// 1) �� ���� �ʱ�ȭ, string str;
// 2) ��� ������ �ʱ�ȭ, string str = "asdasd";
//
// string to char, char to string
// 1)  strcpy_s(ch, str.size()+1, str.c_str()); 
//
// 2) string str; char ch[10];
//    str = ch;
//
// string ��ü�� ���� ���� ������
// string[0] �̴�
//
// map�� Ű�� ���� �Ǵ�
// 1) Key�� �����ϴٸ�, Count() ����ؼ� �����Ѵٸ� 1, �������� �ʴ´ٸ� 0 ����
// 2) Key�� ���������ʴٸ�, if(map.find(Key) == map.end()), ã�����ߴٸ� true, ã�Ҵٸ� false ���� 
//
// chat �޽��� ����? Ŭ���̾�Ʈ â �ʺ� ���� ���缭 ��
//
// Send ���� �Լ��� ���� �� ������ �ʾƵ� ��
// Recv ���� �Լ��� ����, return ���缭 �� ( ���� �Լ� ������ ������ ���� ���ؼ� )

#pragma once

#ifndef __SERVER__
#define __SERVER__

#include <Windows.h>
#include <iostream>
#include <string>
#include <list>
#include <map>
#include "Protocol.h"
#include "CRingBuffer.h"
#include "CPacket.h"

using namespace std;

#define dfRECV_BUFF		512

typedef struct st_CLIENT {

	SOCKET		Socket;
	SOCKADDR_IN ConnectAddr;

	CRingBuffer SendQ;
	CRingBuffer RecvQ;

	DWORD		dwUserNo;
	DWORD		dwEnterRoomNo;

	wstring		szNickName;

} CLIENT;

typedef struct st_CHAT_ROOM {

	DWORD		dwRoomNO;
	wstring		szTitle;

	list<DWORD>	UserList;

} CHAT_ROOM;

// ����� ���� map
map<DWORD, st_CLIENT *>		g_ClientMap;

// ä�ù� ���� map
map<DWORD, st_CHAT_ROOM *>	g_RoomMap;

// ����� accept �� ��������
SOCKET						g_ListenSock = INVALID_SOCKET;

// ���� ����Ű, �� ����Ű�� ���� ��������
DWORD						g_dwKey_UserNO = 1;
DWORD						g_dwKey_RoomNO = 1;

// g_wsa
WSADATA g_wsa;

// g_ServerAddr
SOCKADDR_IN g_ServerAddr;


// ��Ʈ��ũ �ʱ�ȭ
bool NetworkInitial() {

	// ���� ����
	if (WSAStartup(MAKEWORD(2, 2), &g_wsa) != 0)
		return false;

	// ListenSocket
	g_ListenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (g_ListenSock == INVALID_SOCKET) {
		wprintf(L"socket()_ERROR\n");
		return false;
	}
	
	// bind()
	ZeroMemory(&g_ServerAddr, sizeof(g_ServerAddr));
	g_ServerAddr.sin_family = AF_INET;
	InetPton(AF_INET, INADDR_ANY, &g_ServerAddr.sin_addr);
	g_ServerAddr.sin_port = htons(dfNETWORK_PORT);
	int retval = bind(g_ListenSock, (SOCKADDR *)&g_ServerAddr, sizeof(g_ServerAddr));
	if (retval == SOCKET_ERROR) {
		wprintf(L"bind()_ERROR\n");
		return false;
	}

	// listen()
	retval = listen(g_ListenSock, SOMAXCONN);
	if (retval == SOCKET_ERROR) {
		wprintf(L"listen()_ERROR\n");
		return false;
	}

	// change nonblock sock
	u_long on = 1;
	retval = ioctlsocket(g_ListenSock, FIONBIO, &on);
	if (retval == SOCKET_ERROR) {
		wprintf(L"ioctlsocket()_ERROR\n");
	}

	wprintf(L"Server Open...\n");

	return true;
}

// ��Ʈ��ũ ����
void NetworkClose() {

	closesocket(g_ListenSock);

	WSACleanup();
}

// Ű �Է� �Լ�
bool KeyProc() {

	// SPACE ��ư
	if (GetKeyState(VK_SPACE) & 8000)
		return true;

	return false;
}

// AddClient
void AddClient(DWORD dwKeyUserNO, st_CLIENT *pNewClient) {

	g_ClientMap.insert(make_pair(dwKeyUserNO, pNewClient));
}

// FindClient
st_CLIENT *FindClient(DWORD dwKeyUserNO) {

	return g_ClientMap.find(dwKeyUserNO)->second;
}

// DelClient
void DelClient(DWORD dwKeyUserNO) {

	g_ClientMap.erase(dwKeyUserNO);
}

// AddRoom
void AddRoom(DWORD dwKeyRoomNO, st_CHAT_ROOM *pNewRoom) {

	g_RoomMap.insert(make_pair(dwKeyRoomNO, pNewRoom));
}

// FindRoom
st_CHAT_ROOM *FindRoom(DWORD dwKeyRoomNO) {

	return g_RoomMap.find(dwKeyRoomNO)->second;
}

// DelClient
void delRoom(DWORD dwKeyRoomNO) {

	g_RoomMap.erase(dwKeyRoomNO);
}

// accept()
void NetProcAccept() {

	int AddrLen = sizeof(SOCKADDR_IN);
	SOCKADDR_IN ClientAddr;
	SOCKET ClientSock = accept(g_ListenSock, (SOCKADDR *)&ClientAddr, &AddrLen);
	if (ClientSock == INVALID_SOCKET) {
		wprintf(L"accept()_ERROR\n");
		return;
	}

	// Ŭ���̾�Ʈ �ʱ�ȭ
	st_CLIENT *pClient = new st_CLIENT;
	pClient->Socket = ClientSock;
	pClient->ConnectAddr = ClientAddr;
	pClient->dwUserNo = g_dwKey_UserNO;
	pClient->dwEnterRoomNo = 0;

	// ����
	AddClient(g_dwKey_UserNO, pClient);
	
	// UserNO ����
	g_dwKey_UserNO++;
}

// DisconnectClient
void DisconnectClient(DWORD dwUserNO) {

	st_CLIENT *pClient = FindClient(dwUserNO);
	DelClient(dwUserNO);
	delete pClient;
}

// ������� ��Ŷ�� Ư�� Ŭ���̾�Ʈ���� ������
void SendUnicast(st_CLIENT *pClient, st_PACKET_HEADER *pHeader, CPacket *pPacket) {

	if (pClient == NULL) {
		wprintf(L"SendUniCast Client is NULL\n");
		return;
	}

	pClient->SendQ.Enqueue((char *)pHeader, sizeof(st_PACKET_HEADER));
	pClient->SendQ.Enqueue((char *)pPacket->GetBufferPtr(), pPacket->GetPacketSize());
}

// ������� ��Ŷ�� ��� �������� ������
void SendBroadcast(st_PACKET_HEADER *pHeader, CPacket *pPacket) {

	st_CLIENT *pClient;
	map<DWORD, st_CLIENT *>::iterator ClientIter;
	map<DWORD, st_CLIENT *>::iterator ClientIterEnd = g_ClientMap.end();

	for (ClientIter = g_ClientMap.begin(); ClientIter != ClientIterEnd; ClientIter++) {

		pClient = ClientIter->second;
		SendUnicast(pClient, pHeader, pPacket);
	}
}

// ������� ��Ŷ�� �� �������� ������, ���� ����ڴ� ����
void SendBroadcastRoom(st_CHAT_ROOM *pRoom, st_CLIENT *pClient, st_PACKET_HEADER *pHeader, CPacket *pPacket) {

	st_CLIENT *pDestClient;
	list<DWORD>::iterator iter;
	list<DWORD>::iterator iterEnd = pRoom->UserList.end();

	for (iter = pRoom->UserList.begin(); iter != iterEnd; ++iter) {

		pDestClient = FindClient(*iter);
	
		if (pClient != pDestClient)
			SendUnicast(pDestClient, pHeader, pPacket);
	}
}

// Packet�� �޾Ƽ� �̿����� CheckSum �����͸� �����
BYTE MakeCheckSum(CPacket *pPacket, WORD dwType) {

	int iSize = pPacket->GetPacketSize();
	BYTE *pPtr = (BYTE *)pPacket->GetBufferPtr();
	int iCheckSum = dwType;
	for (int iCnt = 0; iCnt < iSize; iCnt++) {

		iCheckSum += *pPtr;
		pPtr++;
	}
	return (BYTE)(iCheckSum % 256);
}

//----------------------------------------------------------------
// MakePacket
//----------------------------------------------------------------

// df_RES_LOGIN		2
void MakePacket_ResLogin(st_PACKET_HEADER *pHeader, CPacket *pPacket, BYTE byResult, st_CLIENT *pClient) {

	pPacket->Clear();

	*pPacket << byResult;
	*pPacket << pClient->dwUserNo;

	pHeader->byCode = dfPACKET_CODE;
	pHeader->byCheckSum = MakeCheckSum(pPacket, df_RES_LOGIN);
	pHeader->wMsgType = df_RES_LOGIN;
	pHeader->wPayloadSize = pPacket->GetPacketSize();

	//int breakPoint = 0;
}

// df_RES_ROOM_LIST		4
void MakePacket_ResRoomList(st_PACKET_HEADER *pHeader, CPacket *pPacket, BYTE byResult, st_CHAT_ROOM *pRoom) {

	pPacket->Clear();
	WORD NumOfRoom = (WORD)g_RoomMap.size();
	*pPacket << NumOfRoom;

	map<DWORD, st_CHAT_ROOM *>::iterator RoomIter;
	map<DWORD, st_CHAT_ROOM *>::iterator RoomIterEnd = g_RoomMap.end();

	for(RoomIter = g_RoomMap.begin(); RoomIter != RoomIterEnd; ++RoomIter) {

		st_CHAT_ROOM *pTmpRoom = RoomIter->second;

		// ��ѹ�
		*pPacket << pTmpRoom->dwRoomNO;
		
		// �� �̸� byte size
		WORD TitleSize = (WORD)pTmpRoom->szTitle.size() * 2;
		*pPacket << TitleSize;

		// �� �̸�
		pPacket->Enqueue((char *)&pTmpRoom->szTitle[0], TitleSize);

		// ���� �ο�
		BYTE NumInRoom = (BYTE)pTmpRoom->UserList.size();
		*pPacket << NumInRoom;

		// �г��� ~
		list<DWORD>::iterator UserKeyIter;
		list<DWORD>::iterator UserKeyIterEnd = pTmpRoom->UserList.end();
		for (UserKeyIter = pTmpRoom->UserList.begin(); UserKeyIter != UserKeyIterEnd; ++UserKeyIter) {

			DWORD dwUserKey = *UserKeyIter;
			st_CLIENT *pTmpClient = FindClient(dwUserKey);

			int NickNameSize = 30; // WCHAR[15]
			pPacket->Enqueue((char *)&pTmpClient->szNickName[0], NickNameSize);
		}
	}

	pHeader->byCode = dfPACKET_CODE;
	pHeader->byCheckSum = MakeCheckSum(pPacket, df_RES_ROOM_LIST);
	pHeader->wMsgType = df_RES_ROOM_LIST;
	pHeader->wPayloadSize = pPacket->GetPacketSize();

	//int breakPoint = 0;
}

// df_RES_ROOM_CREATE	6
void MakePacket_ResRoomCreate(st_PACKET_HEADER *pHeader, CPacket *pPacket, BYTE byResult, st_CHAT_ROOM *pRoom) {

	WORD wTitleSize;

	pPacket->Clear();
	*pPacket << byResult;

	if (byResult == df_RESULT_ROOM_CREATE_OK) {

		*pPacket << pRoom->dwRoomNO;
		wTitleSize = (WORD)(pRoom->szTitle.size() * 2); // ���� ���� ����
		*pPacket << wTitleSize;
		pPacket->Enqueue((char *)&pRoom->szTitle[0], wTitleSize);
	}

	pHeader->byCode = dfPACKET_CODE;
	pHeader->byCheckSum = MakeCheckSum(pPacket, df_RES_ROOM_CREATE);
	pHeader->wMsgType = df_RES_ROOM_CREATE;
	pHeader->wPayloadSize = pPacket->GetPacketSize();

	int breakPoint = 0;
}

// df_RES_ROOM_ENTER		8
void MakePacket_ResRoomEnter(st_PACKET_HEADER *pHeader, CPacket *pPacket, BYTE byResult, st_CHAT_ROOM *pRoom) {

	pPacket->Clear();
	*pPacket << byResult;
	
	// OK�� ��쿡�� ����
	if (byResult == df_RESULT_ROOM_ENTER_OK) {

		WORD wTitleSize = (WORD)(pRoom->szTitle.size() * 2);
		*pPacket << pRoom->dwRoomNO;
		*pPacket << wTitleSize;
		pPacket->Enqueue((char *)&pRoom->szTitle[0], wTitleSize);

		BYTE NumInRoom = (BYTE)(pRoom->UserList.size());
		*pPacket << NumInRoom;

		list<DWORD>::iterator UserIter;
		list<DWORD>::iterator UserIterEnd = pRoom->UserList.end();
		
		for (UserIter = pRoom->UserList.begin(); UserIter != UserIterEnd; ++UserIter) {

			DWORD dwUserKey = *UserIter;
			st_CLIENT *pClient = FindClient(dwUserKey);

			WCHAR NickTmp[15] = { 0, };
			wcscpy_s(NickTmp, 15, &pClient->szNickName[0]);
			pPacket->Enqueue((char *)&NickTmp, 30);
			*pPacket << pClient->dwUserNo;
		}
	}

	pHeader->byCode = dfPACKET_CODE;
	pHeader->byCheckSum = MakeCheckSum(pPacket, df_RES_ROOM_ENTER);
	pHeader->wMsgType = df_RES_ROOM_ENTER;
	pHeader->wPayloadSize = pPacket->GetPacketSize();
}

// df_RES_CHAT				10
void MakePacket_ResChat(st_PACKET_HEADER *pHeader, CPacket *pPacket, WORD wMsgSize, WCHAR *pContext, st_CLIENT *pClient) {

	pPacket->Clear();
	*pPacket << pClient->dwUserNo;
	*pPacket << wMsgSize;
	pPacket->Enqueue((char *)&pContext[0], wMsgSize);

	pHeader->byCode = dfPACKET_CODE;
	pHeader->byCheckSum = MakeCheckSum(pPacket, df_RES_CHAT);
	pHeader->wMsgType = df_RES_CHAT;
	pHeader->wPayloadSize = pPacket->GetPacketSize();
}

// df_RES_ROOM_LEAVE		12
void MakePacket_ResRoomLeave(st_PACKET_HEADER *pHeader, CPacket *pPacket, st_CLIENT *pClient) {

	pPacket->Clear();
	*pPacket << pClient->dwUserNo;

	pHeader->byCode = dfPACKET_CODE;
	pHeader->byCheckSum = MakeCheckSum(pPacket, df_RES_ROOM_LEAVE);
	pHeader->wMsgType = df_RES_ROOM_LEAVE;
	pHeader->wPayloadSize = pPacket->GetPacketSize();
}

// df_RES_ROOM_DELETE		13
void MakePacket_ResRoomDelete(st_PACKET_HEADER *pHeader, CPacket *pPacket, st_CHAT_ROOM *pRoom) {

	pPacket->Clear();
	*pPacket << pRoom->dwRoomNO;

	pHeader->byCode = dfPACKET_CODE;
	pHeader->byCheckSum = MakeCheckSum(pPacket, df_RES_ROOM_DELETE);
	pHeader->wMsgType = df_RES_ROOM_DELETE;
	pHeader->wPayloadSize = pPacket->GetPacketSize();
}

// df_RES_USER_ENTER		14
void MakePacket_ResUserEnter(st_PACKET_HEADER *pHeader, CPacket *pPacket, BYTE byResult, st_CLIENT *pClient) {

	pPacket->Clear();

	WCHAR NickTmp[15] = { 0, };
	wcscpy_s(NickTmp, 15, &pClient->szNickName[0]);
	pPacket->Enqueue((char *)&NickTmp, 30);
	*pPacket << pClient->dwUserNo;

	pHeader->byCode = dfPACKET_CODE;
	pHeader->byCheckSum = MakeCheckSum(pPacket, df_RES_USER_ENTER);
	pHeader->wMsgType = df_RES_USER_ENTER;
	pHeader->wPayloadSize = pPacket->GetPacketSize();
}

//----------------------------------------------------------------
// Response
//----------------------------------------------------------------

// df_RES_LOGIN		2
void Send_ResLogin(st_CLIENT *pClient, BYTE byResult, st_CHAT_ROOM *pRoom) {

	st_PACKET_HEADER stHeader;
	CPacket Packet;

	MakePacket_ResLogin(&stHeader, &Packet, byResult, pClient);

	SendUnicast(pClient, &stHeader, &Packet);
}

// df_RES_ROOM_LIST		4
void Send_ResRoomList(st_CLIENT *pClient, BYTE byResult, st_CHAT_ROOM *pRoom) {

	st_PACKET_HEADER stHeader;
	CPacket Packet;

	MakePacket_ResRoomList(&stHeader, &Packet, byResult, pRoom);

	//Packet.PrintPacket(true, 100);

	SendUnicast(pClient, &stHeader, &Packet);
}

// df_RES_ROOM_CREATE	6
void Send_ResRoomCreate(st_CLIENT *pClient, BYTE byResult, st_CHAT_ROOM *pRoom) {

	st_PACKET_HEADER stHeader;
	CPacket Packet;

	MakePacket_ResRoomCreate(&stHeader, &Packet, byResult, pRoom);

	// ä�ù� ���� ����� ������ ��� ��� �������� �Ѹ�
	if (byResult == df_RESULT_ROOM_CREATE_OK) {
	
		SendBroadcast(&stHeader, &Packet);
	}
	// �ƴ� ��쿡�� �� ����⸦ ��û�� �������Ը� �Ѹ�
	else {
	
		SendUnicast(pClient, &stHeader, &Packet);
	}
}

// df_RES_ROOM_ENTER		8
void Send_ResRoomEnter(st_CLIENT *pClient, BYTE byResult, st_CHAT_ROOM *pRoom) {

	st_PACKET_HEADER stHeader;
	CPacket Packet;

	// ä�ù� ���� ������, �� ���� �ִ� ��� �������� Ÿ ����� ���� �۽�
	if(byResult == df_RESULT_ROOM_ENTER_OK) {
		
		MakePacket_ResUserEnter(&stHeader, &Packet, byResult, pClient);
		SendBroadcastRoom(pRoom, pClient, &stHeader, &Packet);							// �� ��ġ ���� �ȵ�
	}

	// ä�ù� ���� ��û�� �������� ���忡 ���� Res �۽�
	MakePacket_ResRoomEnter(&stHeader, &Packet, byResult, pRoom);
	
	SendUnicast(pClient, &stHeader, &Packet);
}

// df_RES_CHAT				10
void Send_ResChat(st_CLIENT *pClient, WORD wMsgSize, WCHAR *pContext, st_CHAT_ROOM *pRoom) {

	st_PACKET_HEADER stHeader;
	CPacket Packet;

	MakePacket_ResChat(&stHeader, &Packet, wMsgSize, pContext, pClient);

	// �� ���� �ٸ� �����鿡�� �޽��� ����
	SendBroadcastRoom(pRoom, pClient, &stHeader, &Packet);
}

// df_RES_ROOM_LEAVE		12
void Send_ResRoomLeave(st_CLIENT *pClient, st_CHAT_ROOM *pRoom) {

	st_PACKET_HEADER stHeader;
	CPacket Packet;

	MakePacket_ResRoomLeave(&stHeader, &Packet, pClient);

	// �� �ȿ� �ִ� ����鿡�� ������ (�ڽ� ����)
	SendBroadcastRoom(pRoom, NULL, &stHeader, &Packet);
}

// df_RES_ROOM_DELETE		13
void Send_ResRoomDelete(st_CHAT_ROOM *pRoom) {

	st_PACKET_HEADER stHeader;
	CPacket Packet;

	MakePacket_ResRoomDelete(&stHeader, &Packet, pRoom);

	SendBroadcast(&stHeader, &Packet);
}

//--------------------------------------
// Request
//--------------------------------------

// df_REQ_LOGIN	1
bool NetPacket_ReqLogin(st_CLIENT *pClient, CPacket *pPacket) {

	// �г��� �ߺ� ���� Ȯ�� (wstring���� �� ��� ã�ƾ���)
	WCHAR tmp[15] = { 0, };
	pPacket->Dequeue((char *)&tmp, sizeof(WCHAR) * 15);
	
	st_CLIENT *pCmpClient = NULL;
	bool OverlapCheck = false;

	map<DWORD, st_CLIENT *>::iterator iter;
	map<DWORD, st_CLIENT *>::iterator iterEnd = g_ClientMap.end();

	for (iter = g_ClientMap.begin(); iter != iterEnd; ++iter) {

		pCmpClient = (*iter).second;

		if (pCmpClient->szNickName._Equal(tmp)) {
			OverlapCheck = true;
			break;
		}
	}

	// ����� �ʰ� Ȯ��, ��Ÿ���� ��üũ

	if (OverlapCheck) {
		Send_ResLogin(pClient, df_RESULT_LOGIN_DNICK, NULL);
	}
	else {
		// OK
		wprintf(L"�α��� [UserNO : %d][NickName : %s]\n", pClient->dwUserNo, (WCHAR *)&pClient->szNickName);
		pClient->szNickName = tmp;
		Send_ResLogin(pClient, df_RESULT_LOGIN_OK, NULL);
	}

	return true;
}

// df_REQ_ROOM_LIST		3
bool NetPacket_ReqRoomList(st_CLIENT *pClient, CPacket *pPacket) {

	Send_ResRoomList(pClient, NULL, NULL);

	return true;
}

// df_REQ_ROOM_CREATE	5
bool NetPacket_ReqRoomCreate(st_CLIENT *pClient, CPacket *pPacket) {

	WCHAR szRoomTitle[256] = { 0, };
	WORD wTitleSize;
	
	// 256 �̻��� ��� ����ó�� �ؾ���
	*pPacket >> wTitleSize;
	if (wTitleSize > 256) {
		pPacket->Dequeue((char *)&szRoomTitle, 255);
		szRoomTitle[255] = '\0';
	}
	else
		pPacket->Dequeue((char *)&szRoomTitle, wTitleSize);

	// �� ���� �۾�
	st_CHAT_ROOM *pRoom = new st_CHAT_ROOM;
	
	pRoom->dwRoomNO = g_dwKey_RoomNO++;

	// �迭 ����
	pRoom->szTitle = szRoomTitle;

	// �� �߰�
	AddRoom(pRoom->dwRoomNO, pRoom);

	// �� ���� �α�
	//wprintf(L"�� ���� [UserNO : %d][Room : %s][TotalRoom : %d]\n", pClient->dwUserNo, (WCHAR *)&pRoom->szTitle, g_RoomMap.size());
	//wcout << L"�� ���� [UserNO : " << pClient->dwUserNo << "][Room : " << pRoom->szTitle << "][TotalRoom : " << g_RoomMap.size() << "]" << endl;
	wprintf(L"�� ����\n"); // �̰� �� �ȵ�?

	// �� ���� ����� Ŭ���̾�Ʈ�� ����
	Send_ResRoomCreate(pClient, df_RESULT_ROOM_CREATE_OK, pRoom);
	return true;
}

// df_REQ_ROOM_ENTER		7
bool NetPacket_ReqRoomEnter(st_CLIENT *pClient, CPacket *pPacket) {

	DWORD RoomNum;
	*pPacket >> RoomNum;

	// ���� ���ٸ�,
	if (g_RoomMap.count(RoomNum) == 0) {
		Send_ResRoomEnter(pClient, df_RESULT_ROOM_ENTER_NOT, NULL);
		return true;
	}

	// ������ �κ� �ƴ� �ٸ� �濡 �ִٸ�,
	if (pClient->dwEnterRoomNo != 0) {
		Send_ResRoomEnter(pClient, df_RESULT_ROOM_ENTER_NOT, NULL);
		return true;
	}

	// ���� ���� ��ȭ
	pClient->dwEnterRoomNo = RoomNum;
	
	// �� ���� ��ȭ
	st_CHAT_ROOM *pRoom = FindRoom(RoomNum);

	pRoom->UserList.push_back(pClient->dwUserNo);

	Send_ResRoomEnter(pClient, df_RESULT_ROOM_ENTER_OK, pRoom);

	return true;
}

// df_REQ_CHAT		9
bool NetPacket_ReqChat(st_CLIENT *pClient, CPacket *pPacket) {

	// �޽��� ������
	WORD wMsgSize;
	*pPacket >> wMsgSize;

	// �޽��� ����, �����ϸ� string���� �ٲ���
	WCHAR *pContext = new WCHAR[wMsgSize / 2 + 1];
	memset(pContext, 0, sizeof(WCHAR) * wMsgSize / 2);
	pPacket->Dequeue((char *)&pContext[0], wMsgSize);
	pContext[wMsgSize / 2] = '\0';

	st_CHAT_ROOM *pRoom = FindRoom(pClient->dwEnterRoomNo);
	Send_ResChat(pClient, wMsgSize, pContext, pRoom);

	delete pContext;

	return true;
}

// df_REQ_ROOM_LEAVE		11
bool NetPacket_ReqRoomLeave(st_CLIENT *pClient, CPacket *pPacket) {

	st_CHAT_ROOM *pRoom = FindRoom(pClient->dwEnterRoomNo);
 	
	Send_ResRoomLeave(pClient, pRoom);

	// ���� ���� ����
	pClient->dwEnterRoomNo = 0;

	// �� ���� ���� (�濡�� ���� ���� ����)
	list<DWORD>::iterator RoomIter;
	list<DWORD>::iterator RoomIterEnd = pRoom->UserList.end();

	for (RoomIter = pRoom->UserList.begin(); RoomIter != RoomIterEnd;) {

		if (*RoomIter == pClient->dwUserNo) {
			RoomIter = pRoom->UserList.erase(RoomIter);
			break;
		}
		else {
			++RoomIter;
		}
	}

	// �濡 ����� ���ٸ� �����
	if (pRoom->UserList.empty() == true) {
		g_RoomMap.erase(pRoom->dwRoomNO);
		Send_ResRoomDelete(pRoom);
	}

	return true;
}

// Ÿ�Ժ� ��Ŷ �޽��� ó����(PayLoad ó����)
bool PacketProc(st_CLIENT *pClient, WORD wMsgType, CPacket *pPacket) {

	wprintf(L"PacketRecv[UserNO : %d][Type : %d]\n", pClient->dwUserNo, wMsgType);
	switch (wMsgType) {

	case df_REQ_LOGIN:
		return NetPacket_ReqLogin(pClient, pPacket);
		break;
	case df_REQ_ROOM_LIST:
		return NetPacket_ReqRoomList(pClient, pPacket);
		break;
	case df_REQ_ROOM_CREATE:
		return NetPacket_ReqRoomCreate(pClient, pPacket);
		break;
	case df_REQ_ROOM_ENTER:
		return NetPacket_ReqRoomEnter(pClient, pPacket);
		break;
	case df_REQ_CHAT:
		return NetPacket_ReqChat(pClient, pPacket);
		break;
	case df_REQ_ROOM_LEAVE:
		return NetPacket_ReqRoomLeave(pClient, pPacket);
		break;
	}

	return true;
}

// �ϳ��� ��Ŷ ó���� 
int CompleteRecvPacket(st_CLIENT *pClient) {

	st_PACKET_HEADER stHeader;
	int iRecvQSize = pClient->RecvQ.GetUseSize();

	// ���� ���� �˻�, ��Ŷ��� ũ�� �̻� ������ �ƴ϶��, �н�
	if (iRecvQSize < sizeof(st_PACKET_HEADER)) {
		//wprintf(L"��� ũ�⺸�� ����\n");
		return 1;
	}

	// 1.��Ŷ�ڵ� �˻�
	pClient->RecvQ.Peek((char *)&stHeader, sizeof(st_PACKET_HEADER));

	if (stHeader.byCode != dfPACKET_CODE) {
		wprintf(L"�߸��� ��Ŷ�ڵ�\n");
		return -1;
	}

	// 2. ť�� ����� �����Ͱ� ����� �ϴ� ��Ŷ�� ũ�⸸ŭ �ִ��� Ȯ��
	if ((WORD)iRecvQSize < stHeader.wPayloadSize + sizeof(st_PACKET_HEADER)) {

		// iRecvSize ����� �۴ٸ�, ���� ��Ŷ�� �Ϸ���� �ʾ����Ƿ� ���߿� ó��
		//wprintf(L"��Ŷ �̿Ϸ�\n");
		return 1;
	}

	// iRecvSize�� ��Ŷ�̻� �ִٴ� ���̹Ƿ�, Peek�� ��� �����ŭ Q���� ���ָ鼭 ����
	pClient->RecvQ.MoveFront(sizeof(st_PACKET_HEADER));

	CPacket Pack;
	// PayLoad�κ� ��Ŷ ���۷� ����
	int iRetval = pClient->RecvQ.Dequeue(Pack.GetBufferPtr(), stHeader.wPayloadSize);
	if (iRetval != stHeader.wPayloadSize ) {
		wprintf(L"Dequeue �Ұ�\n");
		return -1;
	}

	// ��Ŷ Ŭ������ ���� �����͸� ��� ���Ƿ� �����͸� �־����Ƿ� 
	// Ŭ���� ������ ������ �̵�ó���� �ٽ� ������
	Pack.MoveWritePos(stHeader.wPayloadSize);

	// checksum Ȯ��
	BYTE byCheckSum = MakeCheckSum(&Pack, stHeader.wMsgType);
	if (byCheckSum != stHeader.byCheckSum) {
		wprintf(L"CheckSum ERROR - UserNO : %d\n", pClient->dwUserNo);
		return -1;
	}

	// �������� ��Ŷ ó���Լ� ȣ��
	if (!PacketProc(pClient, stHeader.wMsgType, &Pack))
		return -1;

	return 0;
}

void NetProcSend(DWORD dwUserNO) {

	st_CLIENT *pClient;
	char *pSendBuff;
	int iRetval;
	int iSendSize;

	// �ش� ����� ����ã��
	pClient = FindClient(dwUserNO);
	if (pClient == NULL)
		return;

	// SendQ�� �ִ� �����͸� �ִ�� ������
	iSendSize = pClient->SendQ.GetUseSize();
	iSendSize = min(dfRECV_BUFF, iSendSize);
	pSendBuff = new char[iSendSize];

	// ť�� ���� �����Ͱ� ���� ��� ������
	if (iSendSize <= 0)
		return;

	// �ϴ� Peek �Լ��� ���� ��,
	// ������ ����� ������ �Ǿ��ٸ� ����
	pClient->SendQ.Peek(pSendBuff, iSendSize);

	// ����
	iRetval = send(pClient->Socket, pSendBuff, iSendSize, 0);
	delete pSendBuff;

	// ���� ����
	if (iRetval == SOCKET_ERROR) {

		DWORD dwError = WSAGetLastError();
		if (dwError == WSAEWOULDBLOCK) {
			wprintf(L"Socket WSAEWOULDBLOCK - UserNO : %d\n", dwUserNO);
			return;
		}
		wprintf(L"Socket Error - UserNO : %d ErrorCode : %d\n", dwUserNO, dwError);

		closesocket(pClient->Socket);
		DisconnectClient(dwUserNO);
		return;
	}

	// ���� �Ϸ�
	else {

		pClient->SendQ.MoveFront(iRetval);
	}
	return;
}

void NetProcRecv(DWORD dwUserNO) {

	st_CLIENT *pClient;
	char RecvBuff[dfRECV_BUFF];
	int iRetval;

	memset(RecvBuff, 0, sizeof(RecvBuff));

	// �ش� ����� ����ã��
	pClient = FindClient(dwUserNO);

	if (pClient == NULL)
		return;

	// �ޱ� �۾�(���� ���� ���� ��)
	iRetval = recv(pClient->Socket, RecvBuff, dfRECV_BUFF, 0);
	//iRetval = recv(pClient->Socket, pClient->RecvQ.GetRearBufferPtr(), dfRECV_BUFF, 0);
	//pClient->RecvQ.MoveRear(iRetval);

	// �޴ٰ� �������� ����
	if (iRetval == SOCKET_ERROR || iRetval == 0) {

		closesocket(pClient->Socket);
		DisconnectClient(dwUserNO);
		return;
	}
	
	// ���� ������ �ִٸ�
	if (iRetval > 0)
	{
		// �ϴ� RecvQ�� �ִ´�
		pClient->RecvQ.Enqueue(RecvBuff, iRetval);

		// ��Ŷ�� �Ϸ�Ǿ����� Ȯ��
		// ��Ŷó�� ��, ������ �߻� �Ѵٸ� ����ó��
		// ��Ŷ�� �ϳ� �̻��� ���ۿ� ���� �� �����Ƿ� �ݺ������� �� ���� ���� ó��
		for (;;) {

			iRetval = CompleteRecvPacket(pClient);
			// �� �̻� ó���� ��Ŷ ����
			if (iRetval == 1)
				break;

			if (iRetval == -1) {

				wprintf(L"Pakcet_ERROR	USER_NO : %d\n", dwUserNO);
				return;
			}
		}
	}
}

void SelectSocket(DWORD *dwpTableNO, SOCKET *pTableSOCKET, FD_SET *pReadSet, FD_SET *pWriteSet)
{
	timeval Time;
	int iRetval, iCnt;

	// select �Լ��� ���ð� �Է�
	Time.tv_sec = 0;
	Time.tv_usec = 0;

	// ������ ��û�� ���� �������� Ŭ���̾�Ʈ���� �޽��� �۽��� üũ�Ѵ�
	iRetval = select(0, pReadSet, pWriteSet, 0, &Time);

	// ���ϰ��� 0 �̻��̶�� �������� �����Ͱ� �� ��
	if (iRetval > 0) {

		// TableSock ��ȸ, � ���Ͽ� ������ �� ������ Ȯ��
		for (iCnt = 0; iCnt < FD_SETSIZE; ++iCnt) {

			if (pTableSOCKET[iCnt] == INVALID_SOCKET)
				continue;

			// WriteSet Check
			if (FD_ISSET(pTableSOCKET[iCnt], pWriteSet)) {

				NetProcSend(dwpTableNO[iCnt]);
			}

			// ReadSet Check
			if (FD_ISSET(pTableSOCKET[iCnt], pReadSet)) {

				if (dwpTableNO[iCnt] == 0)
					NetProcAccept();
				else
					NetProcRecv(dwpTableNO[iCnt]);
			}
		}
	}

	else if (iRetval == SOCKET_ERROR) {

		wprintf(L"SelectSocket ERROR\n");
	}
}

// ��Ʈ��ũ ���ν���
void NetworkProcess() {

	st_CLIENT *pClient = NULL;
	DWORD UserTable_NO[FD_SETSIZE];
	SOCKET UserTable_SOCKET[FD_SETSIZE];

	int iSocketCount = 0;

	FD_SET ReadSet;
	FD_SET WriteSet;
	FD_ZERO(&ReadSet);
	FD_ZERO(&WriteSet);

	// �迭 �ʱ�ȭ
	memset(UserTable_NO, -1, sizeof(DWORD)*FD_SETSIZE);
	memset(UserTable_SOCKET, INVALID_SOCKET, sizeof(DWORD)*FD_SETSIZE);

	// ���� ���� �ֱ�(�ѹ���)
	FD_SET(g_ListenSock, &ReadSet);

	UserTable_NO[iSocketCount] = 0;
	UserTable_SOCKET[iSocketCount] = g_ListenSock;
	iSocketCount++;

	map<DWORD, st_CLIENT *>::iterator ClientIter;
	map<DWORD, st_CLIENT *>::iterator ClientIterEnd = g_ClientMap.end();
	
	for (ClientIter = g_ClientMap.begin(); ClientIter != ClientIterEnd; ) {

		// ClientIter �� �̸� ++ ( ++iter �� ���� ���� )
		pClient = ClientIter->second;
		ClientIter++;

		// pClient�� �۾�
		// �ش� Ŭ���̾�Ʈ ReadSet�� ���
		// SendQ�� ������ �ִٸ� WriteSet ���
		UserTable_NO[iSocketCount] = pClient->dwUserNo;
		UserTable_SOCKET[iSocketCount] = pClient->Socket;

		FD_SET(pClient->Socket, &ReadSet);

		if (pClient->SendQ.GetUseSize() > 0)
			FD_SET(pClient->Socket, &WriteSet);

		iSocketCount++;

		// select �ִ�ġ ����, ������� ���̺� ������ select ȣ�� �� ����
		if (iSocketCount >= FD_SETSIZE) {

			SelectSocket(UserTable_NO, UserTable_SOCKET, &ReadSet, &WriteSet);

			FD_ZERO(&ReadSet);
			FD_ZERO(&WriteSet);

			memset(UserTable_NO, -1, sizeof(DWORD)*FD_SETSIZE);
			memset(UserTable_SOCKET, INVALID_SOCKET, sizeof(SOCKET)*FD_SETSIZE);
			iSocketCount = 0;
		}
	}

	// ���� iSocketCount�� FD_SETSIZE�� �������� ���Ͽ�, 64�� ������ ������ �������̹Ƿ�
	// ���������� �ѹ� �� SelectSocket ȣ��
	if (iSocketCount > 0) {

		SelectSocket(UserTable_NO, UserTable_SOCKET, &ReadSet, &WriteSet);
	}
}

#endif
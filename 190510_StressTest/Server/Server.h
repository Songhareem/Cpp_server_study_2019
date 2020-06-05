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
// disassmble
// 
// __security_check_cookie (0D94FE0h)
// 1) ���� �����÷ο� �߻���, �߻��ϴ� ����


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
#include "CLogMaker.h"

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

// ����� ���� map
map<DWORD, st_CLIENT *>		g_ClientMap;

// ����� accept �� ��������
SOCKET						g_ListenSock = INVALID_SOCKET;

// ���� ����Ű, �� ����Ű�� ���� ��������
DWORD						g_dwKey_UserNO = 1;

// g_wsa
WSADATA g_wsa;

// g_ServerAddr
SOCKADDR_IN g_ServerAddr;

//Log
CLogMaker CLogMaker::Log;

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

	st_CLIENT *RetClient = NULL;

	int iCnt = g_ClientMap.count(dwKeyUserNO);

	if (iCnt == 1)
		RetClient = g_ClientMap.find(dwKeyUserNO)->second;

	return RetClient;
}

// DelClient
void DelClient(DWORD dwKeyUserNO) {

	g_ClientMap.erase(dwKeyUserNO);
}

// AcceptClient
void AcceptClient(st_CLIENT *pClient) {

	WCHAR szAddr[20] = { 0, };
	DWORD dwPort;
	DWORD dwUserNO = pClient->dwUserNo;
	InetNtop(AF_INET, (void *)&pClient->ConnectAddr.sin_addr, szAddr, 20);
	dwPort = ntohs(pClient->ConnectAddr.sin_port);

	wprintf(L"Accept %s:%d [UserNO : %d][Socket : %d][Port : %d]\n", szAddr, dwPort, dwUserNO, pClient->Socket, dwPort);
}

// DisconnectClient
void DisconnectClient(DWORD dwUserNO) {

	st_CLIENT *pClient = FindClient(dwUserNO);
	static int iDelCnt = 1;
	WCHAR szAddr[20] = { 0, };
	DWORD dwPort;
	InetNtop(AF_INET, (void *)&pClient->ConnectAddr.sin_addr, szAddr, 20);
	dwPort = ntohs(pClient->ConnectAddr.sin_port);
	
	char Log[80] = { 0, };
	wprintf(L"Client Size : %d\n", g_ClientMap.size());
	wprintf(L"[%04d] Disconnect %s:%d [UserNO : %d][Socket : %d][Port : %d]\n", iDelCnt++, szAddr, dwPort, pClient->dwUserNo, pClient->Socket, dwPort);
	//sprintf_s(Log,"Client Size : %d", g_ClientMap.size());
	//CLogMaker::GetInstance()->WriteLog(Log);
	//sprintf_s(Log,"[%04d] Disconnect [UserNO:%d][Socket:%d][Port:%d]", iDelCnt, pClient->dwUserNo, pClient->Socket, dwPort);
	//CLogMaker::GetInstance()->WriteLog(Log);

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

// 101
void MakePacket_ResStressEcho(st_PACKET_HEADER *pHeader, CPacket *pPacket) {

	pHeader->byCode = dfPACKET_CODE;
	pHeader->wMsgType = df_RES_STRESS_ECHO;
	pHeader->wPayloadSize = pPacket->GetPacketSize();
}

//----------------------------------------------------------------
// Response
//----------------------------------------------------------------

// 101
void Send_ResStressEcho(st_CLIENT *pClient, CPacket *pPacket) {

	st_PACKET_HEADER stHeader;

	// makePakcet
	MakePacket_ResStressEcho(&stHeader, pPacket);

	//delete pContent;
	SendUnicast(pClient, &stHeader, pPacket);
}

//--------------------------------------
// Request
//--------------------------------------

// 100
bool NetPacket_ReqStressEcho(st_CLIENT *pClient, CPacket *pPacket) {

	Send_ResStressEcho(pClient, pPacket);

	return true;
}

// Ÿ�Ժ� ��Ŷ �޽��� ó����(PayLoad ó����)
bool PacketProc(st_CLIENT *pClient, WORD wMsgType, CPacket *pPacket) {

	//wprintf(L"PacketRecv[UserNO : %d][Type : %d]\n", pClient->dwUserNo, wMsgType);
	switch (wMsgType) {

	case df_REQ_STRESS_ECHO:
		return NetPacket_ReqStressEcho(pClient, pPacket);
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

	//// checksum Ȯ��
	//BYTE byCheckSum = MakeCheckSum(&Pack, stHeader.wMsgType);
	//if (byCheckSum != stHeader.byCheckSum) {
	//	wprintf(L"CheckSum ERROR - UserNO : %d\n", pClient->dwUserNo);
	//	return -1;
	//}

	// �������� ��Ŷ ó���Լ� ȣ��
	if (!PacketProc(pClient, stHeader.wMsgType, &Pack))
		return -1;

	return 0;
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

	wprintf(L"ClientSock : %d\n", ClientSock);

	// Client ���� print
	AcceptClient(pClient);

	// ����
	AddClient(g_dwKey_UserNO, pClient);

	// UserNO ����
	g_dwKey_UserNO++;
}

void NetProcSend(DWORD dwUserNO) {

	st_CLIENT *pClient;
	char SendBuff[dfRECV_BUFF];
	int iRetval;
	int iSendSize = 0;

	// �ش� ����� ����ã��
	pClient = FindClient(dwUserNO);
	if (pClient == NULL)
		return;

	// SendQ�� �ִ� �����͸� �ִ�� ������
	iSendSize = pClient->SendQ.GetUseSize();
	//iSendSize = min(dfRECV_BUFF, iSendSize);
	
	// ť�� ���� �����Ͱ� ���� ��� ������
	if (iSendSize <= 0)
		return;

	// �ϴ� Peek �Լ��� ���� ��,
	// ������ ����� ������ �Ǿ��ٸ� ����
	//pClient->SendQ.Peek(SendBuff, iSendSize);

	// ���� ���н�, Disconnect �� ���̹Ƿ� ������ ������
	//iRetval = send(pClient->Socket, SendBuff, iSendSize, 0);
	if(iSendSize <= pClient->SendQ.GetNotBrokenGetSize()) {
		iRetval = send(pClient->Socket, pClient->SendQ.GetFrontBufferPtr(), iSendSize, 0);
		pClient->SendQ.MoveFront(iRetval);
	}
	else {
		int iRetval_1 = send(pClient->Socket, pClient->SendQ.GetFrontBufferPtr(), pClient->SendQ.GetNotBrokenGetSize(), 0);
		pClient->SendQ.MoveFront(iRetval_1);
		int iRetval_2 = send(pClient->Socket, pClient->SendQ.GetFrontBufferPtr(), iSendSize - pClient->SendQ.GetNotBrokenGetSize(), 0);
		pClient->SendQ.MoveFront(iRetval_2);
		
		if (iRetval_1 != SOCKET_ERROR && iRetval_2 != SOCKET_ERROR)
			iRetval = iRetval_1 + iRetval_2;
		else
			iRetval = SOCKET_ERROR;
	}
	
	// ���� ����
	if (iRetval == SOCKET_ERROR) {

		DWORD dwError = WSAGetLastError();
		if (dwError == WSAEWOULDBLOCK) {
			wprintf(L"Socket WSAEWOULDBLOCK - UserNO : %d\n", dwUserNO);
			return;
		}
		wprintf(L"Socket Error(Send) - UserNO : %d ErrorCode : %d\n", dwUserNO, dwError);

		closesocket(pClient->Socket);
		DisconnectClient(dwUserNO);
		return;
	}
	
	return;
}

void NetProcRecv(DWORD dwUserNO) {

	st_CLIENT *pClient;
	int iRetval;

	// �ش� ����� ����ã��
	pClient = FindClient(dwUserNO);
	if (pClient == NULL)
		return;
	
	int iSize = pClient->RecvQ.GetFreeSize();

	// �ޱ� �۾�(���� ���� ���� ��)
	if (iSize <= pClient->RecvQ.GetNotBrokenPutSize()) {

		iRetval = recv(pClient->Socket, pClient->RecvQ.GetRearBufferPtr(), iSize, 0);
		pClient->RecvQ.MoveRear(iRetval);
	}
	else {
		
		int iRetval_1 = recv(pClient->Socket, pClient->RecvQ.GetRearBufferPtr(), pClient->RecvQ.GetNotBrokenPutSize(), 0);
		pClient->RecvQ.MoveRear(iRetval_1);
		int iRetval_2 = recv(pClient->Socket, pClient->RecvQ.GetRearBufferPtr(), iSize - pClient->RecvQ.GetNotBrokenPutSize(), 0);
		pClient->RecvQ.MoveRear(iRetval_2);

		if (iRetval_1 != SOCKET_ERROR && iRetval_2 != SOCKET_ERROR)
			iRetval = iRetval_1 + iRetval_2;
		else
			iRetval = SOCKET_ERROR;
	}
	
	// �޴ٰ� �������� ����
	if (iRetval == SOCKET_ERROR || iRetval == 0) {

		wprintf(L"Socket Error(Recv) - UserNO : %d\n", dwUserNO);

		closesocket(pClient->Socket);
		DisconnectClient(dwUserNO);
		return;
	}
	
	// ���� ������ �ִٸ�
	if (iRetval > 0)
	{
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
	memset(UserTable_SOCKET, INVALID_SOCKET, sizeof(SOCKET)*FD_SETSIZE);

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
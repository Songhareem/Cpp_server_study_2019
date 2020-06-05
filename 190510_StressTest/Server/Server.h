// 
// select 인자?
// select (maxfdpl, ReadSet, WriteSet, ExceptSet, st_timeval)
// 
// select return 값?
// 1) 성공 시 반환 값 : 준비된 fd 개수
// 2) 시간 만료시 반환 값 : 0
// 3) 오류 시 반환 값 : -1
// 
// select 첫번째 인자 maxfdpl?
// maxfdpl : 
// 
// ExceptSet : 예외 발생 했는지 확인
//
// timeout 이 NULL 일 때 : 무한정 기다린다. FD 중 하나가 준비되거나 신호가 잡힐 대까지 차단된다.
// timeout 이 0 일 때 : 전혀 기다리지 않는다.차단 없이 FD 상태만 확인할 경우 쓰인다.
// timeout 이 0 이 아닐 때 : 지정된 sec 나 usec 만큼 기다린다. FD 중 하나가 준비되거나 시간이 만료되면 반환된다.
//
//
// string 객체에 대하여
// string 객체 초기화
// 1) 빈 공간 초기화, string str;
// 2) 어떠한 값으로 초기화, string str = "asdasd";
//
// string to char, char to string
// 1)  strcpy_s(ch, str.size()+1, str.c_str()); 
//
// 2) string str; char ch[10];
//    str = ch;
//
// string 객체의 글자 시작 지점은
// string[0] 이다
//
// map의 키값 유무 판단
// 1) Key가 고유하다면, Count() 사용해서 존재한다면 1, 존재하지 않는다면 0 리턴
// 2) Key가 고유하지않다면, if(map.find(Key) == map.end()), 찾지못했다면 true, 찾았다면 false 리턴 
//
// disassmble
// 
// __security_check_cookie (0D94FE0h)
// 1) 버퍼 오버플로우 발생시, 발생하는 에러


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

// 사용자 관리 map
map<DWORD, st_CLIENT *>		g_ClientMap;

// 사용자 accept 용 리슨소켓
SOCKET						g_ListenSock = INVALID_SOCKET;

// 유저 고유키, 방 고유키를 만들 전역변수
DWORD						g_dwKey_UserNO = 1;

// g_wsa
WSADATA g_wsa;

// g_ServerAddr
SOCKADDR_IN g_ServerAddr;

//Log
CLogMaker CLogMaker::Log;

// 네트워크 초기화
bool NetworkInitial() {

	// 윈속 시작
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

// 네트워크 종료
void NetworkClose() {

	closesocket(g_ListenSock);

	WSACleanup();
}

// 키 입력 함수
bool KeyProc() {

	// SPACE 버튼
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

// 만들어진 패킷을 특정 클라이언트에게 보내기
void SendUnicast(st_CLIENT *pClient, st_PACKET_HEADER *pHeader, CPacket *pPacket) {

	if (pClient == NULL) {
		wprintf(L"SendUniCast Client is NULL\n");
		return;
	}

	pClient->SendQ.Enqueue((char *)pHeader, sizeof(st_PACKET_HEADER));
	pClient->SendQ.Enqueue((char *)pPacket->GetBufferPtr(), pPacket->GetPacketSize());
}

// 만들어진 패킷을 모든 유저에게 보내기
void SendBroadcast(st_PACKET_HEADER *pHeader, CPacket *pPacket) {

	st_CLIENT *pClient;
	map<DWORD, st_CLIENT *>::iterator ClientIter;
	map<DWORD, st_CLIENT *>::iterator ClientIterEnd = g_ClientMap.end();

	for (ClientIter = g_ClientMap.begin(); ClientIter != ClientIterEnd; ClientIter++) {

		pClient = ClientIter->second;
		SendUnicast(pClient, pHeader, pPacket);
	}
}

// Packet을 받아서 이에대한 CheckSum 데이터를 만든다
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

// 타입별 패킷 메시지 처리부(PayLoad 처리부)
bool PacketProc(st_CLIENT *pClient, WORD wMsgType, CPacket *pPacket) {

	//wprintf(L"PacketRecv[UserNO : %d][Type : %d]\n", pClient->dwUserNo, wMsgType);
	switch (wMsgType) {

	case df_REQ_STRESS_ECHO:
		return NetPacket_ReqStressEcho(pClient, pPacket);
		break;
	}

	return true;
}

// 하나의 패킷 처리부 
int CompleteRecvPacket(st_CLIENT *pClient) {

	st_PACKET_HEADER stHeader;
	int iRecvQSize = pClient->RecvQ.GetUseSize();

	// 받은 내용 검사, 패킷헤더 크기 이상 받은게 아니라면, 패스
	if (iRecvQSize < sizeof(st_PACKET_HEADER)) {
		//wprintf(L"헤더 크기보다 작음\n");
		return 1;
	}

	// 1.패킷코드 검사
	pClient->RecvQ.Peek((char *)&stHeader, sizeof(st_PACKET_HEADER));

	if (stHeader.byCode != dfPACKET_CODE) {
		wprintf(L"잘못된 패킷코드\n");
		return -1;
	}

	// 2. 큐에 저장된 데이터가 얻고자 하는 패킷의 크기만큼 있는지 확인
	if ((WORD)iRecvQSize < stHeader.wPayloadSize + sizeof(st_PACKET_HEADER)) {

		// iRecvSize 사이즈가 작다면, 아직 패킷이 완료되지 않았으므로 나중에 처리
		//wprintf(L"패킷 미완료\n");
		return 1;
	}

	// iRecvSize가 패킷이상 있다는 것이므로, Peek한 헤더 사이즈만큼 Q에서 빼주면서 진행
	pClient->RecvQ.MoveFront(sizeof(st_PACKET_HEADER));

	CPacket Pack;
	// PayLoad부분 패킷 버퍼로 빼옴
	int iRetval = pClient->RecvQ.Dequeue(Pack.GetBufferPtr(), stHeader.wPayloadSize);
	if (iRetval != stHeader.wPayloadSize ) {
		wprintf(L"Dequeue 불가\n");
		return -1;
	}

	// 패킷 클래스에 버퍼 포인터를 얻어 임의로 데이터를 넣었으므로 
	// 클래스 내부의 사이즈 이동처리를 다시 해주자
	Pack.MoveWritePos(stHeader.wPayloadSize);

	//// checksum 확인
	//BYTE byCheckSum = MakeCheckSum(&Pack, stHeader.wMsgType);
	//if (byCheckSum != stHeader.byCheckSum) {
	//	wprintf(L"CheckSum ERROR - UserNO : %d\n", pClient->dwUserNo);
	//	return -1;
	//}

	// 실질적인 패킷 처리함수 호출
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

	// 클라이언트 초기화
	st_CLIENT *pClient = new st_CLIENT;
	pClient->Socket = ClientSock;
	pClient->ConnectAddr = ClientAddr;
	pClient->dwUserNo = g_dwKey_UserNO;
	pClient->dwEnterRoomNo = 0;

	wprintf(L"ClientSock : %d\n", ClientSock);

	// Client 정보 print
	AcceptClient(pClient);

	// 매핑
	AddClient(g_dwKey_UserNO, pClient);

	// UserNO 증가
	g_dwKey_UserNO++;
}

void NetProcSend(DWORD dwUserNO) {

	st_CLIENT *pClient;
	char SendBuff[dfRECV_BUFF];
	int iRetval;
	int iSendSize = 0;

	// 해당 사용자 세션찾기
	pClient = FindClient(dwUserNO);
	if (pClient == NULL)
		return;

	// SendQ에 있는 데이터를 최대로 보낸다
	iSendSize = pClient->SendQ.GetUseSize();
	//iSendSize = min(dfRECV_BUFF, iSendSize);
	
	// 큐에 보낼 데이터가 있을 경우 보낸다
	if (iSendSize <= 0)
		return;

	// 일단 Peek 함수로 빼낸 후,
	// 전송이 제대로 마무리 되었다면 삭제
	//pClient->SendQ.Peek(SendBuff, iSendSize);

	// 전송 실패시, Disconnect 할 것이므로 데이터 버린다
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
	
	// 전송 오류
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

	// 해당 사용자 세션찾기
	pClient = FindClient(dwUserNO);
	if (pClient == NULL)
		return;
	
	int iSize = pClient->RecvQ.GetFreeSize();

	// 받기 작업(아직 예비 버퍼 씀)
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
	
	// 받다가 에러나면 종료
	if (iRetval == SOCKET_ERROR || iRetval == 0) {

		wprintf(L"Socket Error(Recv) - UserNO : %d\n", dwUserNO);

		closesocket(pClient->Socket);
		DisconnectClient(dwUserNO);
		return;
	}
	
	// 받은 데이터 있다면
	if (iRetval > 0)
	{
		// 패킷이 완료되었는지 확인
		// 패킷처리 중, 문제가 발생 한다면 종료처리
		// 패킷은 하나 이상이 버퍼에 있을 수 있으므로 반복문으로 한 번에 전부 처리
		for (;;) {

			iRetval = CompleteRecvPacket(pClient);
			// 더 이상 처리할 패킷 없음
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

	// select 함수의 대기시간 입력
	Time.tv_sec = 0;
	Time.tv_usec = 0;

	// 접속자 요청과 현재 접속중인 클라이언트들의 메시지 송신을 체크한다
	iRetval = select(0, pReadSet, pWriteSet, 0, &Time);

	// 리턴값이 0 이상이라면 누군가의 데이터가 온 것
	if (iRetval > 0) {

		// TableSock 순회, 어떤 소켓에 반응이 온 것인지 확인
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

// 네트워크 프로시저
void NetworkProcess() {

	st_CLIENT *pClient = NULL;
	DWORD UserTable_NO[FD_SETSIZE];
	SOCKET UserTable_SOCKET[FD_SETSIZE];

	int iSocketCount = 0;

	FD_SET ReadSet;
	FD_SET WriteSet;
	FD_ZERO(&ReadSet);
	FD_ZERO(&WriteSet);

	// 배열 초기화
	memset(UserTable_NO, -1, sizeof(DWORD)*FD_SETSIZE);
	memset(UserTable_SOCKET, INVALID_SOCKET, sizeof(SOCKET)*FD_SETSIZE);

	// 리슨 소켓 넣기(한번만)
	FD_SET(g_ListenSock, &ReadSet);

	UserTable_NO[iSocketCount] = 0;
	UserTable_SOCKET[iSocketCount] = g_ListenSock;
	iSocketCount++;

	map<DWORD, st_CLIENT *>::iterator ClientIter;
	map<DWORD, st_CLIENT *>::iterator ClientIterEnd = g_ClientMap.end();

	for (ClientIter = g_ClientMap.begin(); ClientIter != ClientIterEnd; ) {

		// ClientIter 은 미리 ++ ( ++iter 와 같은 역할 )
		pClient = ClientIter->second;
		ClientIter++;

		// pClient로 작업
		// 해당 클라이언트 ReadSet에 등록
		// SendQ에 데이터 있다면 WriteSet 등록
		UserTable_NO[iSocketCount] = pClient->dwUserNo;
		UserTable_SOCKET[iSocketCount] = pClient->Socket;

		FD_SET(pClient->Socket, &ReadSet);

		if (pClient->SendQ.GetUseSize() > 0)
			FD_SET(pClient->Socket, &WriteSet);

		iSocketCount++;

		// select 최대치 도달, 만들어진 테이블 정보로 select 호출 후 정리
		if (iSocketCount >= FD_SETSIZE) {

			SelectSocket(UserTable_NO, UserTable_SOCKET, &ReadSet, &WriteSet);

			FD_ZERO(&ReadSet);
			FD_ZERO(&WriteSet);

			memset(UserTable_NO, -1, sizeof(DWORD)*FD_SETSIZE);
			memset(UserTable_SOCKET, INVALID_SOCKET, sizeof(SOCKET)*FD_SETSIZE);
			iSocketCount = 0;
		}
	}

	// 아직 iSocketCount이 FD_SETSIZE에 도달하지 못하여, 64개 이하의 소켓이 남은것이므로
	// 마지막으로 한번 더 SelectSocket 호출
	if (iSocketCount > 0) {

		SelectSocket(UserTable_NO, UserTable_SOCKET, &ReadSet, &WriteSet);
	}
}

#endif
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
// chat 메시지 길이? 클라이언트 창 너비에 보통 맞춰서 함
//
// Send 관련 함수는 인자 등 맞추지 않아도 됨
// Recv 관련 함수는 인자, return 맞춰서 함 ( 차후 함수 포인터 등으로 관리 위해서 )

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

// 사용자 관리 map
map<DWORD, st_CLIENT *>		g_ClientMap;

// 채팅방 관리 map
map<DWORD, st_CHAT_ROOM *>	g_RoomMap;

// 사용자 accept 용 리슨소켓
SOCKET						g_ListenSock = INVALID_SOCKET;

// 유저 고유키, 방 고유키를 만들 전역변수
DWORD						g_dwKey_UserNO = 1;
DWORD						g_dwKey_RoomNO = 1;

// g_wsa
WSADATA g_wsa;

// g_ServerAddr
SOCKADDR_IN g_ServerAddr;


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

	// 클라이언트 초기화
	st_CLIENT *pClient = new st_CLIENT;
	pClient->Socket = ClientSock;
	pClient->ConnectAddr = ClientAddr;
	pClient->dwUserNo = g_dwKey_UserNO;
	pClient->dwEnterRoomNo = 0;

	// 매핑
	AddClient(g_dwKey_UserNO, pClient);
	
	// UserNO 증가
	g_dwKey_UserNO++;
}

// DisconnectClient
void DisconnectClient(DWORD dwUserNO) {

	st_CLIENT *pClient = FindClient(dwUserNO);
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

// 만들어진 패킷을 방 유저에게 보내기, 관련 당사자는 제외
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

		// 방넘버
		*pPacket << pTmpRoom->dwRoomNO;
		
		// 방 이름 byte size
		WORD TitleSize = (WORD)pTmpRoom->szTitle.size() * 2;
		*pPacket << TitleSize;

		// 방 이름
		pPacket->Enqueue((char *)&pTmpRoom->szTitle[0], TitleSize);

		// 참여 인원
		BYTE NumInRoom = (BYTE)pTmpRoom->UserList.size();
		*pPacket << NumInRoom;

		// 닉네임 ~
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
		wTitleSize = (WORD)(pRoom->szTitle.size() * 2); // 문제 여지 있음
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
	
	// OK인 경우에만 전송
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

	// 채팅방 생성 결과가 성공인 경우 모든 유저에게 뿌림
	if (byResult == df_RESULT_ROOM_CREATE_OK) {
	
		SendBroadcast(&stHeader, &Packet);
	}
	// 아닌 경우에는 방 만들기를 요청한 유저에게만 뿌림
	else {
	
		SendUnicast(pClient, &stHeader, &Packet);
	}
}

// df_RES_ROOM_ENTER		8
void Send_ResRoomEnter(st_CLIENT *pClient, BYTE byResult, st_CHAT_ROOM *pRoom) {

	st_PACKET_HEADER stHeader;
	CPacket Packet;

	// 채팅방 입장 성공시, 방 내에 있는 모든 유저에게 타 사용자 입장 송신
	if(byResult == df_RESULT_ROOM_ENTER_OK) {
		
		MakePacket_ResUserEnter(&stHeader, &Packet, byResult, pClient);
		SendBroadcastRoom(pRoom, pClient, &stHeader, &Packet);							// 얘 위치 맘에 안듦
	}

	// 채팅방 입장 요청한 유저에게 입장에 대한 Res 송신
	MakePacket_ResRoomEnter(&stHeader, &Packet, byResult, pRoom);
	
	SendUnicast(pClient, &stHeader, &Packet);
}

// df_RES_CHAT				10
void Send_ResChat(st_CLIENT *pClient, WORD wMsgSize, WCHAR *pContext, st_CHAT_ROOM *pRoom) {

	st_PACKET_HEADER stHeader;
	CPacket Packet;

	MakePacket_ResChat(&stHeader, &Packet, wMsgSize, pContext, pClient);

	// 방 안의 다른 유저들에게 메시지 전송
	SendBroadcastRoom(pRoom, pClient, &stHeader, &Packet);
}

// df_RES_ROOM_LEAVE		12
void Send_ResRoomLeave(st_CLIENT *pClient, st_CHAT_ROOM *pRoom) {

	st_PACKET_HEADER stHeader;
	CPacket Packet;

	MakePacket_ResRoomLeave(&stHeader, &Packet, pClient);

	// 방 안에 있는 사람들에게 보내기 (자신 포함)
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

	// 닉네임 중복 오류 확인 (wstring으로 할 방법 찾아야함)
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

	// 사용자 초과 확인, 기타오류 미체크

	if (OverlapCheck) {
		Send_ResLogin(pClient, df_RESULT_LOGIN_DNICK, NULL);
	}
	else {
		// OK
		wprintf(L"로그인 [UserNO : %d][NickName : %s]\n", pClient->dwUserNo, (WCHAR *)&pClient->szNickName);
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
	
	// 256 이상인 경우 예외처리 해야함
	*pPacket >> wTitleSize;
	if (wTitleSize > 256) {
		pPacket->Dequeue((char *)&szRoomTitle, 255);
		szRoomTitle[255] = '\0';
	}
	else
		pPacket->Dequeue((char *)&szRoomTitle, wTitleSize);

	// 방 생성 작업
	st_CHAT_ROOM *pRoom = new st_CHAT_ROOM;
	
	pRoom->dwRoomNO = g_dwKey_RoomNO++;

	// 배열 복사
	pRoom->szTitle = szRoomTitle;

	// 방 추가
	AddRoom(pRoom->dwRoomNO, pRoom);

	// 방 생성 로그
	//wprintf(L"방 생성 [UserNO : %d][Room : %s][TotalRoom : %d]\n", pClient->dwUserNo, (WCHAR *)&pRoom->szTitle, g_RoomMap.size());
	//wcout << L"방 생성 [UserNO : " << pClient->dwUserNo << "][Room : " << pRoom->szTitle << "][TotalRoom : " << g_RoomMap.size() << "]" << endl;
	wprintf(L"방 생성\n"); // 이거 왜 안됨?

	// 방 생성 결과를 클라이언트로 전송
	Send_ResRoomCreate(pClient, df_RESULT_ROOM_CREATE_OK, pRoom);
	return true;
}

// df_REQ_ROOM_ENTER		7
bool NetPacket_ReqRoomEnter(st_CLIENT *pClient, CPacket *pPacket) {

	DWORD RoomNum;
	*pPacket >> RoomNum;

	// 방이 없다면,
	if (g_RoomMap.count(RoomNum) == 0) {
		Send_ResRoomEnter(pClient, df_RESULT_ROOM_ENTER_NOT, NULL);
		return true;
	}

	// 유저가 로비가 아닌 다른 방에 있다면,
	if (pClient->dwEnterRoomNo != 0) {
		Send_ResRoomEnter(pClient, df_RESULT_ROOM_ENTER_NOT, NULL);
		return true;
	}

	// 유저 상태 변화
	pClient->dwEnterRoomNo = RoomNum;
	
	// 방 상태 변화
	st_CHAT_ROOM *pRoom = FindRoom(RoomNum);

	pRoom->UserList.push_back(pClient->dwUserNo);

	Send_ResRoomEnter(pClient, df_RESULT_ROOM_ENTER_OK, pRoom);

	return true;
}

// df_REQ_CHAT		9
bool NetPacket_ReqChat(st_CLIENT *pClient, CPacket *pPacket) {

	// 메시지 사이즈
	WORD wMsgSize;
	*pPacket >> wMsgSize;

	// 메시지 내용, 가능하면 string으로 바꾸자
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

	// 유저 정보 변경
	pClient->dwEnterRoomNo = 0;

	// 방 정보 변경 (방에서 나간 유저 삭제)
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

	// 방에 사람이 없다면 방삭제
	if (pRoom->UserList.empty() == true) {
		g_RoomMap.erase(pRoom->dwRoomNO);
		Send_ResRoomDelete(pRoom);
	}

	return true;
}

// 타입별 패킷 메시지 처리부(PayLoad 처리부)
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

	// checksum 확인
	BYTE byCheckSum = MakeCheckSum(&Pack, stHeader.wMsgType);
	if (byCheckSum != stHeader.byCheckSum) {
		wprintf(L"CheckSum ERROR - UserNO : %d\n", pClient->dwUserNo);
		return -1;
	}

	// 실질적인 패킷 처리함수 호출
	if (!PacketProc(pClient, stHeader.wMsgType, &Pack))
		return -1;

	return 0;
}

void NetProcSend(DWORD dwUserNO) {

	st_CLIENT *pClient;
	char *pSendBuff;
	int iRetval;
	int iSendSize;

	// 해당 사용자 세션찾기
	pClient = FindClient(dwUserNO);
	if (pClient == NULL)
		return;

	// SendQ에 있는 데이터를 최대로 보낸다
	iSendSize = pClient->SendQ.GetUseSize();
	iSendSize = min(dfRECV_BUFF, iSendSize);
	pSendBuff = new char[iSendSize];

	// 큐에 보낼 데이터가 있을 경우 보낸다
	if (iSendSize <= 0)
		return;

	// 일단 Peek 함수로 빼낸 후,
	// 전송이 제대로 마무리 되었다면 삭제
	pClient->SendQ.Peek(pSendBuff, iSendSize);

	// 전송
	iRetval = send(pClient->Socket, pSendBuff, iSendSize, 0);
	delete pSendBuff;

	// 전송 오류
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

	// 전송 완료
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

	// 해당 사용자 세션찾기
	pClient = FindClient(dwUserNO);

	if (pClient == NULL)
		return;

	// 받기 작업(아직 예비 버퍼 씀)
	iRetval = recv(pClient->Socket, RecvBuff, dfRECV_BUFF, 0);
	//iRetval = recv(pClient->Socket, pClient->RecvQ.GetRearBufferPtr(), dfRECV_BUFF, 0);
	//pClient->RecvQ.MoveRear(iRetval);

	// 받다가 에러나면 종료
	if (iRetval == SOCKET_ERROR || iRetval == 0) {

		closesocket(pClient->Socket);
		DisconnectClient(dwUserNO);
		return;
	}
	
	// 받은 데이터 있다면
	if (iRetval > 0)
	{
		// 일단 RecvQ에 넣는다
		pClient->RecvQ.Enqueue(RecvBuff, iRetval);

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
	memset(UserTable_SOCKET, INVALID_SOCKET, sizeof(DWORD)*FD_SETSIZE);

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
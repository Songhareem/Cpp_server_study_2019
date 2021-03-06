// main.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
// 포인터는 항상 NULL포인터인지 처리해줘야함 (인자로 넣을시)
// 일부러 크래시 내는법 -> 포인터가 NULL인데 사용

// 패킷캡쳐 프로그램 -> 와이어 샤크 (127.0.0.1 은 못잡음, 이더넷을 통하는 데이터를 잡아내는 것이기 때문에 )
// 상대방 IP 확인해서 필터링
// src -> 데이터 보낸 쪽, Dest -> 데이터 받는쪽
// Ack 번호 -> 여기서부터 주세요
// win10 / win7 -> windowSize 각각 64kb / 8kb

// 게임 캡쳐 하는 이유?
// 서버가 언제 전환되는지, 움직일때 1초에 얼마나 패킷이 왔다갔다 하는지
// 얼마나 많은 데이터가 오가는지

// 1. 내코드 먼저 의심
// 2. 패킷캡쳐 클라 서버 둘 다 (노이즈 껴있으면 하드웨어나 망문제) 
// 3. Realteck 은 원인미상 트래픽 문제가 있음 ( 그래서 서버 컴퓨터에선 안쓴다 )

#include "pch.h"
#include <iostream>

/*
Session{

	SOCKET
	ID
	X
	Y
};

// 총 64개 소켓
g_listenSock;

g_sessionList;

g_ID;

서버가 잘 작동하는지? 옵저버 클라이언트 새로 필요

리슨소켓 생성
바인드
listen()

while(1) {
	
	키입력()
	네트워크()
	업데이트 로직()
	랜더링() (실제 서버라면 모니터링() - 로그 등 - 으로 대체)
}

네트워크() {

	FD_SET ReadSet
	FD_ZERO(&ReadSet)
	timeval time;	// timeout -> NULL로 select 무한대기 시켜야함(데이터 와야 일 할게 있으므로)
	FD_SET(&g_listenSock,&ReadSet);
	for(SessionList 순회)
		FD_SET(Session->sock,&ReadSet)

	retval = select(0,&ReadSet,NULL, NULL, &time); // 최대 retval값은 64 * 3 (ReadSet 64, WriteSet 64, ExcSet 64)

	if(retval > 0 )
		for( SessionList 순회 )
			if(FD_ISSET(Session->sock,&ReadSet))
				// 수신부
				retval = recv(SessionSock,&Pack, sizeof(Pack),0);
				//예외처리
				if(re == 0) {
				
					Disconnect(세션) {
						오류난 세션 포인터 반환 하는 함수
					}
				}

				else if (re == SOCK_ERR){
					
					ErrCode = WSAGetLastError();
					ErrCode 확인해서 에러코드 눈에 익어야함
					Disconnect(세션)
				}

				switch(Pack.type) {

				case 3:
					MoveStar(Pack.ID, Pack.X, Pack.Y) // send() 포함
					break;
				}

				if(FD_ISSET(g_listenSock,&ReadSet))
					socket = accept()
					if(socket == SOCK_ERR) {
						Log 찍음
					}

					CreateStar(Sock); // ID 할당, 별 위치 할당
				}

// 만들 함수
send_Unicast(대상, &Packet) {}
send_Broadcast(제외시킬 대상 필요(* 세션, NULL은 다 뿌리기),&Pack) {}

Disconnet(세션) {
	SOCKET_ERR? 소켓 API 호출시 일어날 수 있는 에러
	소켓 API 호출하는 함수? send(), recv()

	closesocket(세션->sock); // 멀티스레드에서는 좀 더 신경써서 에러처리 해야함 (리소스만 반환하고 sock 내에 값이 남으므로)
	Pack.Type = 2;
	Pack.ID = 세션->ID;
	SessionList에서 세션삭제

	send_Broadcast(NULL,&Pack);
}

MoveStar(ID, X, Y) {
	ID 세션검색 -> 세션 저장(리스트)
	세션 -> X , Y 반영

	Pack.Type = 3;
	Pack.ID = 세션ID
	Pack.X = 세션X
	Pack.Y = 세션Y

	sendBroadCast(세션,&Pack)
}

CreateStar(Sock) {

	세션 동적할당
	세션 -> ID = g_ID;
	세션 -> Sock = Sock;
	세션 -> X,Y = X,Y;
	세션LIST(세션)

	// 만들어지는 클라 정보를 모든 클라에게
	Pack.type = 0;
	Pack.ID = 세션->ID;
	send_Unicast(세션,&Pack);

	// 모든 클라의 정보를 만들어지는 클라에게

	Pack.type = 1;
	
	for( sessionList 순회) {

		Pack ID = 세션 ID
		Pack X = 세션 X
		Pack Y = 세션 Y

		// 신규세션인 놈은 모두의 정보를 받음
		
		// 나머지는 신규 놈 정보만 받음
		send_Unicast(신규세션, &Pack);
	}
}

retval = send( - )
	if( retval == SOCK_ERR){
		WSAGetLastError
		Disconnect()
	}

*/

int main()
{
	

}

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.

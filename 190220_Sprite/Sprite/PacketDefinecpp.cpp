
#include "Sprite.h"
#include "PacketDefine.h"

////------------------------------
//// Recv 관련 함수
////------------------------------
//bool SC_CreateMyCharacter(char *pPack) {
//
//	stPACKET_CREATE_CHARACTER *pPacket = (stPACKET_CREATE_CHARACTER *)pPack;
//
//	int ID = pPacket->ID;
//	int Dir = pPacket->Direction;
//	int X = pPacket->X;
//	int Y = pPacket->Y;
//	int HP = pPacket->HP;
//
//	CPlayerObject *pPlayer = new CPlayerObject(ID, static_cast<int>(e_OBJECT_TYPE::eTYPE_PLAYER), true, Dir, HP);
//	pPlayer->ActionInput(dfACTION_STAND);
//	pPlayer->SetPosition(X, Y);
//	pPlayer->SetActionStand();
//	g_pPlayerObject = pPlayer;
//	g_ObjectList.push_back(g_pPlayerObject);
//
//	return true;
//}
//
//bool SC_CreateOtherCharacter(char *pPack) {
//
//	stPACKET_CREATE_CHARACTER *pPacket = (stPACKET_CREATE_CHARACTER *)pPack;
//
//	int ID = pPacket->ID;
//	int Dir = pPacket->Direction;
//	int X = pPacket->X;
//	int Y = pPacket->Y;
//	int HP = pPacket->HP;
//
//	CBaseObject *pObject;
//	CPlayerObject *pPlayer = new CPlayerObject(ID, static_cast<int>(e_OBJECT_TYPE::eTYPE_PLAYER), false, Dir, HP);
//	pPlayer->ActionInput(dfACTION_STAND);
//	pPlayer->SetPosition(X, Y);
//	pPlayer->SetActionStand();
//	pObject = pPlayer;
//	g_ObjectList.push_back(pObject);
//
//	return true;
//}
//
//bool SC_DeleteCharacter(char *pPack) {
//
//	stPACKET_DELETE_CHARACTER *pPacket = (stPACKET_DELETE_CHARACTER *)pPack;
//
//	int ID = pPacket->ID;
//
//	list<CBaseObject *>::iterator iter;
//
//	for (iter = g_ObjectList.begin(); iter != g_ObjectList.end(); ++iter) {
//
//		if ((*iter)->GetObjectID() == ID) {
//
//			g_ObjectList.erase(iter);
//			break;
//		}
//	}
//
//	return true;
//}
//
//bool SC_MoveStart(char *pPack) {
//
//	stPACKET_SC_MOVE *pPacket = (stPACKET_SC_MOVE *)pPack;
//
//	int ID = pPacket->ID;
//	int Action = pPacket->Direction; // 8방
//	int X = pPacket->X;
//	int Y = pPacket->Y;
//
//	int Dir;
//	switch (Action) {
//
//	case dfPACKET_MOVE_DIR_LL:
//	case dfPACKET_MOVE_DIR_LU:
//	case dfPACKET_MOVE_DIR_LD:
//		Dir = LEFT;
//		break;
//	case dfPACKET_MOVE_DIR_RR:
//	case dfPACKET_MOVE_DIR_RU:
//	case dfPACKET_MOVE_DIR_RD:
//		Dir = RIGHT;
//		break;
//	default:
//		Dir = -1;
//	}
//
//	list<CBaseObject *>::iterator iter;
//
//	for (iter = g_ObjectList.begin(); iter != g_ObjectList.end(); ++iter) {
//
//		if ((*iter)->GetObjectID() == ID) {
//
//			CPlayerObject *pPlayer = (CPlayerObject *)(*iter);
//			pPlayer->ActionInput(Action + 1);
//			if (Dir != -1)
//				pPlayer->SetDirection(Dir);
//			pPlayer->SetDirection(Dir);
//			pPlayer->SetPosition(X, Y);
//			pPlayer->SetActionMove();
//
//			break;
//		}
//	}
//
//	return true;
//}
//
//bool SC_MoveStop(char *pPack) {
//
//	stPACKET_SC_MOVE *pPacket = (stPACKET_SC_MOVE *)pPack;
//
//	int ID = pPacket->ID;
//	int Dir = pPacket->Direction;
//	int X = pPacket->X;
//	int Y = pPacket->Y;
//
//	list<CBaseObject *>::iterator iter;
//
//	for (iter = g_ObjectList.begin(); iter != g_ObjectList.end(); ++iter) {
//
//		if ((*iter)->GetObjectID() == ID) {
//
//			CPlayerObject *pPlayer = (CPlayerObject *)(*iter);
//			pPlayer->ActionInput(dfACTION_STAND);
//			pPlayer->SetDirection(Dir);
//			pPlayer->SetPosition(X, Y);
//			pPlayer->SetActionStand();
//
//			break;
//		}
//	}
//
//	return true;
//}
//
//bool SC_Attack1(char *pPack) {
//
//	stPACKET_SC_ATTACK *pPacket = (stPACKET_SC_ATTACK *)pPack;
//
//	int ID = pPacket->ID;
//	int Dir = pPacket->Direction;
//	int X = pPacket->X;
//	int Y = pPacket->Y;
//
//	list<CBaseObject *>::iterator iter;
//
//	for (iter = g_ObjectList.begin(); iter != g_ObjectList.end(); ++iter) {
//
//		if ((*iter)->GetObjectID() == ID) {
//
//			CPlayerObject *pPlayer = (CPlayerObject *)(*iter);
//			pPlayer->ActionInput(dfACTION_ATTACK1);
//			pPlayer->SetDirection(Dir);
//			pPlayer->SetPosition(X, Y);
//			pPlayer->SetActionAttack1();
//
//			break;
//		}
//	}
//
//	return true;
//}
//
//bool SC_Attack2(char *pPack) {
//
//	stPACKET_SC_ATTACK *pPacket = (stPACKET_SC_ATTACK *)pPack;
//
//	int ID = pPacket->ID;
//	int Dir = pPacket->Direction;
//	int X = pPacket->X;
//	int Y = pPacket->Y;
//
//	list<CBaseObject *>::iterator iter;
//
//	for (iter = g_ObjectList.begin(); iter != g_ObjectList.end(); ++iter) {
//
//		if ((*iter)->GetObjectID() == ID) {
//
//			CPlayerObject *pPlayer = (CPlayerObject *)(*iter);
//			pPlayer->ActionInput(dfACTION_ATTACK2);
//			pPlayer->SetDirection(Dir);
//			pPlayer->SetPosition(X, Y);
//			pPlayer->SetActionAttack2();
//
//			break;
//		}
//	}
//
//	return true;
//}
//
//bool SC_Attack3(char *pPack) {
//
//	stPACKET_SC_ATTACK *pPacket = (stPACKET_SC_ATTACK *)pPack;
//
//	int ID = pPacket->ID;
//	int Dir = pPacket->Direction;
//	int X = pPacket->X;
//	int Y = pPacket->Y;
//
//	list<CBaseObject *>::iterator iter;
//
//	for (iter = g_ObjectList.begin(); iter != g_ObjectList.end(); ++iter) {
//
//		if ((*iter)->GetObjectID() == ID) {
//
//			CPlayerObject *pPlayer = (CPlayerObject *)(*iter);
//			pPlayer->ActionInput(dfACTION_ATTACK3);
//			pPlayer->SetDirection(Dir);
//			pPlayer->SetPosition(X, Y);
//			pPlayer->SetActionAttack3();
//
//			break;
//		}
//	}
//
//	return true;
//}
//
//bool SC_Damage(char *pPack) {
//
//	stPACKET_DAMAGE *pPacket = (stPACKET_DAMAGE *)pPack;
//
//	int  AtkID = pPacket->AttackID;	// 이펙트를 주기 위한 ID, 일단 제외
//	int  DmgID = pPacket->DamageID;
//	BYTE DmgHP = pPacket->DamageHP;
//
//	list<CBaseObject *>::iterator iter;
//
//	for (iter = g_ObjectList.begin(); iter != g_ObjectList.end(); ++iter) {
//
//		if ((*iter)->GetObjectID() == DmgID) {
//
//			CPlayerObject *pDmgPlayer = (CPlayerObject *)(*iter);
//			pDmgPlayer->SetHP(DmgHP);
//
//			break;
//		}
//	}
//
//	return true;
//}
//
//void PacketProc(BYTE HeaderType, char *pPack) {
//
//	switch (HeaderType) {
//
//	case dfPACKET_SC_CREATE_MY_CHARACTER:
//		SC_CreateMyCharacter(pPack);
//		break;
//	case dfPACKET_SC_CREATE_OTHER_CHARACTER:
//		SC_CreateOtherCharacter(pPack);
//		break;
//	case dfPACKET_SC_DELETE_CHARACTER:
//		SC_DeleteCharacter(pPack);
//		break;
//	case dfPACKET_SC_MOVE_START:
//		SC_MoveStart(pPack);
//		break;
//	case dfPACKET_SC_MOVE_STOP:
//		SC_MoveStop(pPack);
//		break;
//	case dfPACKET_SC_ATTACK1:
//		SC_Attack1(pPack);
//		break;
//	case dfPACKET_SC_ATTACK2:
//		SC_Attack2(pPack);
//		break;
//	case dfPACKET_SC_ATTACK3:
//		SC_Attack3(pPack);
//		break;
//	case dfPACKET_SC_DAMAGE:
//		SC_Damage(pPack);
//		break;
//	default:
//		CLogMaker::GetInstance()->WriteLog((char *)"default Header Type");
//		break;
//	}
//}
//
//bool RecvEvent(SOCKET sock) {
//
//	char buf[1000] = { 0, };
//
//	// recv 호출 및 리턴값에 따른 종료 및 에러처리
//	int retval = recv(sock, buf, sizeof(buf), 0);
//	if (retval == SOCKET_ERROR) {
//		int ErrCode = WSAGetLastError();
//		if (ErrCode == WSAEWOULDBLOCK)
//			CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"recv() : WSAEWOULDBLOCK"));
//		else {
//			CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"recv()", ErrCode));
//		}
//		return false;
//	}
//
//	// recvQ에 넣고 에러처리
//	bool isRecv = g_RecvQ.Enqueue(buf, retval);
//	if (isRecv == false) {
//		CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"recv() : isRecv() FAIL"));
//		// DISCONNECT()
//	}
//
//	// 완료패킷 처리부
//	HEADER Head;
//	char *Pack;
//	int PackSize = 0;
//
//	while (1) {
//
//		// RecvQ에 최소한의 사이즈가 있는가? (헤더사이즈 초과)
//		int QSize = g_RecvQ.GetUseSize();
//		if (QSize < sizeof(HEADER)) {
//			CLogMaker::GetInstance()->WriteLog((char *)"Header Size Error");
//			break;
//		}
//
//		// RecvQ에서 헤더 Peek 및 헤더 코드 확인
//		g_RecvQ.Peek((char *)&Head, sizeof(HEADER));
//		if (Head.byCode != dfNETWORK_PACKET_CODE) {
//			CLogMaker::GetInstance()->WriteLog((char *)"Header Code Error");
//			g_RecvQ.MoveFront(1);
//			continue;
//		}
//
//		// 헤더의 Len값(endCode 제외 사이즈) 과 RecvQ의 데이터 사이즈 비교
//		PackSize = Head.bySize + 1;
//		if (g_RecvQ.GetUseSize() < PackSize) {
//			// 문제 여지 있음 clear
//			CLogMaker::GetInstance()->WriteLog((char *)"Garbage Packet");
//			g_RecvQ.ClearBuffer();
//			break;
//		}
//
//		// 데이터 Peek했던 헤더 지우고, PACKET 뽑아내고, EndCode 확인
//		g_RecvQ.MoveFront(sizeof(HEADER));
//		Pack = new char[PackSize];
//		g_RecvQ.Peek(Pack, PackSize);
//		//g_RecvQ.Peek((char *)&PackView, PackSize);								// for Debug
//		if (Pack[PackSize - 1] != dfNETWORK_PACKET_END) {
//			CLogMaker::GetInstance()->WriteLog((char *)"Header End Error");
//			g_RecvQ.MoveFront(PackSize);
//			continue;
//		}
//
//		g_RecvQ.MoveFront(PackSize);
//
//		int save = 0;
//
//		// 헤더 type에 따른 패킷처리
//		PacketProc(Head.byType, Pack);
//
//		delete[] Pack;
//
//	}
//
//	return true;
//}
//
////-----------------------------------
//// Send 관련 함수
////-----------------------------------
//void CS_MoveStart(HEADER *pHead, stPACKET_CS_MOVE *pPack, BYTE byDir, WORD wX, WORD wY) {
//
//	pHead->byCode = dfNETWORK_PACKET_CODE;
//	pHead->bySize = sizeof(stPACKET_CS_MOVE);
//	pHead->byType = dfPACKET_CS_MOVE_START;
//
//	pPack->Direction = byDir;
//	pPack->X = wX;
//	pPack->Y = wY;
//}
//
//void CS_MoveStop(HEADER *pHead, stPACKET_CS_MOVE *pPack, BYTE byDir, WORD wX, WORD wY) {
//
//	pHead->byCode = dfNETWORK_PACKET_CODE;
//	pHead->bySize = sizeof(stPACKET_CS_MOVE);
//	pHead->byType = dfPACKET_CS_MOVE_STOP;
//
//	pPack->Direction = byDir;
//	pPack->X = wX;
//	pPack->Y = wY;
//}
//
//void CS_Attack1(HEADER *pHead, stPACKET_CS_ATTACK *pPack, BYTE byDir, WORD wX, WORD wY) {
//
//	pHead->byCode = dfNETWORK_PACKET_CODE;
//	pHead->bySize = sizeof(stPACKET_CS_ATTACK);
//	pHead->byType = dfPACKET_CS_ATTACK1;
//
//	pPack->Direction = byDir;
//	pPack->X = wX;
//	pPack->Y = wY;
//}
//
//void CS_Attack2(HEADER *pHead, stPACKET_CS_ATTACK *pPack, BYTE byDir, WORD wX, WORD wY) {
//
//	pHead->byCode = dfNETWORK_PACKET_CODE;
//	pHead->bySize = sizeof(stPACKET_CS_ATTACK);
//	pHead->byType = dfPACKET_CS_ATTACK2;
//
//	pPack->Direction = byDir;
//	pPack->X = wX;
//	pPack->Y = wY;
//}
//
//void CS_Attack3(HEADER *pHead, stPACKET_CS_ATTACK *pPack, BYTE byDir, WORD wX, WORD wY) {
//
//	pHead->byCode = dfNETWORK_PACKET_CODE;
//	pHead->bySize = sizeof(stPACKET_CS_ATTACK);
//	pHead->byType = dfPACKET_CS_ATTACK3;
//
//	pPack->Direction = byDir;
//	pPack->X = wX;
//	pPack->Y = wY;
//}
//
//bool SendEvent(SOCKET socket) {
//
//	// SendQ에 남은 데이터 없을때까지 or Send 실패까지 계속 보냄
//	// 보낼수 없는 상태 될때까지 다 보내야함
//
//	// 보내기용 임시버퍼
//	char buf[1000];
//
//	// Send 가능여부 확인
//	if (g_netSendPossible == false)
//		return false;
//
//	// SendQ에 보낼 데이터 있는지 확인
//	if (g_SendQ.GetUseSize() < sizeof(HEADER)) {
//		g_SendQ.ClearBuffer();
//		return 1;
//	}
//
//	while (1) {
//
//		int UseSize = g_SendQ.GetUseSize();
//
//		if (UseSize < sizeof(HEADER)) {
//			g_SendQ.ClearBuffer();
//			break;
//		}
//
//		if (UseSize < sizeof(buf)) {
//
//			g_SendQ.Peek(buf, UseSize);
//			int retval = send(socket, buf, UseSize, 0);
//
//			if (retval == SOCKET_ERROR) {
//				int ErrCode = WSAGetLastError();
//				if (ErrCode == WSAEWOULDBLOCK) {
//					CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"send() : WSAEWOULDBLOCK"));
//					g_netSendPossible = false;
//				}
//				else {
//					CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"send()", ErrCode));
//				}
//			}
//
//			else if (retval == UseSize) {
//				g_SendQ.MoveFront(UseSize);
//			}
//		}
//		else {
//
//			g_SendQ.Peek(buf, sizeof(buf));
//			int retval = send(socket, buf, sizeof(buf), 0);
//
//			if (retval == SOCKET_ERROR) {
//				int ErrCode = WSAGetLastError();
//				if (ErrCode == WSAEWOULDBLOCK) {
//					CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"send() : WSAEWOULDBLOCK"));
//					g_netSendPossible = false;
//				}
//				else {
//					CLogMaker::GetInstance()->WriteLog(CLogMaker::GetInstance()->MakeErrContext((char *)"send()", ErrCode));
//				}
//			}
//
//			else if (retval == sizeof(buf)) {
//				g_SendQ.MoveFront(sizeof(buf));
//			}
//		}
//	}
//
//	return true;
//}
//
//bool SendPacket(HEADER *pHead, char *pPack, int iPackSize) {
//
//	// 접속 상태 예외처리
//	if (g_netConnect == false) {
//		CLogMaker::GetInstance()->WriteLog((char *)"ProcSend : Connect False Yet");
//		return false;
//	}
//
//	// 에러나면 DISCONNECT?
//	bool isFull = false;
//
//	// SendQ에 헤더넣기
//	isFull = g_SendQ.Enqueue((char *)pHead, sizeof(HEADER));
//	if (isFull == false) {
//		CLogMaker::GetInstance()->WriteLog((char *)"SendQ is Full, Header");
//		return false;
//	}
//	// SendQ에 패킷넣기
//	isFull = g_SendQ.Enqueue((char *)pPack, iPackSize);
//	if (isFull == false) {
//		CLogMaker::GetInstance()->WriteLog((char *)"SendQ is Full, Packet");
//		return false;
//	}
//
//	// SendQ에 EndCode 넣기
//	char EndCode = dfNETWORK_PACKET_END;
//	isFull = g_SendQ.Enqueue(&EndCode, sizeof(EndCode));
//	if (isFull == false) {
//		CLogMaker::GetInstance()->WriteLog((char *)"SendQ is Full, EndCode");
//		return false;
//	}
//
//	bool isSend = SendEvent(g_sock);
//	if (isSend == false)
//		return false;
//
//	return true;
//}
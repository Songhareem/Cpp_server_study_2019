#include "pch.h"
#include "CScreenBuffer.h"
#include "CBaseObject.h"
#include "CSceneManager.h"


CScreenBuffer::CScreenBuffer() : m_Back{ ' ', }
{
	cs_Initial();
}


CScreenBuffer::~CScreenBuffer()
{
}

void CScreenBuffer::Clear() {

	//*/
	for (int i = 0; i < HEIGHT; i++) {

		for (int j = 0; j < WIDTH - 1; j++) {

			m_Back[i][j] = ' ';
		}
		m_Back[i][WIDTH-1] = NULL;
	}
	//*/
}

void CScreenBuffer::Flip() {

	for (int i = 0; i < HEIGHT; i++) {

		cs_MoveCursor(0, i);
		printf("%s", m_Back[i]);
	}
}

// Title, End 화면에서 사용
void CScreenBuffer::Flip(int HEIGHT) {

	for (int i = 0; i < HEIGHT; i++) {

		cs_MoveCursor(0, i);
		printf("%s", m_Back[i]);
	}
}

void CScreenBuffer::cs_MoveCursor(int iPosX, int iPosY)
{
	COORD stCoord;
	stCoord.X = iPosX;
	stCoord.Y = iPosY;
	//-------------------------------------------------------------
	// 원하는 위치로 커서를 이동시킨다.
	//-------------------------------------------------------------
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), stCoord);
}

void CScreenBuffer::cs_Initial(void)
{
	CONSOLE_CURSOR_INFO stConsoleCursor;

	//-------------------------------------------------------------
	// 화면의 커서를 안보이게끔 설정한다.
	//-------------------------------------------------------------
	stConsoleCursor.bVisible = FALSE;
	stConsoleCursor.dwSize = 1;			// 커서 크기.
											// 이상하게도 0 이면 나온다. 1로하면 안나온다.

	//-------------------------------------------------------------
	// 콘솔화면 (스텐다드 아웃풋) 핸들을 구한다.
	//-------------------------------------------------------------

	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &stConsoleCursor);
}

void CScreenBuffer::Render(int ObjType, int iY, int iX, bool bDivision, char c) {
	
	switch (ObjType) {

	case CBaseObject::PLAYER:
		m_Back[iY][iX] = 'O';
		break;
	case CBaseObject::ENEMY:
		m_Back[iY][iX] = 'T';
		break;
	case CBaseObject::BULLET:
		if (bDivision == true) m_Back[iY][iX] = '|';
		else                   m_Back[iY][iX] = 'v';
		break;
	case CBaseObject::UI:
		m_Back[iY][iX] = c;
		break;
	}
}
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

// Title, End ȭ�鿡�� ���
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
	// ���ϴ� ��ġ�� Ŀ���� �̵���Ų��.
	//-------------------------------------------------------------
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), stCoord);
}

void CScreenBuffer::cs_Initial(void)
{
	CONSOLE_CURSOR_INFO stConsoleCursor;

	//-------------------------------------------------------------
	// ȭ���� Ŀ���� �Ⱥ��̰Բ� �����Ѵ�.
	//-------------------------------------------------------------
	stConsoleCursor.bVisible = FALSE;
	stConsoleCursor.dwSize = 1;			// Ŀ�� ũ��.
											// �̻��ϰԵ� 0 �̸� ���´�. 1���ϸ� �ȳ��´�.

	//-------------------------------------------------------------
	// �ܼ�ȭ�� (���ٴٵ� �ƿ�ǲ) �ڵ��� ���Ѵ�.
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
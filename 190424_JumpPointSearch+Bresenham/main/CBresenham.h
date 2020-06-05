#pragma once
class CBresenham
{
	enum eBaseLine{
	
		HORIZON = 0,
		VERTICAL
	};

	struct st_Pos {

		int iX;
		int iY;
	};

public:
	CBresenham();
	CBresenham(int iStrX, int iStrY, int iEndX, int iEndY);
	~CBresenham();

	// Set
	void SetPos(int iStrX, int iStrY, int iEndX, int iEndY);

	//------------------------------------------------------
	// GetNext
	// ��� ���� iX, iY�� ����
	// return false ���� �� �ִ� ���� ����
	// return true  ���� �� �ִ� �� ����
	//------------------------------------------------------
	bool GetNext(int *iX, int *iY);
	
	//------------------------------------------------------
	// GetPeek
	// ��� ���� iX, iY�� ���� (�̾Ƴ��� ����)
	// return false ���� �� �ִ� ���� ����
	// return true  ���� �� �ִ� �� ����
	//------------------------------------------------------
	bool GetPeek(int *iX, int *iY);

	// Bresenham Algorithm
	void Bresenham();

private:

	int _iStrX;
	int _iStrY;
	int _iEndX;
	int _iEndY;
	int _iX;
	int _iY;

	bool _blBaseLine;
	bool _blSignX;
	bool _blSignY;
	int _iHorizonLen;
	int _iVerticalLen;
	int _iError;
	int _iCheckEnd;
};


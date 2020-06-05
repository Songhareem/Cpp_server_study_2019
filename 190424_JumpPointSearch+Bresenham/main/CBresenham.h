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
	// 결과 값을 iX, iY로 전달
	// return false 받을 수 있는 값이 없음
	// return true  받을 수 있는 값 있음
	//------------------------------------------------------
	bool GetNext(int *iX, int *iY);
	
	//------------------------------------------------------
	// GetPeek
	// 결과 값을 iX, iY로 전달 (뽑아내지 않음)
	// return false 받을 수 있는 값이 없음
	// return true  받을 수 있는 값 있음
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


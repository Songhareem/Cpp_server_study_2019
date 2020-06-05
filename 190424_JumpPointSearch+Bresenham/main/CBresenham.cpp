#include "stdafx.h"
#include "CBresenham.h"

CBresenham::CBresenham()
{
}

CBresenham::CBresenham(int iStrX, int iStrY, int iEndX, int iEndY)
{
	SetPos(iStrX, iStrY, iEndX, iEndY);
}


CBresenham::~CBresenham()
{
}

// Set
void CBresenham::SetPos(int iStrX, int iStrY, int iEndX, int iEndY) {

	_iStrX = iStrX;
	_iStrY = iStrY;
	_iEndX = iEndX;
	_iEndY = iEndY;

	_iX = iStrX;
	_iY = iStrY;

	_iHorizonLen = iEndX - iStrX;
	_iVerticalLen = iEndY - iStrY;
	_iError = 0;
	_iCheckEnd = 0;

	if (_iHorizonLen > 0) {
		_blSignX = true;
	}
	else {
		_blSignX = false;
		_iHorizonLen *= -1;
	}

	if (_iVerticalLen > 0) {
		_blSignY = true;
	}
	else {
		_blSignY = false;
		_iVerticalLen *= -1;
	}

	_blBaseLine = _iHorizonLen >= _iVerticalLen ? HORIZON : VERTICAL;
}

// Get
bool CBresenham::GetNext(int *iX, int *iY) {

	// 예외처리
	switch (_blBaseLine) {

	case HORIZON:
	{
		if (_iCheckEnd > _iHorizonLen)
			return false;
	}
	break;
	case VERTICAL:
	{
		if (_iCheckEnd > _iVerticalLen)
			return false;
	}
	break;
	}

	// 값얻기
	*iX = _iX;
	*iY = _iY;

	// 다음 값으로 이동
	int VarianceX, VarianceY;

	if (_blSignX == true)
		VarianceX = 1;
	else
		VarianceX = -1;

	if (_blSignY == true)
		VarianceY = 1;
	else
		VarianceY = -1;

	switch (_blBaseLine) {

	case HORIZON:
	{
		_iX += VarianceX;
		_iError += _iVerticalLen + 1;			// +1 이 아니라 *2
		_iCheckEnd += 1;

		if (_iError >= _iHorizonLen && _iY != _iEndY) {
			_iY += VarianceY;
			_iError -= _iHorizonLen;
		}
	}
	break;
	case VERTICAL:
	{
		_iY += VarianceY;
		_iError += _iHorizonLen + 1;
		_iCheckEnd += 1;

		if (_iError >= _iVerticalLen && _iX != _iEndX) {
			_iX += VarianceX;
			_iError -= _iVerticalLen;
		}
	}
	break;
	}

	return true;
}

// Error = 긴축의 2/1 값 넣어주고 시작

bool CBresenham::GetPeek(int *iX, int *iY) {

	// 예외처리
	switch (_blBaseLine) {

	case HORIZON:
	{
		if (_iCheckEnd > _iHorizonLen)
			return false;
	}
	break;
	case VERTICAL:
	{
		if (_iCheckEnd > _iVerticalLen)
			return false;
	}
	break;
	}

	// 값얻기
	*iX = _iX;
	*iY = _iY;

	return true;
}

void CBresenham::Bresenham() {

	switch (_blBaseLine) {

	case HORIZON:
	{
		int x;
		int y = 0;
		for (x = 0; x < _iHorizonLen; x++) {

			_iError += _iVerticalLen;
			if (_iError >= _iVerticalLen)
				y++;
		}
	}
	break;
	case VERTICAL:
	{

	}
	break;
	}
}
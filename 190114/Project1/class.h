
#pragma once

using namespace std;

class BaseObject {

protected:
	// x = º°Âï´Â ½ÃÀÛ ÁöÁ¡
	int x = 0;

public:

	virtual bool Run() = 0;
	virtual void Render() = 0;
};

class OneStar : public BaseObject {

public:

	OneStar(int val = -1) {

		x = val;
	}

	// true : ¼Ò¸ê, false : ³öµÎ±â
	virtual bool Run() {

		x += 1;
		if (x >= 75) return true;
		return false;
	}

	virtual void Render() {

		for (int i = 0; i < x; i++) cout << " ";
		cout << "*" << endl;
		//cout << x << endl;

	}
};

class TwoStar : public BaseObject {

public:

	TwoStar(int val = -2) {

		x = val;
	}

	virtual bool Run() {

		x += 2;
		if (x >= 75) return true;
		return false;
	}

	virtual void Render() {

		for (int i = 0; i < x; i++) cout << " ";
		cout << "**" << endl;
	}
};

class ThreeStar : public BaseObject {

public:

	ThreeStar(int val = -3) {

		x = val;
	}

	virtual bool Run() {

		x += 3;
		if (x >= 75) return true;
		return false;
	}

	virtual void Render() {

		for (int i = 0; i < x; i++) cout << " ";
		cout << "***" << endl;
	}
};
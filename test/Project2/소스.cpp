
#include<iostream>
#include"CTest.h"
#include"CTest2.h"
#include <Windows.h>

#pragma comment (lib,"winmm.lib")

using namespace std;

class TEST {

public:

	int x;
	int y;

	TEST(int x=1, int y=2) : x(x), y(y) {}
	~TEST(){}

	TEST(const TEST &test) {
		
		x = test.x;
		y = test.y;
	}

	void operator ++(int) {
	
		x++;
		y++;
	}

	TEST operator +(const TEST &test) {

		TEST tmp;
		tmp.x = x + test.x;
		tmp.y = y + test.y;

		return tmp;
	}

	bool operator ==(const TEST &test) {
		
		if (x == test.x && y == test.y) return true;
		else false;
	}
};

typedef struct st_PACKET_HEADER {

	//DWORD dwPacketCode;		// 0x11223344

	//WCHAR szName[32];		// 본인이름, 유니코드 NULL문자 끝
	//WCHAR szFileName[128];	// 파일이름, 유니코드 NULL문자 끝
	//int iFileSize;
	DWORD c;

} PACKET_H;

void TestHexAndUnsignedInt();
void Hello();
int	 add(int a, int b);

// Main 함수
int main() {
	
	// 함수 포인터 모양은 받아서 쓸 함수의 반환값과 인자값이 같아야함
	void(*fp)();
	fp = Hello;
	fp();

	printf("origin : %p\n", Hello);
	printf("Pointer : %p\n", fp);

	int(*fp2)(int, int);
	fp2 = add;
	int retval = fp2(2,3);
	
	printf("%d\n", retval);

	class fruit
	{
	public:
		void apple()
		{
			cout << "apple" << endl;
		}
		void berry()
		{
			cout << "berry" << endl;
		}
	};

	fruit c1, *c2;
	void (fruit::*pfunc)();
	
	c2 = new fruit;
	
	pfunc = &fruit::apple;
	(c1.*pfunc)();
	
	pfunc = &fruit::berry;
	(c2->*pfunc)();

	delete c2;

	class Calculator
	{
	public:
		int sum(int a, int b)
		{
			return a + b;
		}
		int sub(int a, int b)
		{
			return a - b;
		}
		int mul(int a, int b)
		{
			return a * b;
		}
		int dev(int a, int b)
		{
			return a / b;
		}
	};

	Calculator Cal, *Cal2;
	Cal2 = new Calculator;

	int(Calculator::*pCalfunc)(int, int);
	pCalfunc = &Calculator::sum;
	printf("10 + 2 = %d\n", (Cal.*pCalfunc)(10, 2));
	
	/*pCalfunc = Calculator::sub;
	printf("10 + 2 = %d\n", pCalfunc);

	pCalfunc = Calculator::mul;
	printf("10 + 2 = %d\n", pCalfunc);

	pCalfunc = Calculator::dev;
	printf("10 + 2 = %d\n", pCalfunc);
*/
	return 0;
}

void TestHexAndUnsignedInt() {

	int retval = 0xdddddddd;

	WCHAR dot = L'．';

	if (retval == 0xdddddddd)
		std::cout << "good" << std::endl;

}

void Hello() {

	printf("Hello_World!\n");
}

int add(int a, int b) {

	return a + b;
}
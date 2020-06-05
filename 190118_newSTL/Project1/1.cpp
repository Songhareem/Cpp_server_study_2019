
#include <iostream>
//#include "List.h"
#include<list>

using namespace std;

class Test {
	
public :
	int x;
	int y;
	
	Test(int iX=1, int iY=1) : x(iX), y(iY) {}
	~Test() {}

	void setVal(int iX, int iY) {
		x = iX;
		y = iY;
	};
};

bool Compare_x(Test *item1, Test *item2)
{

	return item1->x < item2->x; // 오름차순 정렬

}

using namespace std;


int main() {
	
	Test *Tmp;
	list<Test *> testList;
	list<Test *>::iterator iter;

	Tmp = new Test(4, 16);
	testList.push_back(Tmp);

	Tmp = new Test(2, 12);
	testList.push_back(Tmp);

	Tmp = new Test(1, 10);
	testList.push_back(Tmp);

	Tmp = new Test(5, 18);
	testList.push_back(Tmp);

	Tmp = new Test(3, 14);
	testList.push_back(Tmp);

	printf("\nBefore\n\n");

	for (iter = testList.begin(); iter != testList.end(); ++iter) {

		printf("%d %d\n", (*iter)->x, (*iter)->y);
	}

	printf("\nAfter\n\n");

	// sort
	testList.sort(Compare_x);

	for (iter = testList.begin(); iter != testList.end(); ++iter) {

		printf("%d %d\n", (*iter)->x, (*iter)->y);
	}

	return 0;
}
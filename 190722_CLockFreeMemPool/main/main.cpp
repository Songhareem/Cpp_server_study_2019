

#include "pch.h"
#include <iostream>
#include "CLockFreeMemPool.h"

int main()
{
	class CTest {
	public:
		CTest() {

			printf("Create\n");
			a = 0;
		};

		~CTest() {

			printf("Destroy\n");
		};
		int a;
	};

	CLockFreeMemPool<CTest> LFMemPool(10, true);

	CTest *pTest = LFMemPool.Alloc();

	pTest->a = 10;

	printf("%d\n", pTest->a);
}

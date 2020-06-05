
#include <iostream>

/*////////////
   typedef
////////////*/

typedef struct allocInfo {

	void *ptr;
	int size;
	char file[100];
	int line;

} INFO;

/*////////////
    define
////////////*/
#define _MemAlloc(type,size) memAlloc<type>(size,__FILE__,__LINE__);
#define _MemRelease(type, p) memRelease<type>(p,__FILE__,__LINE__);

#define MAX_INFO 10

/*////////////
   ��������
////////////*/
INFO info[MAX_INFO];
char infoCnt = 0;

/*////////////
  namespace
////////////*/
using namespace std;

/*////////////
    ���ø�
////////////*/
template<typename type>
type *memAlloc(int size, const char *str, int i) {

	type *p = new type[sizeof(type) * size];
	
	// INFO struct�� ������ ����
	info[infoCnt].ptr = (void *)p;
	info[infoCnt].size = sizeof(type) * size;
	strcpy_s(&info[infoCnt].file[0], sizeof(info[infoCnt].file), str);
	info[infoCnt].line = i;
	infoCnt++;

	return p;
}

template<typename T>
void memRelease(T *p,const char *file, int line) {

	for (int i = 0; i < MAX_INFO; i++) {

		// �ش� ������(p) ������ INFO�� �ִٸ�
		if (info[i].ptr == p) {

			info[i].ptr = NULL;
			delete[] p;
			return;
		}
	}

	// ���ٸ�
	printf("wrong access in %s : %d\n\n", file, line);
	return;
}

/*////////////
   �Ϲ��Լ�
////////////*/
void printAlloc() {

	// make total alloc size
	int totalSize = 0;

	for (int i = 0; i < infoCnt; i++) {

		totalSize += info[i].size;
	}

	// ��º�
	printf("Total Alloc Size : %d\n", totalSize);
	printf("Total Alloc Count : %d\n\n", infoCnt);

	for (int i = 0; i < MAX_INFO; i++) {

		// �������� ���� �����Ͱ� �����ִٸ�
		if (info[i].ptr != NULL) {

			printf("Not Release Memory [0x%x] %dbyte \n", info[i].ptr, info[i].size);
			printf("File : %s : %dline \n\n", info[i].file, info[i].line);
		}
	}
}

int main() {

	// ������ �ʱ�ȭ -�Ҵ� ���� Ȯ�ο�- ( NULL : �Ҵ� �ȵ��� )
	for (int i = 0; i < MAX_INFO; i++) {

		info[i].ptr = NULL;
	}
	
	int *p4 = _MemAlloc(int, 1);
	int *p400 = _MemAlloc(int, 100);
	char *pZ1 = _MemAlloc(char, 50);
	char *pZ2 = _MemAlloc(char, 150);
	char *pZ3 = _MemAlloc(char, 60);
	char *pZ4 = _MemAlloc(char, 70);

	//_MemRelease(int, p4); // p4�� ������Ű�� ����
	_MemRelease(int, p400);
	_MemRelease(char, pZ1);
	_MemRelease(char, pZ2);
	_MemRelease(char, pZ3);
	_MemRelease(char, pZ3); // pZ3 �ߺ�����
	_MemRelease(char, pZ4);

	printAlloc();
	
	/* test
	int *p = new int;
	//int *p2 = alloc<int>(2);

	*p = 1;

	cout << *p << endl;

	delete p;
	//delete[]p2;

	cout << *p << endl;

	*p = 0; // �� ���ٰ���?

	cout << *p << endl;

	//*/

	return 0;
}

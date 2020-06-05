
#include <iostream>
#include <ctime>
#include "CMyNew.h"

#undef new new(__FILE__, __LINE__)

// define
#define MAX_INFO 20

enum {
	DONE,
	NOALLOC,
	ARRAY
};

// typedef
typedef struct allocInfo {

	void *ptr;
	bool isArray;	 // true : 배열, false : 배열 X
	int size;
	char file[100];
	int line;

} INFO;

// class
class CMyNew {

public:

	CMyNew();
	~CMyNew();
	
private:

	INFO info[MAX_INFO];
	static int infoCnt;

	char fileName[50];
	FILE *logFile;
	
	bool newData(void *ptr, bool isArray, int size, const char *File, int Line);
	int deleteData(void *ptr, bool isArray);

	friend void *operator new(size_t sz, const char *File, int Line);
	friend void *operator new[](size_t sz, const char *File, int Line);

	friend void operator delete(void *p);
	friend void operator delete[](void *p);
};

CMyNew::CMyNew() {

	errno_t errCheck;

	time_t base_time = time(NULL);
	struct tm base_date_local;

	localtime_s(&base_date_local, &base_time);

	sprintf_s(fileName, "Alloc_%02d%02d%02d_%02d%02d%02d.txt", 
		(base_date_local.tm_year - 100),
		(base_date_local.tm_mon + 1),
		base_date_local.tm_yday,
		base_date_local.tm_hour,
		base_date_local.tm_min,
		base_date_local.tm_sec);

	errCheck = fopen_s(&logFile, fileName, "a");
	if (errCheck != 0) {

		printf("%s", "file open error\n");
	}

	fclose(logFile);

	for (int i = 0; i < MAX_INFO; i++) {

		info[i].ptr = NULL;
	}
}

CMyNew::~CMyNew() {
	
	errno_t errCheck;
	char tmp[150];
	int len;

	memset(tmp, '\0', sizeof(tmp));

	// print 아직 안풀린 애들
	for (int i = 0; i < MAX_INFO; i++) {

		if (info[i].ptr != NULL) {

			errCheck = fopen_s(&logFile, fileName, "a");
			if (errCheck) {
				printf("%s", "file open error\n");
				break;
			}
			len = sprintf_s(tmp, "LEAK    [0x%08x] [%08d byte] %s : %d line\n", info[i].ptr, info[i].size, info[i].file, info[i].line);
			
			fwrite(tmp, sizeof(char), len, logFile);
			fclose(logFile);
		}
	}
}

// true : fail, false : success
bool CMyNew::newData(void *ptr, bool isArray, int size, const char *File, int Line) {

	int i;

	if (infoCnt >= MAX_INFO) return true;

	for (i = 0; i < MAX_INFO; i++) {

		if (info[i].ptr == NULL) break;
	}

	info[i].ptr = ptr;
	info[i].isArray = isArray;
	info[i].size = size;
	strcpy_s(&info[i].file[0], sizeof(info[i].file), File);
	info[i].line = Line;
	infoCnt++;

	return false;
}

// true : success, false : fail
int CMyNew::deleteData(void *pptr, bool isArray) {

	// isArray : true, info[i].isArray : true,
	int i;
	int *plus4byte = (int *)pptr;
	int *minus4byte = (int *)pptr;
	int state = NOALLOC;
	errno_t errCheck;
	char tmp[100];
	int len;

	plus4byte++;
	minus4byte--;
	memset(tmp, '\0', sizeof(tmp));

	for (i = 0; i < MAX_INFO; i++) {
		
		// new/delete, new[]/delete[] 호출시
		if (info[i].ptr == pptr && info[i].isArray == isArray) {

			state = DONE;
			break;
		}
		// new/delete[] 호출시
		else if (info[i].ptr == plus4byte && info[i].isArray != isArray) {

			state = ARRAY;
			break;
		}
		// new[]/delete 호출시
		else if (info[i].ptr == minus4byte && info[i].isArray != isArray) {

			state = ARRAY;
			break;
		}
	}

	switch (state) {

	case DONE: 
		info[i].ptr = NULL;
		infoCnt--;
		return DONE;
		break;

	case NOALLOC:
		errCheck = fopen_s(&logFile, fileName, "a");
		if (errCheck) {
			printf("%s", "file open error\n");
			break;
		}
		len = sprintf_s(tmp, "NOALLOC [0x%08x]\n", info[i].ptr);
		fwrite(tmp, sizeof(char), len, logFile);
		fclose(logFile);
		return NOALLOC;
		break;

	case ARRAY:
		errCheck = fopen_s(&logFile, fileName, "a");
		if (errCheck) {
			printf("%s", "file open error\n");
			break;
		}
		len = sprintf_s(tmp, "ARRAY   [0x%08x] [%08d byte] %s : %d line\n", info[i].ptr, info[i].size, info[i].file, info[i].line);
		
		fwrite(tmp, sizeof(char), len, logFile);
		fclose(logFile);

		info[i].ptr = NULL;

		return ARRAY;
		break;

	default:
		break;
	}
}

// 전역변수 초기화
int CMyNew::infoCnt = 0;

// 전역 인스턴스 선언
CMyNew myNew;

//* new / delete overloading
void *operator new (size_t sz, const char *File, int Line) {

	void *ptr = malloc(sz);
	
	if (myNew.newData(ptr, false, sz, File, Line)) {

		printf("ERROR_NEW_OVERFLOW\n");
		return NULL;
	}
	
	return ptr;
}

void *operator new[] (size_t sz, const char *File, int Line) {
	
	void *ptr = malloc(sz);
	
	if (myNew.newData(ptr, true, sz, File, Line)) {

		printf("ERROR_NEW_OVERFLOW\n");
		return NULL;
	}

	return ptr;
}

void operator delete (void *p, const char *File, int Line) {}
void operator delete[] (void *p, const char *File, int Line) {}

void operator delete(void *p) {

	// true : 주소 -4 해주고 free, false : 그대로 free 
	int state = -1;
	int *tmp;
	state = myNew.deleteData(p, false);
	
	switch (state) {

	case DONE:
		free(p);
		break;
	case ARRAY:
		tmp = (int *)p;
		tmp--;
		p = (void *)tmp;
		free(p);
		break;
	case NOALLOC:
		break;
	default:
		break;
	}
}

void operator delete[] (void *p) {

	// true : 주소 +4 해주고 free, false : 그대로 free 
	int state = -1;
	int *tmp;
	state = myNew.deleteData(p, true);

	//std::cout << "in" << std::endl;

	switch (state) {

	case DONE:
		free(p);
		break;
	case ARRAY:
		tmp = (int *)p;
		tmp++;
		p = (void *)tmp;
		free(p);
		break;
	case NOALLOC:
		break;
	default:
		break;
	}
}

//*/


/*////////////
   일반함수
////////////*/
/*
void printAlloc() {

	// make total alloc size
	int totalSize = 0;

	for (int i = 0; i < infoCnt; i++) {

		totalSize += info[i].size;
	}

	// 출력부
	printf("Total Alloc Size : %d\n", totalSize);
	printf("Total Alloc Count : %d\n\n", infoCnt);

	for (int i = 0; i < MAX_INFO; i++) {

		// 해제되지 않은 포인터가 남아있다면
		if (info[i].ptr != NULL) {

			printf("Not Release Memory [0x%x] %dbyte \n", info[i].ptr, info[i].size);
			printf("File : %s : %dline \n\n", info[i].file, info[i].line);
		}
	}
}

template<typename type>
type *memAlloc(int size, const char *str, int i) {

	type *p = new type[sizeof(type) * size];

	// INFO struct에 데이터 저장
	info[infoCnt].ptr = (void *)p;
	info[infoCnt].size = sizeof(type) * size;
	strcpy_s(&info[infoCnt].file[0], sizeof(info[infoCnt].file), str);
	info[infoCnt].line = i;
	infoCnt++;

	return p;
}

template<typename T>
void memRelease(T *p, const char *file, int line) {

	for (int i = 0; i < MAX_INFO; i++) {

		// 해당 포인터(p) 정보가 INFO에 있다면
		if (info[i].ptr == p) {

			info[i].ptr = NULL;
			delete[] p;
			return;
		}
	}

	// 없다면
	printf("wrong access in %s : %d\n\n", file, line);
	return;
}
//////////////////////////////////////////////////////////
		if (info[i].ptr != pptr) continue;

		if (info[i].isArray == isArray) {

			info[i].ptr = NULL;
			infoCnt--;

			// DONE
			state = DONE;
			break;
		}
		else if (info[i].isArray != isArray) {

			// ARRAY
			state = ARRAY;
			break;
		}
//*/
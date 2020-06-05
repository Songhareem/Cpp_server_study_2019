
// 과제2. 퀵 정렬

#include<stdio.h>
#include<windows.h>

int quick(int left, int right);

//배열 선언
int arr[10] = { 6,2,5,7,1,3,8,0,9,4 };

int main() {

	// 퀵소트 함수
	quick(0, 9);
	
	printf("\n");

	return 1;
}

// 퀵소트 함수 (재귀함수)
int quick(int left, int right) {

	printf("left : %d, right : %d\n", left, right);

	// 첫번째 위치를 피봇 설정
	int set = left;
	int pivot = arr[set];
	//printf("pivot : %d\n", pivot);

	// left, right, 중간 지점 지정
	int sp = left+1;
	int ep = right;
	int tmp;

	// 배열 보기
	for (int i = 0; i < 10; i++) printf("%d ", arr[i]);
	printf("\n");

	// 무한루프
	while (1) {

		// left 가 피봇보다 크지 않고, left <=right left 주소 + 1
		if (arr[sp] < pivot && sp < ep) sp++;
		printf("sp : %d ", sp);

		// right 가 피봇보다 작지 않고, 중간 지점이 아니라면, right 주소 - 1
		if (arr[ep] > pivot && sp <= ep) ep--;
		printf("ep : %d\n", ep);

		// right가 left보다 작다면 피봇과 위치 교체 후, break;
		if (ep <= sp && arr[set] > arr[ep]) {

			tmp = arr[set];
			arr[set] = arr[ep];
			arr[ep] = tmp;

			break;
		}
		else if (ep <= sp) {

			break;
		}

		// left가 피봇보다 크고, right 가 피봇보다 작다 둘이 교체
		if (arr[sp] > pivot && arr[ep] < pivot) {
		
			printf("arr[%d] : %d, arr[%d] : %d\n", sp, arr[sp], ep, arr[ep]);
			tmp = arr[sp];
			arr[sp] = arr[ep];
			arr[ep] = tmp;
		}
		
		// 배열 보기
		for (int i = 0; i < 10; i++) printf("%d ", arr[i]);
		printf("\n");

		Sleep(500);
	}
	
	// 배열 보기
	for (int i = 0; i < 10; i++) printf("%d ", arr[i]);
	printf("\n\n");

	// 정렬 후, left+1이 right가 같거나(배열이 2개) 크다면(배열 1개) 재귀종료 
	if (left + 1 >= right) return 0;

	// 중간 값 기준으로 두 구간으로 나누어서 재귀
	quick(left, ep-1);
	quick(ep, right);
	
	return 1;
}
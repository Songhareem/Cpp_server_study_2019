
// ����2. �� ����

#include<stdio.h>
#include<windows.h>

int quick(int left, int right);

//�迭 ����
int arr[10] = { 6,2,5,7,1,3,8,0,9,4 };

int main() {

	// ����Ʈ �Լ�
	quick(0, 9);
	
	printf("\n");

	return 1;
}

// ����Ʈ �Լ� (����Լ�)
int quick(int left, int right) {

	printf("left : %d, right : %d\n", left, right);

	// ù��° ��ġ�� �Ǻ� ����
	int set = left;
	int pivot = arr[set];
	//printf("pivot : %d\n", pivot);

	// left, right, �߰� ���� ����
	int sp = left+1;
	int ep = right;
	int tmp;

	// �迭 ����
	for (int i = 0; i < 10; i++) printf("%d ", arr[i]);
	printf("\n");

	// ���ѷ���
	while (1) {

		// left �� �Ǻ����� ũ�� �ʰ�, left <=right left �ּ� + 1
		if (arr[sp] < pivot && sp < ep) sp++;
		printf("sp : %d ", sp);

		// right �� �Ǻ����� ���� �ʰ�, �߰� ������ �ƴ϶��, right �ּ� - 1
		if (arr[ep] > pivot && sp <= ep) ep--;
		printf("ep : %d\n", ep);

		// right�� left���� �۴ٸ� �Ǻ��� ��ġ ��ü ��, break;
		if (ep <= sp && arr[set] > arr[ep]) {

			tmp = arr[set];
			arr[set] = arr[ep];
			arr[ep] = tmp;

			break;
		}
		else if (ep <= sp) {

			break;
		}

		// left�� �Ǻ����� ũ��, right �� �Ǻ����� �۴� ���� ��ü
		if (arr[sp] > pivot && arr[ep] < pivot) {
		
			printf("arr[%d] : %d, arr[%d] : %d\n", sp, arr[sp], ep, arr[ep]);
			tmp = arr[sp];
			arr[sp] = arr[ep];
			arr[ep] = tmp;
		}
		
		// �迭 ����
		for (int i = 0; i < 10; i++) printf("%d ", arr[i]);
		printf("\n");

		Sleep(500);
	}
	
	// �迭 ����
	for (int i = 0; i < 10; i++) printf("%d ", arr[i]);
	printf("\n\n");

	// ���� ��, left+1�� right�� ���ų�(�迭�� 2��) ũ�ٸ�(�迭 1��) ������� 
	if (left + 1 >= right) return 0;

	// �߰� �� �������� �� �������� ����� ���
	quick(left, ep-1);
	quick(ep, right);
	
	return 1;
}
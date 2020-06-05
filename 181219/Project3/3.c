
// 과제3 간단한 번역기 구현

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX 128

int translator(char *buf, char **dic[20]);

int main() {

	int i;
	int j;
	int result;

	char dic[10][2][20] = { {"i","나"},
							{"me","나를"},
							{"my","나의"},
							{"you","너"},
							{"your","너의"},
							{"love","사랑한다"},
							{"like","좋아한다"},
							{"food","음식"},
							{"car","차"},
							{"game","게임"} };

	char *test = "love";
	char *pdic = &dic[0][0][0];
	char *reader;
	char *ptmp;

	char buf[MAX];
	char tmp[3][10];
	memset(buf, 0, sizeof(buf));
	memset(tmp, 0, sizeof(tmp));

	reader = buf;
	ptmp = &tmp[0][0];

	puts("공백 포함 127자 이하 영문을 적으시오");
	gets_s(buf, sizeof(buf));

	// 삼차원 배열 인자 문제?
	//translator(buf, dic);


	i = 0;
	while (*reader != 0) {

		if (*reader == 0x20) *reader = '\0';
		reader++;
	}

	reader = buf;
	while (*reader != 0) {

		*ptmp = *reader;
		ptmp++;
		reader++;
		if (*reader == '\0') {

			*ptmp = *reader;
			ptmp = &tmp[++i][0];
			reader++;
		}
	}

	/*
	for (i = 0; i < 3; i++) {
		
		for (j = 0; j < 10; j++) {

			printf("%c", tmp[i][j]);
		}
		printf("\n");
	}
	*/

	for (j = 0; j < 3; j++) {

		for (int i = 0; i < 10; i++) {

			pdic = &dic[i][0][0];

			result = strcmp(&tmp[j][0], pdic);
			//result 0 = 같을때
			if (result == 0) {

				pdic = &dic[i][1][0];
				
				while (*pdic != '\0') {

					printf("%c", *pdic);
					pdic++;
				}

				printf(" ");

				//다음 단어로 번역으로 이행
				break;
			}
		}
	}
	//*/

}

int translator(char *buf, char **dic[20]) {
	
	int i;
	int j;
	int cnt=0;
	int result;

	char *reader = buf;
	char *pdic;

	while (*reader != 0) {

		reader++;
		if (*reader == '\0' || *reader == 0x20) {
			
			cnt++;
		}
	}

	//printf("%d\n", cnt);
	reader = &buf[0];

	//while (*reader != '\0' || *reader != 0x20) {

		for (i = 0; i < 10; i++) {

			pdic = &dic[i][0][0];
			
			puts(pdic);
			/*
			result = strcmp(reader, pdic);

			if (result == 0) {

				pdic = &dic[i][1][0];
				printf("%s ", *pdic);

				break;
			}
			*/
		}

	//	reader++;
	//}

	return 1;
}
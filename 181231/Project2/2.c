
// 과제2. 이미지 읽어서 합치기
/*
typedef struct tagBITMAPFILEHEADER {
  WORD  bfType;
  DWORD bfSize;
  WORD  bfReserved1;
  WORD  bfReserved2;
  DWORD bfOffBits;
} BITMAPFILEHEADER, *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;
  LONG  biWidth;
  LONG  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG  biXPelsPerMeter;
  LONG  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;
*/

#include<stdio.h>
#include<Windows.h> // BITMAPFILEHEADER, BITMAPINFOHEADER

int main() {

	// for test
	WORD *pw;

	// open for read and write
	FILE *fdesert = fopen("desert.bmp", "rb");
	FILE *ftulips = fopen("tulips.bmp", "rb");
	FILE *fnew = fopen("new", "wb");
	byte rgb_d[4] = { 0, };
	byte rgb_t[4] = { 0, };
	int ep;
	int tmp;

	BITMAPFILEHEADER dbfh;
	BITMAPINFOHEADER dbih;
	RGBQUAD	drgb;

	BITMAPFILEHEADER tbfh;
	BITMAPINFOHEADER tbih;
	RGBQUAD	trgb;

	fseek(fdesert, 0, SEEK_END);
	ep = ftell(fdesert);
	//printf("ep : %x\n", ep);

	fseek(fdesert, 0, SEEK_SET);

	// desert.bmp HEADER read
	fread(&dbfh, sizeof(BITMAPFILEHEADER), 1, fdesert);
	fread(&dbih, sizeof(BITMAPINFOHEADER), 1, fdesert);
	fread(&drgb, sizeof(RGBQUAD), 1, fdesert);
	
	// desert.bmp HEADER read
	fread(&tbfh, sizeof(BITMAPFILEHEADER), 1, ftulips);
	fread(&tbih, sizeof(BITMAPINFOHEADER), 1, ftulips);
	fread(&trgb, sizeof(RGBQUAD), 1, ftulips);
	
	fwrite(&dbfh, sizeof(BITMAPFILEHEADER), 1, fnew);
	fwrite(&dbih, sizeof(BITMAPINFOHEADER), 1, fnew);
	fwrite(&drgb, sizeof(RGBQUAD), 1, fnew);

	while (ftell(fdesert) < ep) {

		fread(rgb_d, sizeof(rgb_d), 1, fdesert);
		fread(rgb_t, sizeof(rgb_t), 1, ftulips);

		for (int i = 0; i < 3; i++) {
			tmp = (rgb_d[i] + rgb_t[i]) / 2;
			rgb_d[i] = tmp;
		}

		fwrite(rgb_d, sizeof(rgb_d), 1, fnew);
	}

	/* 왜 안돼는거..???
	pw = &dbfh;
	for (int i = 0; i < 7; i++) {
		printf("%x ", *pw);
		pw++;
	}

	printf("\n");

	fseek(fnew, 0, SEEK_SET);
	//printf("%x\n", fnew);
	
	pw = (WORD)fnew;
	printf("%x\n", pw);

	for (int i = 0; i < 7; i++) {
		printf("%x ", *pw);
		pw++;
	}
	//*/

	/* 사이즈 왜 안맞지..?
	for (int i = 1; i <= dbih.biHeight; i++) {

		for (int j = 1; j <= dbih.biWidth; j++) {

			fread(rgb, sizeof(rgb), 1, fdesert);

			//for (int i = 0; i < 4; i++) printf("%x ", rgb[i]);
			//printf("\n");

			fwrite(rgb, sizeof(rgb), 1, fnew);
		
		}
	}
	//*/

	// close
	fclose(fdesert);
	fclose(ftulips);
	fclose(fnew);

	return 0;
}
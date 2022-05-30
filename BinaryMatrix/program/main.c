#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "general.h"


void main()
{
	int* image1;
	int row, col;

	image1 = getImage("mat1.bin", &row, &col);
	printMat(image1, row, col);
	saveCompressImageToFile("mat1_comp.bin", image1, row, col);
	
	int* image2 = getImage_compress("mat1_comp.bin", &row, &col);
	printMat(image2, row, col);

}

int*	getImage(const char* fileName, int* pRow, int* pCol)
{
	FILE* fp = fopen(fileName, "rb");
	if (!fp)
		return NULL;
	if (fread(pRow, sizeof(int), 1, fp) != 1)
	{
		fclose(fp);
		return NULL;
	}
	if (fread(pCol, sizeof(int), 1, fp) != 1)
	{
		fclose(fp);
		return NULL;
	}
	int matSize = *pCol*(*pRow);
	int* mat = (int*)malloc(sizeof(int)*matSize);
	if (!mat)
	{
		fclose(fp);
		return NULL;
	}
	if (fread(mat, sizeof(int), matSize, fp) != matSize)
	{
		free(mat);
		fclose(fp);
		return NULL;
	}
	fclose(fp);
	return mat;
}

int * getImage_compress(const char * fileName, int * pRow, int * pCol)
{
	FILE* fp = fopen(fileName, "rb");
	if (!fp)
		return NULL;
	if (fread(pRow, sizeof(int), 1, fp) != 1)
	{
		fclose(fp);
		return NULL;
	}
	if (fread(pCol, sizeof(int), 1, fp) != 1)
	{
		fclose(fp);
		return NULL;
	}
	size_t mat_size = *pRow * (*pCol);
	int byteCount = (int) mat_size / 8;
	int flag = 0;
	if (byteCount * 8 < mat_size)
	{
		byteCount++;
		flag = 1;
	}

	BYTE* data = (BYTE*)malloc(sizeof(BYTE)*byteCount);
	if (!data)
	{
		fclose(fp);
		return NULL;
	}
	if (fread(data, sizeof(BYTE), byteCount, fp) != byteCount)
	{
		free(data);
		fclose(fp);
		return NULL;
	}
	fclose(fp);

	int* mat = (int*)calloc(mat_size, sizeof(int));
	if (!mat)
	{
		free(data);
		fclose(fp);
		return NULL;
	}
	for (int i = 0; i < byteCount; i++)
	{	
		for (int j = 0; (j < 8) && (i * 8 + j)< mat_size; j++)
		{
			mat[i * 8 + j] = (data[i] >> (7 - j)) & 0x1;
		}
	}
	free(data);
	return mat;
}

Rect*	createRectArr(const int* image, int row, int col, int* pCount)
{
	Rect* rectArr = NULL;
	int resNew, count = 0;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			resNew = 0;
			if (*(image + (i*col + j)) == 1)
				resNew = checkNewRect(image, col, i, j);
			if (resNew == 1)
			{
				rectArr = (Rect*)realloc(rectArr, sizeof(Rect)*(count + 1));
				rectArr[count] = *createsingleRect(image, row, col, i, j);
				count++;
			}
		}
	}
	*pCount = count;
	return rectArr;
}

int		saveRectToFile(const char* fileName, Rect* rectArr, int count)
{
	FILE* fp = fopen(fileName, "w");
	if (!fp)
		return 0;
	fprintf(fp, "%d\n", count);
	for (int i = 0; i < count; i++)
	{
		fprintf(fp, "%d\t%d\t%d\t%d\n", rectArr[i].upperLeftX, rectArr[i].upperLeftY,
			rectArr[i].width, rectArr[i].height);
	}
	fclose(fp);
	return 1;
}

int		saveCompressImageToFile(const char* fileName, const int* image, int row, int col)
{
	FILE* fp = fopen(fileName, "wb");
	if (!fp)
		return 0;
	if (fwrite(&row, sizeof(int), 1, fp) != 1)
	{
		fclose(fp);
		return 0;
	}
	if (fwrite(&col, sizeof(int), 1, fp) != 1)
	{
		fclose(fp);
		return 0;
	}
	int byteCount = row * col / 8;
	if (byteCount * 8 < row*col)
		byteCount++;

	BYTE* data = (BYTE*)calloc(sizeof(BYTE), byteCount);
	if (!data)
	{
		fclose(fp);
		return 0;
	}

	int dataPos;
	int dataShift;
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			dataPos = (i * col + j) / 8;
			dataShift = 7 - (i* col + j) % 8;
			data[dataPos] |= (*(image + i * col + j)) << dataShift;
		}
	}
	if (fwrite(data, sizeof(BYTE), byteCount, fp) != byteCount)
	{
		free(data);
		fclose(fp);
		return 0;
	}
	fclose(fp);
	free(data);
	return 1;
}

int checkNewRect(const int * image, int col, int i, int j)
{
	if (i == 0 && j == 0) // top corner
		return 1;
	if (i == 0 && *(image + j - 1) == 0) // top row
		return 1;
	else if (j == 0 && *(image + (i - 1)*col) == 0) // left col
		return 1;
	else if (*(image + (i*col + j - 1)) == 0 && *(image + ((i - 1)*col + j)) == 0)
		return 1;
	return 0;
}

Rect * createsingleRect(const int * image, int row, int col, int i, int j)
{//func assuming that the index is a new rect ! checked before!
	Rect* pRes = (Rect*)malloc(sizeof(Rect));
	if (!pRes)
		return NULL;
	pRes->upperLeftX = i;
	pRes->upperLeftY = j;
	pRes->height = 1;
	pRes->width = 1;
	for (j += 1; j < col; j++)
	{
		if (*(image + (i*col + j)) == 1)
			pRes->width++;
		else
			break;
	}
	j = pRes->upperLeftY;
	for (i += 1; i < row; i++)
	{
		if (*(image + (i*col + j)) == 1)
			pRes->height++;
		else
			break;
	}
	return pRes;
}

void printMat(const int * mat, int row, int col)
{
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			printf("%d ", *(mat + (i*col + j)));
		}
		printf("\n");
	}
}

void printRect(const Rect * pRect)
{
	printf("Rect starts in [%d][%d]\n width : %d , height : %d \n",
		pRect->upperLeftX, pRect->upperLeftY, pRect->width, pRect->height);
}


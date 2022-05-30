#pragma once
typedef unsigned char BYTE;

int*	getImage(const char* fileName, int* pRow, int* pCol);
int*	getImage_compress(const char* fileName, int* pRow, int* pCol);
int		saveCompressImageToFile(const char* fileName, const int* image, int row, int col);
void	printMat(const int* mat , int row ,int col);


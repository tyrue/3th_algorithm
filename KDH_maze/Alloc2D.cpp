#pragma once
#include <cstdio>

int** alloc2DInt (int rows, int cols)	// 동적할당 하여 배열의 크기를 정해준다.
{	
	if ( rows <= 0 || cols <= 0) return NULL;	// 만약 음수면 할당 X
	int** mat = new int* [rows];		// 열 동적할당(세로)
	for (int i=0; i<rows; i++) 
		mat[i] = new int[cols];			// 행 동적할당(가로)
	
	return mat;							// 배열을 리턴한다.
}

void free2DInt(int** mat, int rows, int cols) 	// 동적할당 해제.
{
	if (mat != NULL) 
	{
		for (int i=0; i<rows; i++) 
			delete[] mat[i];
		delete[] mat;
	}
}

#pragma once
#include <cstdio>

int** alloc2DInt (int rows, int cols)	// �����Ҵ� �Ͽ� �迭�� ũ�⸦ �����ش�.
{	
	if ( rows <= 0 || cols <= 0) return NULL;	// ���� ������ �Ҵ� X
	int** mat = new int* [rows];		// �� �����Ҵ�(����)
	for (int i=0; i<rows; i++) 
		mat[i] = new int[cols];			// �� �����Ҵ�(����)
	
	return mat;							// �迭�� �����Ѵ�.
}

void free2DInt(int** mat, int rows, int cols) 	// �����Ҵ� ����.
{
	if (mat != NULL) 
	{
		for (int i=0; i<rows; i++) 
			delete[] mat[i];
		delete[] mat;
	}
}

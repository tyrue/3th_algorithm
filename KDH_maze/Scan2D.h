/* ������ ������ ���� */

#pragma once
#include <cstdio>

class Scan2D		// ��ĵ 
{
public:
	int row;
	int col;
	Scan2D();
	Scan2D(int r = 0 , int c = 0) { 
		row = r;
		col = c;
	}
	~Scan2D(void) { }
};
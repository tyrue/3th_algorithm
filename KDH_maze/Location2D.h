/* ������ ������ ���� */
#pragma once
#include <cstdio>

class Location2D			// ��ġ
{
public:
	int row;
	int col;
	Location2D();
	Location2D(int r = 0 , int c = 0) { 
		row = r;
		col = c;
	}
	~Location2D(void) { }
};
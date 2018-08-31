/* 저장할 스택의 형식 */

#pragma once
#include <cstdio>

class Scan2D		// 스캔 
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
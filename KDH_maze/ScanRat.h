/* 저장할 스택의 형식 */

#pragma once
#include <cstdio>

class ScanRat			// 스캔 쥐(스캔을 뿌린곳에 가서 지도를 3x3 밝혀줌)
{
public:
	int row;			// 행
	int col;			// 열
	ScanRat();			// 생성자
	ScanRat(int r = 0 , int c = 0) 
	{ 
		row = r;
		col = c;
	}
	~ScanRat(void) { }	// 소멸자
};
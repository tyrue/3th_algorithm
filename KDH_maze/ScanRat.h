/* ������ ������ ���� */

#pragma once
#include <cstdio>

class ScanRat			// ��ĵ ��(��ĵ�� �Ѹ����� ���� ������ 3x3 ������)
{
public:
	int row;			// ��
	int col;			// ��
	ScanRat();			// ������
	ScanRat(int r = 0 , int c = 0) 
	{ 
		row = r;
		col = c;
	}
	~ScanRat(void) { }	// �Ҹ���
};
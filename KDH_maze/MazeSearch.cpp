#include "Location2D.h"			// ��ġ
#include "Scan2D.h"				// ��ĵ
#include "ScanRat.h"			// ��ĵ ��
#include <Windows.h>			// �ܼ� �����
#include <stack>				// ����
using namespace std;			// �̸� ���� ���

#define UP_RIGHT   0			
#define UP_LEFT	   1
#define DOWN_RIGHT 2
#define DOWN_LEFT  3
#define RIGHT_UP   4
#define RIGHT_DOWN 5
#define LEFT_UP    6
#define LEFT_DOWN  7			// �켱������ ��ũ��

extern int** alloc2DInt( int rows, int cols);				// �ܺ� �Լ� ��������
extern void free2DInt (int** mat, int rows, int cols=0);

// ��ġ ����
int**	map  = NULL;		// ���� ������( �̷� ǥ����)
int		rows = 1;		
int		cols = 0;			// �̷��� ��� ��

// ���� ���� ����
int		initEnergy  = 0;	// ���� ü��
int		mazeMove	= 0;	// �̵�Ƚ��
double	mana		= 0;	// ���� ����

// ��ų ����
int     scan_count	 = 0;	  // ��ų ī��Ʈ(���Ƚ��)
int		bunker_count = 0;
bool    scan_avail	 = FALSE; // ��ĵ ��� ���� 

// �Լ� ȣ��
void loadMazeFile();	// �̷� ���� �������� �Լ�
void printMaze(stack<Location2D> here, stack<ScanRat>& rat, int r, int c, int mode); // �̷θ� �ֿܼ� ǥ�����ִ� �Լ�
void selectMode(char* mode);
void mazeSearch(stack<Location2D> locStack, stack<ScanRat>& ScanRatStack, char mode);

bool isValidLoc(int r, int c);
bool isValidS(int r,int c);
bool isExit(int r, int c);

void main() 
{
	char mode;

	loadMazeFile();
	selectMode(&mode);

	stack<Location2D>	locStack;		// ��ġ ����
	stack<ScanRat>		ScanRatStack;	// ��ĵ�� ����

	Location2D entry(0, 1);				// -> �������� 0, 1
	locStack.push(entry);				// ��ġ ���ÿ� ó����ġ(0.1)�� �ִ´�.

	//	printMaze(locStack,0,1,mode);
	//	getchar();
	mazeSearch(locStack, ScanRatStack, mode);
}

bool isValid(int r, int c)	//  �� ���� ��ǥ���� Ȯ��
{
	if ( r < 0 || c < 0 || r >= rows || c >= cols ) return FALSE;		// ���� ���̸� �ȵ�
	else return TRUE;
}

bool isValidLoc(int r, int c)	// �� �� �ִ� ��ġ���� Ȯ��
{
	if (isValid(r, c)) return (map[r][c] == 0 || map[r][c] == 4 || map[r][c] == 5);	// ���� �ش� ��ġ�� 0�Ǵ� 4�Ǵ� 5�̸� �� �� �ִ�.
	else  return false;	 		// ���� ���̸� �ȵ�
}

bool isValidS(int r, int c)		// ��ĵ �Ѹ� �� �ִ� ������ Ȯ��
{
	if (isValid(r, c)) return map[r][c] == 5 ;// �ش� ��ġ�� 5�̸� ������
	else return false; 
}

bool isExit(int r, int c)	// �ⱸ�� ã�� �Լ�
{
	if(isValid(r,c) && (r == 0 || r == rows-1 || c == 0 || c == cols-1) && (r != 0 && c != 1)) // �ʾ��� ��ǥ�̰�, �Ա��� �ƴϸ�,
	{
		return	(map[r][c] == 0 || map[r][c] == 4);	// ��躮�ʿ� �ִ� ��ǥ��� �ⱸ			  		
	}
	else return FALSE;
}

bool isWall(int r, int c) 
{
	if(isValid(r, c))	return (map[r][c] == 1);	// Ȯ�� �ȵ� ��ǥ�� ���̾���.
	else	return false;
}

/*****************��������� �ǵ�� ���� ******************/
bool isClose(int r, int c) // ���� ������ Ȯ���ϴ� �Լ�, ���ݱ��� Ȯ���� ���� ���� ���� ������ Ȯ����
{
	int count = 0;
	
	int a[] = {0, -1, 0, 1};
	int b[] = {1, 0, -1, 0};

	for(int i = 0; i < 4; i++)
	{
		int ra = r + a[i], cb = c + b[i];
		if(isValid(ra, cb) || (ra == 0 && cb == 1)) if(map[ra][cb] == 2 || map[ra][cb] == -1) count++;
	}

	if(count >= 3) return TRUE;
	else return FALSE;
}

void selectMode(char* mode)		// ��� ����(�ڵ�, ����)
{
	static int i = 0;	

	if( i == 0 )				// ó������ ��带 ������
	{
		printf("Select Mode ( A or M ) : ");
		scanf("%c", mode);
		i++;
	}
}

void printMaze(stack<Location2D> here, stack<ScanRat>& rat, int r, int c, int mode)	// �̷θ� �ֿܼ� ǥ�����ִ� �Լ�
// ���� ��ġ, ��ĵ��ġ, 
{
	system("cls");	// ȭ���� ������ �ְ� �����ֱ� ���ؼ� 
	printf( "=============================================\n");
	printf( " ��ü �̷��� ũ�� = %d %d\n", rows, cols);
	printf( " ������ = %d\n", initEnergy);
	printf( " �̵� = %d\n", mazeMove);
	printf( " ���� ��ġ = (%d, %d)\n", c, r);
	printf( " ���� ������ = %d\n", initEnergy-mazeMove); // ü���� �ѹ� �̵��� ������ 1�� �پ��
	printf( " ���� = %.1lf\n", mana);
	
	Scan2D s(0, 0);				// ��ĵ ��ǥ
	if(rat.size() != 0)			// ��ĵ ������ ��������� �ȵ�
	{	
		s.col = rat.top().col;
		s.row = rat.top().row;
	}
	printf( "\n ��ĵ �߽� ��ǥ = %d , %d", s.col, s.row);
	printf( "\n ��ĵ ��� Ƚ�� = %d", scan_count);
	printf( "\n ������ ��� Ƚ�� = %d", bunker_count);
	printf( "\n=============================================\n");
	
	for( int i = 0 ; i < rows; i++ ) 
	{
		for( int j = 0 ; j < cols; j++ )
		{	
			if(i == r && j == c)
			{
				printf("��");		// ���� ��ġ
				continue;
			}
			switch (map[i][j]) 
			{
				case -1 : printf("��");	break;	// ���� ��, ��Ŀ�����ͷ� ������ �ⱸ�� ���±�
				case 0  : printf("  "); break;  // ���� ������ ���� ��, �� �� �ִ� ��
				case 1  :						// ���� Ȯ�� ���� ��
					if( mode == 'e' )			// �ⱸ�� ã������ �� ��ü�� ������ ����
					{	
						printf( "��" );			
						break;
					}
					else 
					{
						printf("  ");			// �̷θ� Ž���߿� ���� �𸣹Ƿ� ����ó��
						break;
					}
				case 2  : printf("��");	break;	// �̹� Ȯ���� ��
				case 3  : printf("  "); break;	// Ȯ�ε� �� �� �ִ� ��
				case 4  : printf("��");	break;	// �ⱸ�� Ȯ���� ��
				case 5  : printf("��");	break;	// ��ĵ���� Ȯ���� ��ĵ�㰡 �� �� �ִ±�
				case 6  : printf("��");	break;	// 4�� ������ ��
				case 7  : printf("��");	break;	// 3�� ������ ��
				case 8  : printf("��");	break;	// 2�� ������ ��
				case 9  : printf("��");	break;	// 1�� ������ ��	
				case 10 : printf("��"); break;  // �ⱸ�� �̾����±�
			}
		} 
		printf("\n");				
	}

	if( (mode == 'm' || mode == 'M') && mode != 'e') {
		printf("/////////////////////////////////////////////\n");
		printf("���� ��ġ (pop�� ����)\n");
		printf("row = %d col = %d\n", r, c) ;
		printf("���� ��ġ�� ��(map[r][c]) = %d\n",map[r][c]);
		printf("���� ����\n");
		stack<Location2D> pos;
		pos = here;
		for(int i = 0; i < here.size(); i++) {
			Location2D pos2D = pos.top();
			printf("[rows = %d , cols = %d]\t", pos2D.row, pos2D.col);
			pos.pop();
		}
	}	
}

void loadMazeFile()		// �̷θ� �д� �Լ�
{	
	char ch;			// ���� ������ �б� ���� ���� �ӽ� ����.
	char filename[30];
	FILE *fp;			//= fopen(filename, "r");	// ������ ����.

	while(1) 
	{
		printf("���� �̸��� �Է��ϼ��� : ");
		gets_s(filename);
		fp = fopen(filename, "r");
		if ( fp == NULL ) 
		{
			printf(" ���� %s�� �� �� �����ϴ�.!!!\n", filename);
			continue;
		}
		break;
	}
	while(fscanf(fp, "%c", &ch) != EOF)		// �̷� txt������ �д´�, ������ ������ �ݺ��� ����
	{	
		if(ch == '\n') rows++;				// txt���Ͽ��� ���๮��(enter)�� ������ �ళ���� ����.
		//if(ch == '0' || ch == '1') cols++;	// txt���Ͽ��� ������ ������ ����.
		else if(ch != ' ') cols++;
	}
	cols = cols/rows;				// ���� �� = �� ������ ���� / ���� ���� 

	initEnergy = 2 * cols * rows;	// ���� ü�� = 2 * �� * ��

	map = alloc2DInt(rows,cols);	// map�� �����Ҵ� �޴´�.
	fseek(fp, 0L, SEEK_SET);		// 05.18 -> ���� �����͸� ó����ġ�� �̵���Ų��. 

	for(int i=0; i<rows; i++) 
	{
		for(int j=0; j<cols; j++) 
			fscanf(fp, "%d", &(map[i][j]));
	}
	fclose(fp);
}

/** 5. 21 ��ĵ �Լ� ����**/
void scan(bool &e, int &d, int r, int c, stack<ScanRat>& ScanRatStack) // ��ĵ �Լ�, ��ǥ�� �޾Ƽ� �̷� �ʿ� ��ĵ�� �Ѹ���. d = �ⱸ�� �ִ� ���� 
{
	if (e == FALSE)	//��ĵ�� �ⱸ�� ��ã���� ��
	{
		for(int i = r-1; i <= r+1; i++)
		{
			for(int j = c-1; j <= c+1; j++)			// ��ĵ �߽����� �������� ���� ��ġ�� ���� 8���� Ž	
			{
				if(isWall(i, j)) map[i][j] = 2;		// ���̸� ���ڸ� 2�� ��ȯ
				else if(isExit(i, j)) 
				{
					map[i][j] = 4;			// �ⱸ�� �ⱸ Ȯ��
					e = TRUE;				// �ⱸ ã���� ����
					ScanRatStack.push(ScanRat(i, j));	// �ⱸ ��ǥ�� ������
					
					// �ⱸ�� �ִ� ��ġ
					if(i == 0 && j <= cols / 2)			d = UP_LEFT;
					if(i == 0 && j >= cols / 2)			d = UP_RIGHT;
					if(i == rows - 1 && j <= cols / 2)	d = DOWN_LEFT;
					if(i == rows - 1 && j >= cols / 2)	d = DOWN_RIGHT;
					if(i <= rows / 2 && j == 0)			d = LEFT_UP;
					if(i >= rows / 2 && j == 0)			d = LEFT_DOWN;
					if(i <= rows / 2 && j == cols - 1)	d = RIGHT_UP;
					if(i >= rows / 2 && j == cols - 1)	d = RIGHT_DOWN;

					for(int k = 0; k < cols; k++)	// �ⱸ ã���� ������ ��躮�� 2�� ����
					{
						if(map[0][k] == 1) map[0][k] = 2;
						if(map[rows - 1][k] == 1) map[rows - 1][k] = 2;
					}
					for(int k = 0; k < rows; k++)
					{
						if(map[k][0] == 1) map[k][0] = 2;
						if(map[k][cols - 1] == 1) map[k][cols - 1] = 2;
					}
					break;
				}
			}
		}	
	}

	if (e == TRUE)	//��ĵ�� �ⱸ�� ã������
	{
		for(int i = r-1; i <= r+1; i++)
		{
			for(int j = c-1; j <= c+1; j++)	// ��ĵ �߽����� �������� ���� ��ġ�� ���� 8���� Ž��
			{
				if(isValid(i, j))
				{
					if(isWall(i, j)) map[i][j] = 2;	// ���̸�(1) ���ڸ� 2�� ��ȯ
					// ���� �ƴϰ�, �游 �շ� �ִٸ� �ⱸ�� �̾����� ��
					else if(map[i][j] == 0 || map[i][j] == 3) map[i][j] = 5; 
				}
			}
		}										
	}
}

/** 5.21 ��ĵ ��ǥ ���� �Լ� ���� **/
// ��ĵ ����Ʈ ��, ��, �ⱸ ã�Ҵ��� Ȯ��, ��ĵ �� ��ǥ����, ���� ��ġ, ��ĵ ��� ����
void scanLocation(int& r, int& c, bool& e, stack<ScanRat>& ScanRatStack, int hr, int hc)	// ��ĵ ��ǥ�� �������ִ� �Լ�
{
	static int d = 3;		// ���� ��������, ó���� ���� ������
	if (e == FALSE)			// ��ĵ�� �ⱸ�� ��ã���� �� �ϴ� ��
	{
		if (d == 0)			// �����ʰ�����, �̹� �������̶�� �Ѿ��.
		{
			for(int i = c; i <= cols - 2; i++)	// �̹� Ȯ���� ������ Ȯ���Ѵ�.
			{
				if(map[0][i] == 2) 	c = i + 1;	// Ȯ���� ��躮
				else if(map[0][i] == 1)		// ���� Ȯ�� ���� ���̸�
				{
					while(map[1][c] == 2)	// �������� ������ Ȯ��.
						c++;				// ���� ��
					c++;					// ���� Ȯ�� ���� ���� ��ġ
					break;
				}
			}
			if ( c >= cols - 2)				 // ���������� �ִ�ġ���� ���� ��� �Ʒ����̵�	
			{
				c = cols - 2;
				if(map[r][c] != 1)	d = 1;// ���� �Ʒ��� ���� ����			
			}
		}

		if (d == 1)		// �Ʒ��� ������
		{
			for(int i = r; i <= rows - 2; i++)
			{
				if(map[i][cols - 1] == 2) r = i + 1; // Ȯ���� ��躮
				else if(map[i][cols - 1] == 1)	// ���� Ȯ�� ���� ��
				{
					while(map[r][cols - 2] == 2)
						r++;
					r++;
					break;
				}
			}
			if (r >= rows - 2) 
			{		
				r = rows - 2;
				if(map[r][c] != 1)	d = 2;		// �ִ�ġ���� ���� ��� �������� ����		
			}
		}

		if (d == 2)		// �������� ������
		{
			for(int i = c; i >= 1; i--)
			{
				if(map[rows - 1][i] == 2) c = i - 1; // Ȯ���� ��躮
				else if(map[rows - 1][i] == 1)	
				{
					while(map[rows - 2][c] == 2)
						c--;
					c--;
					break;
				}
			}
			if (c <= 1) 
			{
				c = 1;				
				if(map[r][c] != 1) d = 3;		// �ִ�ġ���� ���� ��� �������� ����			
			}
		}

		if (d == 3)		 // ���� ������
		{
			for(int i = r; i >= 1; i--)
			{
				if(map[i][0] == 2) r = i - 1; // Ȯ���� ��躮
				else if(map[i][0] == 1)	
				{
					while(map[r][1] == 2)
						r--;
					r--;
					break;
				}
			}
			if (r <= 1)
			{
				r = 1;
				d = 0;	 // ������
			}						
		}
	}
	
	else //��ĵ�� �ⱸ�� ã������
	{
		//�㰡 �������� 0�̳� 1�̸� ����
		//���� Ž���˰����ε� '5'�� ���� ��
		//�������� 5 = Ȯ������ ����
		//���࿡ ���� ���� ���ƿ��� ���� ����(�̱���)
		static bool find = FALSE;			// �㰡 ������ ���� ã���� �� TRUE
		while (ScanRatStack.size() != 0)
		{
			ScanRat rathere = ScanRatStack.top();	
			ScanRatStack.pop();				// ������ ���� ����   
			int rat_r = rathere.row;		// ��� ���� ��ĵ���� ��ġ�� �ٲ�
			int rat_c = rathere.col;

			// �㰡 �ϴ� ��ĵ �Ѹ���, 5�� �ִ� ������ ������ ������ �ű⼭ ���߰� ��ĵ �Ѹ�
			if(map[rat_r][rat_c] == 2)		map[rat_r][rat_c] = 2;
			else if(isClose(rat_r, rat_c) || (rat_r == 0 && rat_c == 1))	map[rat_r][rat_c] = -1;	// ���� ���̸� -1�� ����� �ǵ��ƿ�

			else if(find == TRUE)			// �㰡 ������ ��� ������.
			{
				int a[] = {0, -1, 1, 0};
				int b[] = {-1, 0, 0, 1};	// ��, ��, ��, ��

				// ��ĵ�㰡 ������ ������ 4�� �ٲ���
				if(map[rat_r][rat_c] >= 5 && map[rat_r][rat_c] <= 9 || map[rat_r][rat_c] == 3)	map[rat_r][rat_c] = 4; 
				for(int i = 0; i < 4; i++)
				{
					int ra = rat_r + a[i], cb = rat_c + b[i]; // 4���� Ȯ��
					if(isValid(ra, cb))		// �ʾ��� ��ǥ������
					{					
						if(ra == hr && cb == hc) // ��ĵ��� �㰡 ������.
						{
							printf("%d %d", hr, hc);
							system("pause");
							map[ra][cb] = 4;
							scan_avail = FALSE;
							return; // ���� ������ġ�� ��ĵ�㰡 ���� ��ĵ ���� 
						}
						else if(map[ra][cb] >= 6 && map[ra][cb] <= 9 || map[ra][cb] == 3 ) // ��ĵ�㰡 �㰡 ������ ����ã����?
						{
							ScanRatStack.push(ScanRat(rat_r, rat_c)); // ���ƿ� �� �ְ� ���ڸ� ����
							ScanRatStack.push(ScanRat(ra, cb));						
						}
					}
				}
			}

			else if(find == FALSE)	// ���� �㰡 ������ ���� �Ȱ��� ��
			{
				int a[] = {0, 1, 0, -1};
				int b[] = {-1, 0, 1, 0};	// ��, ��, ��, ��
				if(map[rat_r][rat_c] >= 5 && map[rat_r][rat_c] <= 9 || map[rat_r][rat_c] == 3)	map[rat_r][rat_c] = 4;

				for(int i = 0; i < 4; i++)
				{
					int ra = rat_r + a[i], cb = rat_c + b[i];	 // 4���� Ȯ��
					if(isValid(ra, cb))							 // �ʾ��� ��ǥ������
					{
						if(map[ra][cb] >= 6 && map[ra][cb] <= 9 || map[ra][cb] == 3) // ��ĵ�㰡 �㰡 ������ ����ã����?
						{
							while(ScanRatStack.size() > 0)	ScanRatStack.pop();	// ������ �� ����.
							ScanRatStack.push(ScanRat(ra, cb));
							find = TRUE;	// ���� ��ĵ�ϴ� �� ���߰�, ������ġ�� ���� �� ���� ��� 4�� �����.
							break;
						}
						// ���� �㰡 �ٶ󺸴� �ʿ� Ȯ�� ���� ���� �ִٸ� �ű� ���� �����ϰ� ����
						else if (map[ra][cb] == 0 || map[ra][cb] == 1)	
						{					
							ScanRatStack.push(ScanRat(rat_r, rat_c));		// ���ƿ� �� �ְ� ���ڸ� ����							
							ScanRatStack.push(ScanRat(ra, cb));
							r = ra + a[i];
							c = cb + b[i];	// ��ĵ ����Ʈ ����
							return;
						}
						else if (isValidS(ra, cb))	// �ش� ��ġ�� 5���� Ȯ��
						{   
							ScanRatStack.push(ScanRat(rat_r, rat_c));		// ���ƿ� �� �ְ� ���ڸ� ����
							ScanRatStack.push(ScanRat(ra, cb));
						}
					}
				}
			}
		}
	}
}

// �׳� ��ǥ�� ���� �վ����(4��)
void bunkerBuster(int r, int c) 
{
	map[r][c] = 4;
}

// ���� �㸦 �������� 2ĭ�� ���µ�, �� + Ȯ�����̸� ���� �հ� �ٸ� ���� ���ƹ�����.
bool bunker_good(stack<Location2D>& locStack)	// ��Ŀ ������ ����ϱ� ������ �Ǵ�
{
	Location2D loc = locStack.top();	// ������ġ�� ����
	int r = loc.row;
	int c = loc.col;
	
	int a[] = {0, -1, 0, 1};
	int b[] = {-1, 0, 1, 0};

	for(int i = 0; i < 4; i++)
	{
		int ra = r + a[i], cb = c + b[i];
		int rra = ra + a[i], ccb = cb + b[i];	// 2ĭ ���� �����ؼ�
		if(isValid(ra, cb) && isValid(rra, ccb))
		{
			if(map[ra][cb] == 2 && map[rra][ccb] == 4) // Ȯ������ ���Ҳ����� ��
			{
				map[r][c] = -1;	// ���� ��ġ�� �ٽ� �Ȱ����� 10���� ����
				int tr = locStack.top().row;
				int tc = locStack.top().col;
				while(!(tr == rra && tc == ccb))	
				{
					map[tr][tc] = -1;
					locStack.pop();	
					if(locStack.size() == 0) break;
					tr = locStack.top().row;
					tc = locStack.top().col;
				}
				locStack.push(Location2D(ra, cb));		// ���� ���� ��ǥ�� �ִ´�.
				return TRUE;
			}
			else if(map[ra][cb] == 2 && map[rra][ccb] == -1 && map[r][c] != 10)
			{
				map[r][c] = 10;	// ���� ��ġ�� �ٽ� �Ȱ����� 10���� ����
				while(locStack.size() > 0 )	locStack.pop();			
				locStack.push(Location2D(ra, cb));	// ���� ���� ��ǥ�� �ִ´�.
				return TRUE;
			}
		}
	}
	return FALSE;
}
void mazeSearch(stack<Location2D> locStack, stack<ScanRat>& ScanRatStack, char mode)
// ��ġ, ��ĵ ��ġ, ��ĵ�� ��ġ, ���
{
	bool exit_find = FALSE;		// ��ĵ�� �ⱸ�� ã�Ҵ��� Ȯ���ϴ� ����
	scan_avail = TRUE;			// ��ĵ ��� ����

	int direction = 5;			// �ʱ� ������ ������ �Ʒ�
	int scan_row = rows - 2;
	int scan_col = 1;			// ��ĵ �߽� ����Ʈ

	while (locStack.empty() == false)		// ��ġ ������ �� �� ����(�ٽ� �Ա��� ���ƿ�)
	{
		Location2D here = locStack.top();	// ���� ��ġ�� ���� �� ��
		locStack.pop();						// ������ ���� ����	

		mazeMove++;							// pop�� �ѹ� �Ͼ������(=�ѹ� �̵��� ������) mazeMove�� ����
		mana = mana + 0.1;					// pop�� �ѹ� �Ͼ������(=�ѹ� �̵��� ������) mana�� 0.1 ����

		int r = here.row;					// ���� ��ġ ��
		int c = here.col;					// ���� ��ġ ��	

		/**************** ������ ����(�⺻ Ʋ, �̷θ� Ž���ϰ� �ⱸ�� ������ ����) *******************/
		if (mode == 'm' || mode == 'M')		// ��尡 m�̶�� �ѹ��� ������ �Ѵ�.
			getchar();

		// ���� ��ġ�� �ⱸ��� �ݺ����� �����ϰ� ���α׷��� ������.
		if (isExit(r, c))
		{
			map[r][c] = 9;
			printMaze(locStack, ScanRatStack, r, c, mode);
			printf("\n �̷� Ž�� ����\n");
			getchar();

			mode = 'e';	// �̷� ��ü �����ֱ�
			printMaze(locStack, ScanRatStack, r, c, mode);

			getchar();
			free2DInt(map, rows, cols);	// �����Ҵ��� �޸𸮸� �ݳ���
			return;
		}

		// ���� ��ġ�� �ⱸ�� �ƴ϶�� �ֺ��� �ѷ����� �����Ѵ�.	
		else
		{
			// ��(��)���� �ⱸ(��)���� Ȯ��
			for(int i = r - 1; i <= r + 1; i++)			// 8���� Ȯ����
			{
				for(int j = c - 1; j <= c + 1; j++)
				{
					if (isWall(i, j)) map[i][j] = 2;
					else if(isExit(i, j)) 
					{
						map[i][j] = 4;
						exit_find = TRUE;	// �ⱸ ã�Ҵ� ǥ��
						for(int k = 0; k < cols; k++)	// �ⱸ ã���� ������ ��躮�� 2�� ����
						{
							if(map[0][k] == 1) map[0][k] = 2;
							if(map[rows - 1][k] == 1) map[rows - 1][k] = 2;
						}
						for(int k = 0; k < rows; k++)
						{
							if(map[k][0] == 1) map[k][0] = 2;
							if(map[k][cols - 1] == 1) map[k][cols - 1] = 2;
						}
					}
				}
			}

			if(mana >= 3.0)		// ������ 3�̻��̸� ��Ŀ�����͸� ���� ��ĵ�� ���� ����Ѵ�.
			{
				locStack.push(Location2D(r, c)); // ������ǥ ������ �������¶� �ٽ� �־��ش�.
				if(bunker_good(locStack))		 // ��Ŀ������ �ᵵ �Ǵ��� Ȯ��
				{
					bunkerBuster(locStack.top().row, locStack.top().col); // �����͸� ����� ��ǥ�� �մ´�.
					mana -= 3.0;
					bunker_count++;
				}
				else if(scan_avail == TRUE)
				{
					while(mana >= 1.0 && scan_avail == TRUE)
					{
						scan_count++;								// ��ĵ ��� Ƚ�� + 1 
						mana -= 1.0;								// ���� ����				
						scan(exit_find, direction, scan_row, scan_col, ScanRatStack);	// ������ ��ǥ���ٰ� ��ĵ �Ѹ�
						scanLocation(scan_row, scan_col, exit_find, ScanRatStack, r, c);// ��ĵ�Ѹ� ��ǥ ����
					}
				}
			}

			// ���� ��ġ ���¸� �ٲ�
			if		(map[r][c] == 0 || map[r][c] == 3) map[r][c] = 9;	// 1�� ������
			else if (map[r][c] == 9)				   map[r][c] = 8;	// 2�� ������
			else if (map[r][c] == 8)				   map[r][c] = 7;	// 3�� ������
			else if (map[r][c] == 7)				   map[r][c] = 6;	// 4�� ������
			else if (map[r][c] == 5 || map[r][c] == 4) map[r][c] = 10;	// �ⱸ�� ���� ���� ����

			// 4����, �켱����
			int a[] = {0, -1, 1, 0};	// ���� ����
			int b[] = {-1, 0, 0, 1};	// ���� ����, ��, ��, ��, ��
			int p[][4] = {				// ���� �켱����, ���� ���� Ž���� ����
				{1, 3, 2, 0},			// ����
				{1, 0, 2, 3},			// ����
				{2, 3, 1, 0},			// �ƿ�
				{2, 0, 1, 3},			// �ƿ�
				{3, 1, 2, 0},			// ����
				{3, 2, 1, 0},			// ����
				{0, 1, 2, 3},			// ����
				{0, 2, 1 ,3}};			// �޾�, �켱 ���� ����

			// ����� Ȯ��
			for(int i = 0; i < 4; i++)
			{
				if(map[r][c] == -1) break;			// ���� ��ġ�� �����ִ� ���̸� �ٽ� �ǵ��ƿ�

				int ra = r + a[i], cb = c + b[i];	// ������ġ���� 4������ ��Ÿ������
				int rpa = r + a[p[direction][i]]; 
				int cpb = c + b[p[direction][i]];	// ������ġ���� �켱������ ����

				if(isValid(rpa, cpb))	// ��ǥ�� �� �ȿ� �־����
				{
					if(isExit(rpa, cpb))					// ���� �ⱸ�� ã����
					{
						locStack.push(Location2D(r, c));
						locStack.push(Location2D(rpa, cpb)); // �ⱸ��ġ�� �ְ� �ݺ��� Ż��
						break;
					}
					else if(map[rpa][cpb] == 4)	// ���� �ⱸ�� �̾������� ã���� �ְ� �ݺ��� Ż��
					{
						map[r][c] = 10;			// �ⱸ�� �̾������� ����
						locStack.push(Location2D(r, c));
						locStack.push(Location2D(rpa, cpb));
						break;
					}
				}
					// �� ��ġ�� 8���⿡ �ⱸ�� ���ٸ�
					// �� ��ġ�� �����¿찡 �� �� �ִ±�(0) ������ �Ǵ��Ͽ�
					// �� �� �ִ� ���̶�� ���ÿ� �ְ� �����Ѵ�.
					// ���ÿ� �־����� ������ ���ư��� �ʾҴٸ� �ش� ��ġ�� 3���� �ٲ��ش�.(���ÿ� �ѹ� �ְ� �ٽ� �ȳֱ� ����)
				if(isValid(ra, cb))
				{
					if (map[ra][cb] == 0)
					{			
						// ���� ��ġ�� �ѹ� �̻� �������� ���� ���. r,c�� �㰡 �ִ°��, ������ map[r][c]�� 0�� ��� 9�� �مf��.
						locStack.push(Location2D(r, c));		// �ٽ� �ǵ��ƿ��� ���� ���� ��ġ�� ����
						locStack.push(Location2D(ra, cb));		// ���⿡ ���� ���� ����
						if(map[ra][cb] == 0) map[ra][cb] = 3;	// ó�� ���� �濡�� ���ÿ� ����											
					}
					else if(map[ra][cb] == 5)	// ���� �ⱸ�� �̾������� ã���� �ְ� �ݺ��� Ż��
					{
						map[r][c] = 10;
						locStack.push(Location2D(r, c));
						locStack.push(Location2D(ra, cb));
					}
				}
			}
		}
		printMaze(locStack, ScanRatStack, r, c, mode);	// �̷� ���
		printf("�ⱸ ã��? : %d (1: TRUE, 0 : FALSE), �ⱸ�� �ִ� �� : %d \n",exit_find, direction);
		if ((initEnergy - mazeMove) == 0) break;	// ���� ü���� �� �ߴ�.
	}

	printf("�̷� Ž�� ����\n");
	system("pause");

	free2DInt(map, rows, cols);
}	// Yol!!
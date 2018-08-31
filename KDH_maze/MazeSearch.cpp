#include "Location2D.h"			// 위치
#include "Scan2D.h"				// 스캔
#include "ScanRat.h"			// 스캔 쥐
#include <Windows.h>			// 콘솔 제어용
#include <stack>				// 스택
using namespace std;			// 이름 공간 사용

#define UP_RIGHT   0			
#define UP_LEFT	   1
#define DOWN_RIGHT 2
#define DOWN_LEFT  3
#define RIGHT_UP   4
#define RIGHT_DOWN 5
#define LEFT_UP    6
#define LEFT_DOWN  7			// 우선순위용 매크로

extern int** alloc2DInt( int rows, int cols);				// 외부 함수 가져오기
extern void free2DInt (int** mat, int rows, int cols=0);

// 위치 변수
int**	map  = NULL;		// 이중 포인터( 미로 표현용)
int		rows = 1;		
int		cols = 0;			// 미로의 행과 열

// 쥐의 상태 변수
int		initEnergy  = 0;	// 쥐의 체력
int		mazeMove	= 0;	// 이동횟수
double	mana		= 0;	// 쥐의 마나

// 스킬 변수
int     scan_count	 = 0;	  // 스킬 카운트(사용횟수)
int		bunker_count = 0;
bool    scan_avail	 = FALSE; // 스캔 사용 여부 

// 함수 호출
void loadMazeFile();	// 미로 파일 가져오는 함수
void printMaze(stack<Location2D> here, stack<ScanRat>& rat, int r, int c, int mode); // 미로를 콘솔에 표현해주는 함수
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

	stack<Location2D>	locStack;		// 위치 스택
	stack<ScanRat>		ScanRatStack;	// 스캔쥐 스택

	Location2D entry(0, 1);				// -> 시작지점 0, 1
	locStack.push(entry);				// 위치 스택에 처음위치(0.1)을 넣는다.

	//	printMaze(locStack,0,1,mode);
	//	getchar();
	mazeSearch(locStack, ScanRatStack, mode);
}

bool isValid(int r, int c)	//  맵 안의 좌표인지 확인
{
	if ( r < 0 || c < 0 || r >= rows || c >= cols ) return FALSE;		// 지도 밖이면 안됨
	else return TRUE;
}

bool isValidLoc(int r, int c)	// 갈 수 있는 위치인지 확인
{
	if (isValid(r, c)) return (map[r][c] == 0 || map[r][c] == 4 || map[r][c] == 5);	// 만약 해당 위치가 0또는 4또는 5이면 갈 수 있다.
	else  return false;	 		// 지도 밖이면 안됨
}

bool isValidS(int r, int c)		// 스캔 뿌릴 수 있는 곳인지 확인
{
	if (isValid(r, c)) return map[r][c] == 5 ;// 해당 위치가 5이면 가능함
	else return false; 
}

bool isExit(int r, int c)	// 출구를 찾는 함수
{
	if(isValid(r,c) && (r == 0 || r == rows-1 || c == 0 || c == cols-1) && (r != 0 && c != 1)) // 맵안의 좌표이고, 입구가 아니며,
	{
		return	(map[r][c] == 0 || map[r][c] == 4);	// 경계벽쪽에 있는 좌표라면 출구			  		
	}
	else return FALSE;
}

bool isWall(int r, int c) 
{
	if(isValid(r, c))	return (map[r][c] == 1);	// 확인 안된 좌표는 벽이었다.
	else	return false;
}

/*****************여기까지는 건들게 없다 ******************/
bool isClose(int r, int c) // 막힌 곳인지 확인하는 함수, 지금까지 확인한 벽을 토대로 막힌 길인지 확인함
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

void selectMode(char* mode)		// 모드 선택(자동, 수동)
{
	static int i = 0;	

	if( i == 0 )				// 처음에만 모드를 선택함
	{
		printf("Select Mode ( A or M ) : ");
		scanf("%c", mode);
		i++;
	}
}

void printMaze(stack<Location2D> here, stack<ScanRat>& rat, int r, int c, int mode)	// 미로를 콘솔에 표시해주는 함수
// 현재 위치, 스캔위치, 
{
	system("cls");	// 화면을 생동감 있게 보여주기 위해서 
	printf( "=============================================\n");
	printf( " 전체 미로의 크기 = %d %d\n", rows, cols);
	printf( " 에너지 = %d\n", initEnergy);
	printf( " 이동 = %d\n", mazeMove);
	printf( " 현재 위치 = (%d, %d)\n", c, r);
	printf( " 남은 에너지 = %d\n", initEnergy-mazeMove); // 체력은 한번 이동할 때마다 1씩 줄어듬
	printf( " 마나 = %.1lf\n", mana);
	
	Scan2D s(0, 0);				// 스캔 좌표
	if(rat.size() != 0)			// 스캔 스택이 비어있으면 안됨
	{	
		s.col = rat.top().col;
		s.row = rat.top().row;
	}
	printf( "\n 스캔 중심 좌표 = %d , %d", s.col, s.row);
	printf( "\n 스캔 사용 횟수 = %d", scan_count);
	printf( "\n 버스터 사용 횟수 = %d", bunker_count);
	printf( "\n=============================================\n");
	
	for( int i = 0 ; i < rows; i++ ) 
	{
		for( int j = 0 ; j < cols; j++ )
		{	
			if(i == r && j == c)
			{
				printf("☆");		// 현재 위치
				continue;
			}
			switch (map[i][j]) 
			{
				case -1 : printf("♩");	break;	// 막힌 곳, 벙커버스터로 뚫으면 출구로 가는길
				case 0  : printf("  "); break;  // 아직 가보지 않은 곳, 갈 수 있는 길
				case 1  :						// 아직 확인 안한 벽
					if( mode == 'e' )			// 출구를 찾앗을때 맵 전체를 밝히기 위해
					{	
						printf( "■" );			
						break;
					}
					else 
					{
						printf("  ");			// 미로를 탐색중엔 맵을 모르므로 공백처리
						break;
					}
				case 2  : printf("■");	break;	// 이미 확인한 벽
				case 3  : printf("  "); break;	// 확인된 갈 수 있는 길
				case 4  : printf("♬");	break;	// 출구로 확정된 길
				case 5  : printf("♪");	break;	// 스캔으로 확인한 스캔쥐가 갈 수 있는길
				case 6  : printf("④");	break;	// 4번 지나간 길
				case 7  : printf("③");	break;	// 3번 지나간 길
				case 8  : printf("②");	break;	// 2번 지나간 길
				case 9  : printf("①");	break;	// 1번 지나간 길	
				case 10 : printf("♥"); break;  // 출구로 이어지는길
			}
		} 
		printf("\n");				
	}

	if( (mode == 'm' || mode == 'M') && mode != 'e') {
		printf("/////////////////////////////////////////////\n");
		printf("현재 위치 (pop된 스택)\n");
		printf("row = %d col = %d\n", r, c) ;
		printf("현재 위치의 값(map[r][c]) = %d\n",map[r][c]);
		printf("남은 스택\n");
		stack<Location2D> pos;
		pos = here;
		for(int i = 0; i < here.size(); i++) {
			Location2D pos2D = pos.top();
			printf("[rows = %d , cols = %d]\t", pos2D.row, pos2D.col);
			pos.pop();
		}
	}	
}

void loadMazeFile()		// 미로를 읽는 함수
{	
	char ch;			// 열은 파일을 읽기 위해 만든 임시 변수.
	char filename[30];
	FILE *fp;			//= fopen(filename, "r");	// 파일을 연다.

	while(1) 
	{
		printf("파일 이름을 입력하세요 : ");
		gets_s(filename);
		fp = fopen(filename, "r");
		if ( fp == NULL ) 
		{
			printf(" 파일 %s을 열 수 없습니다.!!!\n", filename);
			continue;
		}
		break;
	}
	while(fscanf(fp, "%c", &ch) != EOF)		// 미로 txt파일을 읽는다, 파일이 끝나면 반복문 종료
	{	
		if(ch == '\n') rows++;				// txt파일에서 개행문자(enter)를 만나면 행개수를 센다.
		//if(ch == '0' || ch == '1') cols++;	// txt파일에서 문자의 개수를 센다.
		else if(ch != ' ') cols++;
	}
	cols = cols/rows;				// 열의 수 = 총 문자의 개수 / 행의 개수 

	initEnergy = 2 * cols * rows;	// 쥐의 체력 = 2 * 행 * 열

	map = alloc2DInt(rows,cols);	// map을 동적할당 받는다.
	fseek(fp, 0L, SEEK_SET);		// 05.18 -> 파일 포인터를 처음위치로 이동시킨다. 

	for(int i=0; i<rows; i++) 
	{
		for(int j=0; j<cols; j++) 
			fscanf(fp, "%d", &(map[i][j]));
	}
	fclose(fp);
}

/** 5. 21 스캔 함수 생성**/
void scan(bool &e, int &d, int r, int c, stack<ScanRat>& ScanRatStack) // 스캔 함수, 좌표를 받아서 미로 맵에 스캔을 뿌린다. d = 출구가 있는 방향 
{
	if (e == FALSE)	//스캔이 출구를 못찾았을 때
	{
		for(int i = r-1; i <= r+1; i++)
		{
			for(int j = c-1; j <= c+1; j++)			// 스캔 중심지를 기준으로 본인 위치와 주위 8방향 탐	
			{
				if(isWall(i, j)) map[i][j] = 2;		// 벽이면 숫자를 2로 전환
				else if(isExit(i, j)) 
				{
					map[i][j] = 4;			// 출구면 출구 확정
					e = TRUE;				// 출구 찾으면 설정
					ScanRatStack.push(ScanRat(i, j));	// 출구 좌표를 저장함
					
					// 출구가 있는 위치
					if(i == 0 && j <= cols / 2)			d = UP_LEFT;
					if(i == 0 && j >= cols / 2)			d = UP_RIGHT;
					if(i == rows - 1 && j <= cols / 2)	d = DOWN_LEFT;
					if(i == rows - 1 && j >= cols / 2)	d = DOWN_RIGHT;
					if(i <= rows / 2 && j == 0)			d = LEFT_UP;
					if(i >= rows / 2 && j == 0)			d = LEFT_DOWN;
					if(i <= rows / 2 && j == cols - 1)	d = RIGHT_UP;
					if(i >= rows / 2 && j == cols - 1)	d = RIGHT_DOWN;

					for(int k = 0; k < cols; k++)	// 출구 찾으면 나머지 경계벽을 2로 만듦
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

	if (e == TRUE)	//스캔이 출구를 찾았을때
	{
		for(int i = r-1; i <= r+1; i++)
		{
			for(int j = c-1; j <= c+1; j++)	// 스캔 중심지를 기준으로 본인 위치와 주위 8방향 탐색
			{
				if(isValid(i, j))
				{
					if(isWall(i, j)) map[i][j] = 2;	// 벽이면(1) 숫자를 2로 전환
					// 벽이 아니고, 길만 뚫려 있다면 출구로 이어지는 길
					else if(map[i][j] == 0 || map[i][j] == 3) map[i][j] = 5; 
				}
			}
		}										
	}
}

/** 5.21 스캔 좌표 설정 함수 생성 **/
// 스캔 포인트 행, 열, 출구 찾았는지 확인, 스캔 쥐 좌표스택, 현재 위치, 스캔 사용 여부
void scanLocation(int& r, int& c, bool& e, stack<ScanRat>& ScanRatStack, int hr, int hc)	// 스캔 좌표를 설정해주는 함수
{
	static int d = 3;		// 정적 지역변수, 처음엔 위로 가는중
	if (e == FALSE)			// 스캔이 출구를 못찾았을 때 일단 들어감
	{
		if (d == 0)			// 오른쪽가는중, 이미 갔던길이라면 넘어가자.
		{
			for(int i = c; i <= cols - 2; i++)	// 이미 확인한 곳인지 확인한다.
			{
				if(map[0][i] == 2) 	c = i + 1;	// 확인한 경계벽
				else if(map[0][i] == 1)		// 아직 확인 안한 벽이면
				{
					while(map[1][c] == 2)	// 지나갔던 곳인지 확인.
						c++;				// 다음 열
					c++;					// 아직 확인 안한 열의 위치
					break;
				}
			}
			if ( c >= cols - 2)				 // 오른쪽으로 최대치까지 갔을 경우 아래로이동	
			{
				c = cols - 2;
				if(map[r][c] != 1)	d = 1;// 이제 아래로 가는 방향			
			}
		}

		if (d == 1)		// 아래로 가는중
		{
			for(int i = r; i <= rows - 2; i++)
			{
				if(map[i][cols - 1] == 2) r = i + 1; // 확인한 경계벽
				else if(map[i][cols - 1] == 1)	// 아직 확인 안한 벽
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
				if(map[r][c] != 1)	d = 2;		// 최대치까지 갔을 경우 왼쪽으로 가자		
			}
		}

		if (d == 2)		// 왼쪽으로 가는중
		{
			for(int i = c; i >= 1; i--)
			{
				if(map[rows - 1][i] == 2) c = i - 1; // 확인한 경계벽
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
				if(map[r][c] != 1) d = 3;		// 최대치까지 갔을 경우 위쪽으로 가자			
			}
		}

		if (d == 3)		 // 위로 가는중
		{
			for(int i = r; i >= 1; i--)
			{
				if(map[i][0] == 2) r = i - 1; // 확인한 경계벽
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
				d = 0;	 // 오른쪽
			}						
		}
	}
	
	else //스캔이 출구를 찾았을때
	{
		//쥐가 본스택이 0이나 1이면 종료
		//기존 탐색알고리즘인데 '5'만 가는 쥐
		//지나가면 5 = 확정길을 만듦
		//만약에 벽을 보고 돌아오면 길을 막음(미구현)
		static bool find = FALSE;			// 쥐가 지나간 길을 찾았을 때 TRUE
		while (ScanRatStack.size() != 0)
		{
			ScanRat rathere = ScanRatStack.top();	
			ScanRatStack.pop();				// 앞으로 가는 과정   
			int rat_r = rathere.row;		// 행과 열을 스캔쥐의 위치로 바꿈
			int rat_c = rathere.col;

			// 쥐가 일단 스캔 뿌린곳, 5가 있는 곳까지 가보고 막히면 거기서 멈추고 스캔 뿌림
			if(map[rat_r][rat_c] == 2)		map[rat_r][rat_c] = 2;
			else if(isClose(rat_r, rat_c) || (rat_r == 0 && rat_c == 1))	map[rat_r][rat_c] = -1;	// 막힌 곳이면 -1로 만들고 되돌아옴

			else if(find == TRUE)			// 쥐가 지나간 길과 만났다.
			{
				int a[] = {0, -1, 1, 0};
				int b[] = {-1, 0, 0, 1};	// 왼, 위, 아, 오

				// 스캔쥐가 길위에 있으면 4로 바꿔줌
				if(map[rat_r][rat_c] >= 5 && map[rat_r][rat_c] <= 9 || map[rat_r][rat_c] == 3)	map[rat_r][rat_c] = 4; 
				for(int i = 0; i < 4; i++)
				{
					int ra = rat_r + a[i], cb = rat_c + b[i]; // 4방향 확인
					if(isValid(ra, cb))		// 맵안의 좌표여야함
					{					
						if(ra == hr && cb == hc) // 스캔쥐와 쥐가 만났다.
						{
							printf("%d %d", hr, hc);
							system("pause");
							map[ra][cb] = 4;
							scan_avail = FALSE;
							return; // 만약 현재위치에 스캔쥐가 오면 스캔 종료 
						}
						else if(map[ra][cb] >= 6 && map[ra][cb] <= 9 || map[ra][cb] == 3 ) // 스캔쥐가 쥐가 지나간 길을찾으면?
						{
							ScanRatStack.push(ScanRat(rat_r, rat_c)); // 돌아올 수 있게 제자리 저장
							ScanRatStack.push(ScanRat(ra, cb));						
						}
					}
				}
			}

			else if(find == FALSE)	// 아직 쥐가 지나간 길을 안갔을 때
			{
				int a[] = {0, 1, 0, -1};
				int b[] = {-1, 0, 1, 0};	// 왼, 아, 오, 위
				if(map[rat_r][rat_c] >= 5 && map[rat_r][rat_c] <= 9 || map[rat_r][rat_c] == 3)	map[rat_r][rat_c] = 4;

				for(int i = 0; i < 4; i++)
				{
					int ra = rat_r + a[i], cb = rat_c + b[i];	 // 4방향 확인
					if(isValid(ra, cb))							 // 맵안의 좌표여야함
					{
						if(map[ra][cb] >= 6 && map[ra][cb] <= 9 || map[ra][cb] == 3) // 스캔쥐가 쥐가 지나간 길을찾으면?
						{
							while(ScanRatStack.size() > 0)	ScanRatStack.pop();	// 스택을 다 비운다.
							ScanRatStack.push(ScanRat(ra, cb));
							find = TRUE;	// 이제 스캔하는 건 멈추고, 현재위치가 나올 때 까지 계속 4로 만든다.
							break;
						}
						// 만약 쥐가 바라보는 쪽에 확인 안한 곳이 있다면 거길 먼저 저장하고 종료
						else if (map[ra][cb] == 0 || map[ra][cb] == 1)	
						{					
							ScanRatStack.push(ScanRat(rat_r, rat_c));		// 돌아올 수 있게 제자리 저장							
							ScanRatStack.push(ScanRat(ra, cb));
							r = ra + a[i];
							c = cb + b[i];	// 스캔 포인트 설정
							return;
						}
						else if (isValidS(ra, cb))	// 해당 위치가 5인지 확인
						{   
							ScanRatStack.push(ScanRat(rat_r, rat_c));		// 돌아올 수 있게 제자리 저장
							ScanRatStack.push(ScanRat(ra, cb));
						}
					}
				}
			}
		}
	}
}

// 그냥 좌표의 길을 뚫어버림(4로)
void bunkerBuster(int r, int c) 
{
	map[r][c] = 4;
}

// 현재 쥐를 기준으로 2칸을 보는데, 벽 + 확정길이면 벽을 뚫고 다른 길을 막아버린다.
bool bunker_good(stack<Location2D>& locStack)	// 벙커 버스터 사용하기 좋은지 판단
{
	Location2D loc = locStack.top();	// 현재위치를 저장
	int r = loc.row;
	int c = loc.col;
	
	int a[] = {0, -1, 0, 1};
	int b[] = {-1, 0, 1, 0};

	for(int i = 0; i < 4; i++)
	{
		int ra = r + a[i], cb = c + b[i];
		int rra = ra + a[i], ccb = cb + b[i];	// 2칸 앞을 봐야해서
		if(isValid(ra, cb) && isValid(rra, ccb))
		{
			if(map[ra][cb] == 2 && map[rra][ccb] == 4) // 확정길이 꼬불꼬불한 길
			{
				map[r][c] = -1;	// 현재 위치는 다시 안가도록 10으로 설정
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
				locStack.push(Location2D(ra, cb));		// 뚫을 벽쪽 좌표를 넣는다.
				return TRUE;
			}
			else if(map[ra][cb] == 2 && map[rra][ccb] == -1 && map[r][c] != 10)
			{
				map[r][c] = 10;	// 현재 위치는 다시 안가도록 10으로 설정
				while(locStack.size() > 0 )	locStack.pop();			
				locStack.push(Location2D(ra, cb));	// 뚫을 벽쪽 좌표를 넣는다.
				return TRUE;
			}
		}
	}
	return FALSE;
}
void mazeSearch(stack<Location2D> locStack, stack<ScanRat>& ScanRatStack, char mode)
// 위치, 스캔 위치, 스캔쥐 위치, 모드
{
	bool exit_find = FALSE;		// 스캔이 출구를 찾았는지 확인하는 변수
	scan_avail = TRUE;			// 스캔 사용 가능

	int direction = 5;			// 초기 설정은 오른쪽 아래
	int scan_row = rows - 2;
	int scan_col = 1;			// 스캔 중심 포인트

	while (locStack.empty() == false)		// 위치 스택이 빌 때 까지(다시 입구로 돌아옴)
	{
		Location2D here = locStack.top();	// 현재 위치는 스택 맨 위
		locStack.pop();						// 앞으로 가는 과정	

		mazeMove++;							// pop이 한번 일어날때마다(=한번 이동할 때마다) mazeMove가 증가
		mana = mana + 0.1;					// pop이 한번 일어날때마다(=한번 이동할 때마다) mana가 0.1 증가

		int r = here.row;					// 현재 위치 행
		int c = here.col;					// 현재 위치 열	

		/**************** 현석이 만듦(기본 틀, 미로를 탐색하고 출구로 나가는 과정) *******************/
		if (mode == 'm' || mode == 'M')		// 모드가 m이라면 한번씩 나가게 한다.
			getchar();

		// 현재 위치가 출구라면 반복문을 종료하고 프로그램을 끝낸다.
		if (isExit(r, c))
		{
			map[r][c] = 9;
			printMaze(locStack, ScanRatStack, r, c, mode);
			printf("\n 미로 탐색 성공\n");
			getchar();

			mode = 'e';	// 미로 전체 보여주기
			printMaze(locStack, ScanRatStack, r, c, mode);

			getchar();
			free2DInt(map, rows, cols);	// 동적할당한 메모리를 반납함
			return;
		}

		// 현재 위치가 출구가 아니라면 주변을 둘러보기 시작한다.	
		else
		{
			// 벽(■)인지 출구(♬)인지 확인
			for(int i = r - 1; i <= r + 1; i++)			// 8방향 확인함
			{
				for(int j = c - 1; j <= c + 1; j++)
				{
					if (isWall(i, j)) map[i][j] = 2;
					else if(isExit(i, j)) 
					{
						map[i][j] = 4;
						exit_find = TRUE;	// 출구 찾았다 표시
						for(int k = 0; k < cols; k++)	// 출구 찾으면 나머지 경계벽을 2로 만듦
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

			if(mana >= 3.0)		// 마나가 3이상이면 벙커버스터를 쓸지 스캔을 쓸지 고민한다.
			{
				locStack.push(Location2D(r, c)); // 현재좌표 스택이 빠진상태라 다시 넣어준다.
				if(bunker_good(locStack))		 // 벙커버스터 써도 되는지 확인
				{
					bunkerBuster(locStack.top().row, locStack.top().col); // 버스터를 사용할 좌표를 뚫는다.
					mana -= 3.0;
					bunker_count++;
				}
				else if(scan_avail == TRUE)
				{
					while(mana >= 1.0 && scan_avail == TRUE)
					{
						scan_count++;								// 스캔 사용 횟수 + 1 
						mana -= 1.0;								// 마나 감소				
						scan(exit_find, direction, scan_row, scan_col, ScanRatStack);	// 설정된 좌표에다가 스캔 뿌림
						scanLocation(scan_row, scan_col, exit_find, ScanRatStack, r, c);// 스캔뿌릴 좌표 설정
					}
				}
			}

			// 현재 위치 상태를 바꿈
			if		(map[r][c] == 0 || map[r][c] == 3) map[r][c] = 9;	// 1번 지나감
			else if (map[r][c] == 9)				   map[r][c] = 8;	// 2번 지나감
			else if (map[r][c] == 8)				   map[r][c] = 7;	// 3번 지나감
			else if (map[r][c] == 7)				   map[r][c] = 6;	// 4번 지나감
			else if (map[r][c] == 5 || map[r][c] == 4) map[r][c] = 10;	// 출구로 가는 길을 지남

			// 4방향, 우선순위
			int a[] = {0, -1, 1, 0};	// 행의 방향
			int b[] = {-1, 0, 0, 1};	// 열의 방향, 왼, 위, 아, 오
			int p[][4] = {				// 행은 우선순위, 열은 먼저 탐색할 방향
				{1, 3, 2, 0},			// 위오
				{1, 0, 2, 3},			// 위왼
				{2, 3, 1, 0},			// 아오
				{2, 0, 1, 3},			// 아왼
				{3, 1, 2, 0},			// 오위
				{3, 2, 1, 0},			// 오아
				{0, 1, 2, 3},			// 왼위
				{0, 2, 1 ,3}};			// 왼아, 우선 순위 설정

			// 사방향 확인
			for(int i = 0; i < 4; i++)
			{
				if(map[r][c] == -1) break;			// 현재 위치가 막혀있는 길이면 다시 되돌아옴

				int ra = r + a[i], cb = c + b[i];	// 현재위치에서 4방향을 나타내게함
				int rpa = r + a[p[direction][i]]; 
				int cpb = c + b[p[direction][i]];	// 현재위치에서 우선순위를 정함

				if(isValid(rpa, cpb))	// 좌표는 맵 안에 있어야함
				{
					if(isExit(rpa, cpb))					// 만약 출구를 찾으면
					{
						locStack.push(Location2D(r, c));
						locStack.push(Location2D(rpa, cpb)); // 출구위치를 넣고 반복문 탈출
						break;
					}
					else if(map[rpa][cpb] == 4)	// 만약 출구로 이어진길을 찾으면 넣고 반복문 탈출
					{
						map[r][c] = 10;			// 출구로 이어진길을 지남
						locStack.push(Location2D(r, c));
						locStack.push(Location2D(rpa, cpb));
						break;
					}
				}
					// 현 위치의 8방향에 출구가 없다면
					// 현 위치의 상하좌우가 갈 수 있는길(0) 인지를 판단하여
					// 갈 수 있는 길이라면 스택에 넣고 진행한다.
					// 스택에 넣엇지만 앞으로 나아가지 않았다면 해당 위치는 3으로 바꿔준다.(스택에 한번 넣고 다시 안넣기 위해)
				if(isValid(ra, cb))
				{
					if (map[ra][cb] == 0)
					{			
						// 현재 위치가 한번 이상 지나가지 않은 경우. r,c에 쥐가 있는경우, 위에서 map[r][c]가 0일 경우 9로 바꿧다.
						locStack.push(Location2D(r, c));		// 다시 되돌아오기 위해 현재 위치를 저장
						locStack.push(Location2D(ra, cb));		// 방향에 따른 스택 저장
						if(map[ra][cb] == 0) map[ra][cb] = 3;	// 처음 가본 길에만 스택에 넣음											
					}
					else if(map[ra][cb] == 5)	// 만약 출구로 이어진길을 찾으면 넣고 반복문 탈출
					{
						map[r][c] = 10;
						locStack.push(Location2D(r, c));
						locStack.push(Location2D(ra, cb));
					}
				}
			}
		}
		printMaze(locStack, ScanRatStack, r, c, mode);	// 미로 출력
		printf("출구 찾음? : %d (1: TRUE, 0 : FALSE), 출구가 있는 쪽 : %d \n",exit_find, direction);
		if ((initEnergy - mazeMove) == 0) break;	// 쥐의 체력이 다 했다.
	}

	printf("미로 탐색 실패\n");
	system("pause");

	free2DInt(map, rows, cols);
}	// Yol!!
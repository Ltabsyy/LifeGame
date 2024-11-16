#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

#define LimName 32

int** board = 0;
int** numberOfLiveAround = 0;
int heightOfBoard = 25;
int widthOfBoard = 50;
int time = 0;

void Print();//显示
void CountAround();//分析
void Init();//初始化
void Evolve();//演化
void Edit();//编辑

struct Template//模板
{
	char* name;
	int height;
	int width;
	int** board;//0死，1生，2强制死
};
struct Templates
{
	struct Template* tem;//template是C++关键字(doge)
	int numberOfTemplate;
}templates;
void ReadTemplates();
void WriteTemplates();
void AddTemplate();
void DelTemplate(int del);
void PrintTemplate(struct Template t);
void PrintTemplates();
void EditTemplate(struct Template* t);
void InsTemplate(struct Template t, int r0, int c0);

// 控制台显示
void ColorStr(const char* content, int color)//输出彩色字符
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
	printf("%s", content);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07);
}
void gotoxy(short int x, short int y)//以覆写代替清屏，加速Bench
{
	COORD coord = {x, y};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
void clrscr()//清空屏幕
{
	HANDLE hdout = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hdout, &csbi);//获取标准输出设备的屏幕缓冲区属性
	DWORD size = csbi.dwSize.X * csbi.dwSize.Y, num = 0;//定义双字节变量
	COORD pos = {0, 0};
	//把窗口缓冲区全部填充为空格并填充为默认颜色
	FillConsoleOutputCharacter(hdout, ' ', size, pos, &num);
	FillConsoleOutputAttribute(hdout, csbi.wAttributes, size, pos, &num);
	SetConsoleCursorPosition(hdout, pos);//光标定位到窗口左上角
}
void setbgcolor(int color)//设置背景颜色
{
	HANDLE hdout = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hdout, &csbi);
	DWORD size = csbi.dwSize.X * csbi.dwSize.Y, num = 0;
	COORD pos = {0, 0};
	FillConsoleOutputAttribute(hdout, color, size, pos, &num);
	SetConsoleTextAttribute(hdout, color);
}
void showCursor(int visible)//显示或隐藏光标
{
	CONSOLE_CURSOR_INFO cursor_info = {20, visible};
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}
void ColorNumber(int number, int color)//输出彩色数字
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
	printf("%d", number);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07);
}
void SetConsoleMouseMode(int mode)//键鼠操作切换
{
	if(mode == 1)//切换到鼠标
	{
		SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);
	}
	else if(mode == 0)//切换到键盘
	{
		//SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_QUICK_EDIT_MODE | ENABLE_EXTENDED_FLAGS);
		//system("pause");//system指令使SetConsoleMode失效
		SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_PROCESSED_INPUT | ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT
			| ENABLE_MOUSE_INPUT | ENABLE_INSERT_MODE | ENABLE_QUICK_EDIT_MODE | ENABLE_EXTENDED_FLAGS
			| ENABLE_AUTO_POSITION);
	}
}

int main()
{
	int choice, i, seed;
	ReadTemplates();
	while(1)
	{
		clrscr();
		PrintTemplates();
		printf("1.开始游戏\n2.新建模板\n3.删除模板\n4.编辑模板\n");
		scanf("%d", &choice);
		if(choice == 1)
		{
			break;
		}
		else if(choice == 2)
		{
			AddTemplate();
		}
		else if(choice == 3)
		{
			printf("[序号]>");
			scanf("%d", &i);
			DelTemplate(i);
		}
		else if(choice == 4)
		{
			printf("[序号]>");
			scanf("%d", &i);
			EditTemplate(&(templates.tem[i]));
		}
		WriteTemplates();
	}
	printf("推荐：小窗25*50，全屏45*90，种子0\n");
	printf("[高度] [宽度]>");
	scanf("%d%d", &heightOfBoard, &widthOfBoard);
	printf("[种子]>");
	scanf("%d", &seed);
	clrscr();
	Init();
	CountAround();
	Print();
	printf("[继续]");
	getch();
	clrscr();
	showCursor(0);
	while(1)
	{
		Evolve();
		CountAround();
		Print();
		printf("[暂停编辑]");
		Sleep(50);
		if(kbhit())
		{
			getch();
			Edit();//编辑地图
		}
	}
	return 0;
}

void Print()
{
	int r, c;
	gotoxy(0, 0);
	printf("Time:%d\n", time);
	for(r=0; r<heightOfBoard; r++)
	{
		/*for(c=0; c<widthOfBoard; c++)
		{
			if(board[r][c] == 0)
			{
				printf(" _");
			}
			else
			{
				printf(" @");
			}
		}*/
		/*for(c=0; c<widthOfBoard; c++)
		{
			printf(" %d", numberOfLiveAround[r][c]);
		}*/
		for(c=0; c<widthOfBoard; c++)
		{
			if(board[r][c] == 0)
			{
				ColorStr(" ", 0x00);
				if(numberOfLiveAround[r][c] == 3)
				{
					ColorNumber(numberOfLiveAround[r][c], 0x02);
				}
				else
				{
					ColorNumber(numberOfLiveAround[r][c], 0x01);
				}
			}
			else
			{
				ColorStr(" ", 0xf0);
				if(numberOfLiveAround[r][c] < 2 || numberOfLiveAround[r][c] > 3)
				{
					ColorNumber(numberOfLiveAround[r][c], 0xf4);
				}
				else
				{
					ColorNumber(numberOfLiveAround[r][c], 0xf2);
				}
			}
		}
		printf("\n");
	}
}

void CountAround()
{
	int r, c, ra, ca;
	for(r=0; r<heightOfBoard; r++)
	{
		for(c=0; c<widthOfBoard; c++)
		{
			numberOfLiveAround[r][c] = 0;
		}
	}
	for(r=0; r<heightOfBoard; r++)
	{
		for(c=0; c<widthOfBoard; c++)
		{
			if(board[r][c] == 1)//从存活细胞出发
			{
				for(ra=r-1; ra<=r+1; ra++)
				{
					for(ca=c-1; ca<=c+1; ca++)
					{
						if(ra>=0 && ra<heightOfBoard && ca>=0 && ca<widthOfBoard)
						{
							numberOfLiveAround[ra][ca]++;
						}
					}
				}
				numberOfLiveAround[r][c]--;//排除自身
			}
		}
	}
}

void Init()
{
	int r, c;
	board =(int**) calloc(heightOfBoard, sizeof(int*));
	numberOfLiveAround =(int**) calloc(heightOfBoard, sizeof(int*));
	time = 0;
	for(r=0; r<heightOfBoard; r++)
	{
		board[r] =(int*) calloc(widthOfBoard, sizeof(int));
		numberOfLiveAround[r] =(int*) calloc(widthOfBoard, sizeof(int));
		for(c=0; c<widthOfBoard; c++)
		{
			board[r][c] = rand()%2;
			numberOfLiveAround[r][c] = 0;
		}
	}
}

void Evolve()
{
	int r, c;
	for(r=0; r<heightOfBoard; r++)
	{
		for(c=0; c<widthOfBoard; c++)
		{
			if(board[r][c] == 0)
			{
				if(numberOfLiveAround[r][c] == 3)
				{
					board[r][c] = 1;
				}
			}
			else
			{
				if(numberOfLiveAround[r][c] < 2 || numberOfLiveAround[r][c] > 3)
				{
					board[r][c] = 0;
				}
			}
		}
	}
	time++;
}

void Edit()
{
	HANDLE hdin = GetStdHandle(STD_INPUT_HANDLE);
	COORD mousePos = {0, 0};
	COORD mouseOperatedPos = {0, 0};
	INPUT_RECORD rcd;
	DWORD rcdnum;
	int isReadyRefreshMouseOperatedPos = 0;
	int isReadyInsertTemplate = 0, insIndex;
	int r, c;
	SetConsoleMouseMode(1);
	while(1)
	{
		gotoxy(0, 0);
		CountAround();
		Print();
		if(isReadyInsertTemplate == 1)
		{
			printf("[左键:插入/地图内右键:卸下模板/地图外右键:继续]");
		}
		else
		{
			printf("[左键:修改/地图内右键:加载模板/地图外右键:继续]");
		}
		isReadyRefreshMouseOperatedPos = 1;
		ReadConsoleInput(hdin, &rcd, 1, &rcdnum);
		if(rcd.EventType == MOUSE_EVENT)
		{
			mousePos = rcd.Event.MouseEvent.dwMousePosition;
			r = mousePos.Y-1;
			c = mousePos.X/2;
			if(rcd.Event.MouseEvent.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
			{
				if(r>=0 && r<heightOfBoard && c>=0 && c<widthOfBoard)
				{
					if(mousePos.X >= mouseOperatedPos.X-1 && mousePos.X <= mouseOperatedPos.X+1
						&& mousePos.Y == mouseOperatedPos.Y)
					{
						isReadyRefreshMouseOperatedPos = 0;
					}
					else
					{
						if(isReadyInsertTemplate == 1)
						{
							isReadyInsertTemplate = 0;
						}
						else
						{
							gotoxy(0, heightOfBoard+2);
							SetConsoleMouseMode(0);
							PrintTemplates();
							printf("[模板序号]>");
							scanf("%d", &insIndex);
							//printf("已加载模板，下次修改即可插入");
							isReadyInsertTemplate = 1;
							clrscr();
							SetConsoleMouseMode(1);
						}
						mouseOperatedPos.X = mousePos.X;
						mouseOperatedPos.Y = mousePos.Y;
						isReadyRefreshMouseOperatedPos = 0;
					}
				}
				else
				{
					break;
				}
			}
			if(rcd.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
			{
				if(r>=0 && r<heightOfBoard && c>=0 && c<widthOfBoard)
				{
					if(mousePos.X >= mouseOperatedPos.X-1 && mousePos.X <= mouseOperatedPos.X+1
						&& mousePos.Y == mouseOperatedPos.Y)
					{
						isReadyRefreshMouseOperatedPos = 0;
					}
					else
					{
						if(isReadyInsertTemplate == 1)
						{
							InsTemplate(templates.tem[insIndex], r, c);
							//isReadyInsertTemplate = 0;
						}
						else
						{
							board[r][c] = 1-board[r][c];
						}
						mouseOperatedPos.X = mousePos.X;
						mouseOperatedPos.Y = mousePos.Y;
						isReadyRefreshMouseOperatedPos = 0;
					}
				}
			}
		}
		if(isReadyRefreshMouseOperatedPos == 1)
		{
			mouseOperatedPos.X = 0;
			mouseOperatedPos.Y = 0;
		}
		Sleep(50);
	}
	clrscr();
	SetConsoleMouseMode(0);
}

void ReadTemplates()
{
	FILE* file;
	int i, r, c;
	templates.numberOfTemplate = 0;
	if(file = fopen("lifegame-templates.txt", "r"))
	{
		fscanf(file, "numberOfTemplates=%d\n", &(templates.numberOfTemplate));
		templates.tem =(struct Template*) calloc(templates.numberOfTemplate, sizeof(struct Template));
		//templates.tem =(struct Template*) calloc(templates.numberOfTemplate, sizeof(struct Templates));
		//如果为此，templates.tem[0].name会指向templates.tem[7].height
		for(i=0; i<templates.numberOfTemplate; i++)
		{
			fscanf(file, "\n");
			templates.tem[i].name =(char*) calloc(LimName, sizeof(char));
			fscanf(file, "name:");
			fgets(templates.tem[i].name, LimName, file);//读取空格和回车
			fscanf(file, "size:%d*%d\n", &(templates.tem[i].height), &(templates.tem[i].width));
			templates.tem[i].board =(int**) calloc(templates.tem[i].height, sizeof(int*));
			for(r=0; r<templates.tem[i].height; r++)
			{
				templates.tem[i].board[r] =(int*) calloc(templates.tem[i].width, sizeof(int));
				fscanf(file, " ");
				for(c=0; c<templates.tem[i].width; c++)
				{
					fscanf(file, " %d", &(templates.tem[i].board[r][c]));
				}
				fscanf(file, "\n");
			}
		}
		fclose(file);
	}
}

void WriteTemplates()
{
	FILE* file;
	struct Template temp;
	int i, r, c;
	file = fopen("lifegame-templates.txt", "w");
	fprintf(file, "numberOfTemplates=%d\n", templates.numberOfTemplate);
	for(i=0; i<templates.numberOfTemplate; i++)
	{
		temp = templates.tem[i];//减少访问次数
		fprintf(file, "\n");
		fprintf(file, "name:%s", temp.name);//输出空格和回车
		fprintf(file, "size:%d*%d\n", temp.height, temp.width);
		for(r=0; r<temp.height; r++)
		{
			fprintf(file, " ");
			for(c=0; c<temp.width; c++)
			{
				fprintf(file, " %d", temp.board[r][c]);
			}
			fprintf(file, "\n");
		}
	}
	fclose(file);
	if(templates.numberOfTemplate == 0)
	{
		remove("lifegame-templates.txt");
	}
}

void AddTemplate()
{
	struct Template* temp =(struct Template*) calloc(templates.numberOfTemplate+1, sizeof(struct Template));
	struct Template newtem;
	int i, r;
	for(i=0; i<templates.numberOfTemplate; i++)
	{
		temp[i] = templates.tem[i];
	}
	//编辑新模板
	newtem.name =(char*) calloc(LimName, sizeof(char));
	fflush(stdin);
	printf("[name]>");
	fgets(newtem.name, LimName, stdin);
	printf("[height] [width]>");
	scanf("%d%d", &(newtem.height), &(newtem.width));
	newtem.board =(int**) calloc(newtem.height, sizeof(int*));
	for(r=0; r<newtem.height; r++)
	{
		newtem.board[r] =(int*) calloc(newtem.width, sizeof(int));
	}
	EditTemplate(&newtem);
	temp[templates.numberOfTemplate] = newtem;
	if(templates.numberOfTemplate != 0) free(templates.tem);
	templates.tem = temp;
	templates.numberOfTemplate++;
}

void DelTemplate(int del)
{
	int i;
	if(del >= 0 && del < templates.numberOfTemplate)
	{
		struct Template* temp =(struct Template*) calloc(templates.numberOfTemplate-1, sizeof(struct Template));
		for(i=0; i<del; i++)
		{
			temp[i] = templates.tem[i];
		}
		free(templates.tem[del].name);
		for(i=0; i<templates.tem[del].height; i++)
		{
			free(templates.tem[del].board[i]);
		}
		free(templates.tem[del].board);
		for(i=del; i<templates.numberOfTemplate-1; i++)
		{
			temp[i] = templates.tem[i+1];
		}
		free(templates.tem);
		templates.tem = temp;
		templates.numberOfTemplate--;
	}
}

void PrintTemplate(struct Template t)
{
	int r, c;
	printf("name:%s", t.name);
	printf("size:%d*%d\n", t.height, t.width);
	for(r=0; r<t.height; r++)
	{
		for(c=0; c<t.width; c++)
		{
			if(t.board[r][c] == 1)//生
			{
				ColorStr("  ", 0x20);
			}
			else if(t.board[r][c] == 2)//强制死
			{
				ColorStr("  ", 0x40);
			}
			else//默认死
			{
				ColorStr("  ", 0x10);
			}
		}
		printf("\n");
	}
}

void PrintTemplates()
{
	int i;
	for(i=0; i<templates.numberOfTemplate; i++)
	{
		printf("模板%d:\n", i);
		PrintTemplate(templates.tem[i]);
	}
	printf("模板共%d个\n", templates.numberOfTemplate);
}

void EditTemplate(struct Template* t)
{
	HANDLE hdin = GetStdHandle(STD_INPUT_HANDLE);
	COORD mousePos = {0, 0};
	COORD mouseOperatedPos = {0, 0};
	INPUT_RECORD rcd;
	DWORD rcdnum;
	int isReadyRefreshMouseOperatedPos = 0;
	int r, c;
	clrscr();
	SetConsoleMouseMode(1);
	while(1)
	{
		gotoxy(0, 0);
		//CountAround();
		//Print();
		PrintTemplate(*t);
		printf("蓝:死(默认) 绿:生 红:强制死\n");
		printf("[左键:修改/右键:退出]");
		isReadyRefreshMouseOperatedPos = 1;
		ReadConsoleInput(hdin, &rcd, 1, &rcdnum);
		if(rcd.EventType == MOUSE_EVENT)
		{
			mousePos = rcd.Event.MouseEvent.dwMousePosition;
			r = mousePos.Y-2;
			c = mousePos.X/2;
			if(rcd.Event.MouseEvent.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
			{
				break;
			}
			if(rcd.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
			{
				if(r>=0 && r<t->height && c>=0 && c<t->width)
				{
					if(mousePos.X >= mouseOperatedPos.X-1 && mousePos.X <= mouseOperatedPos.X+1
						&& mousePos.Y == mouseOperatedPos.Y)
					{
						isReadyRefreshMouseOperatedPos = 0;
					}
					else
					{
						t->board[r][c]++;
						if(t->board[r][c] > 2) t->board[r][c] = 0;
						mouseOperatedPos.X = mousePos.X;
						mouseOperatedPos.Y = mousePos.Y;
						isReadyRefreshMouseOperatedPos = 0;
					}
				}
			}
		}
		if(isReadyRefreshMouseOperatedPos == 1)
		{
			mouseOperatedPos.X = 0;
			mouseOperatedPos.Y = 0;
		}
		Sleep(50);
	}
	clrscr();
	SetConsoleMouseMode(0);
}

void InsTemplate(struct Template t, int r0, int c0)
{
	int r, c;
	for(r=0; r<t.height; r++)
	{
		for(c=0; c<t.width; c++)
		{
			if(t.board[r][c] == 1)
			{
				board[r0+r][c0+c] = 1;
			}
			else if(t.board[r][c] == 2)
			{
				board[r0+r][c0+c] = 0;
			}
		}
	}
}

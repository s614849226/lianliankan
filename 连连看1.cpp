// 连连看1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
//using namespace std;

#include <iostream>
#include<stdio.h>
#include<graphics.h>
#define IMG_NUM 42//图片数量
#define IMG_SIZE 39//图片宽度
#define MAP_SIZE 10//每行十张图片
#define WIN_SIZE MAP_SIZE*IMG_SIZE+2*IMG_SIZE
#include<windows.h>
#include<Mmsystem.h>
#include<dsound.h>
#pragma comment(lib, "WINMM.LIB")
//二维数组做地图 数字代表图片
IMAGE img_total[2];
IMAGE img_animal[42][2];
POINT begin, end;//开始和结束点击的数组下标
bool play_sound=0;
bool click=0;
enum STATE
{
	m_begin,
	m_end
};
int state = m_begin;
int map[10 + 2][10 + 2];

void GameInit()
{
	//设置随机数种子
	srand(GetTickCount());
	//加载图片
	loadimage(&img_total[0], L"./animal.bmp");
	//分割图片并保存数组
	loadimage(&img_total[1], L"./bk.jpg", WIN_SIZE+60, WIN_SIZE+60);
	SetWorkingImage(&img_total[0]);//处理这张图片
	for (int i = 0; i < IMG_NUM; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			getimage(&img_animal[i][j], j*IMG_SIZE,i*IMG_SIZE,IMG_SIZE, IMG_SIZE);
		}
		
	}
	SetWorkingImage();

	int num = 1,count=0;
	//对游戏区域设置数据
	for (int i = 1; i <= MAP_SIZE; i++)
	{
		for (int k = 1; k <= MAP_SIZE ;k++)
		{
			//printf("-%2d,%2d-", i,k);
			map[i][k] = num;
			
			num = (count % 10 == 0) ? (num + 1) : num;
			count++;
		}
	//	putchar('\n');

	}
	//打乱动物
	for (int i = 1; i <= MAP_SIZE; i++)
	{
		for (int k = 1; k <= MAP_SIZE; k++)
		{
			//printf("-%2d,%2d-", i,k);
			int r1 = rand() % MAP_SIZE + 1;
			int c1 = rand() % MAP_SIZE + 1;
			int r2 = rand() % MAP_SIZE + 1;
			int c2 = rand() % MAP_SIZE + 1;
			int temp = map[r1][c1];
			map[r1][c1] = map[r2][c2];
			map[r2][c2] = temp;
			//map[i][k] = num;

		//	num = (count % 10 == 0) ? (num + 1) : num;
			//count++;
		}
		//	putchar('\n');
	}
}
void GameDraw()
{
	putimage(0,0,&img_total[1]);
	//透明贴图

	


	int num = 1, count = 0;
	//对游戏区域设置数据
	for (int i = 1; i <= MAP_SIZE; i++)
	{
		for (int k = 1; k <= MAP_SIZE; k++)
		{
			if (map[i][k] != 0)
			{
			putimage(k*IMG_SIZE, i*IMG_SIZE, &img_animal[map[i][k]][1], SRCAND);
			putimage(k*IMG_SIZE, i*IMG_SIZE, &img_animal[map[i][k]][0], SRCPAINT);
			}

		}
		//	putchar('\n');

	}
}
bool clear(POINT begin, POINT end);
void show()
{
	for (int i = 0; i < MAP_SIZE + 2; i++)
	{
		for (int k = 0; k < MAP_SIZE + 2; k++)
		{
		//	printf("%2d ", map[i][k]);

		}
		//putchar('\n');

	}


}
//鼠标操作
void MouseControl()
{

	//只有鼠标点击才可能触发播放音效，防止音效持续播放
	//判断是否有鼠标消息
	if (MouseHit())
	{	//获取鼠标消息
		click = 1;
		MOUSEMSG msg = GetMouseMsg();
		//转化成数组下标
		int c=(msg.x) / IMG_SIZE;
		int r=(msg.y) / IMG_SIZE;
		if (msg.uMsg == WM_LBUTTONDOWN)//鼠标左键点击
		{
			if (state == m_begin)
			{
			begin.x = r;
			begin.y = c;
			state = m_end;

			}
			else if (state == m_end)
			{
				state = m_begin;
				end.x = r;
				end.y = c;
				
			}
		//	printf("begin(%d,%d) end(%d,%d)\n", begin.x, begin.y, end.x,end.y);
			if (clear(begin, end))
			{
				map[begin.x][begin.y] = 0;
				map[end.x][end.y] = 0;
				begin.x = -1;
				begin.y = -1;
				play_sound = 1;
				if (play_sound&&click) {
					PlaySound(TEXT(".\\音效.wav"), NULL, SND_FILENAME | SND_ASYNC);
				}
			}
		
		}
		
	}

}
//判断水平方向能否消除
bool isBlocked(int r, int c)
{
	return map[r][c];//判断是否有障碍
}
bool horizon(POINT begin,POINT end)
{
	//判断是否在同一水平方向
	if(begin.x!=end.x)
	{ 
		return false;
	}
	else if (begin.x == end.x&&begin.y == end.y)
	{
		return false;
	}
	int maxc = max(begin.y, end.y);
	int minc = min(begin.y, end.y);//判断哪个在左边
	for (int i = minc + 1; i < maxc; i++)
	{
		if (isBlocked(begin.x, i))
		{
			return false;
		}
	}
	return true;
}
bool same(POINT begin, POINT end)
{
	int r1 = begin.x;
	int c1 = begin.y;
	int r2 = end.x;
	int c2 = end.y;

	if (map[r1][c1] == map[r2][c2])
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool column(POINT begin, POINT end)
{
	//判断是否在同一竖直方向
	if (begin.y != end.y)
	{
		return false;
	}
	else if (begin.x == end.x&&begin.y == end.y)
	{
		return false;
	}
	int maxc = max(begin.x, end.x);
	int minc = min(begin.x, end.x);//判断哪个在上边
	for (int i = minc + 1; i < maxc; i++)
	{
		if (isBlocked(i, begin.y))
		{
			return false;
		}
	}
	return true;
}
bool one_turner(POINT begin, POINT end)
{
	POINT turner1 = { 0,0 }; POINT turner2 = {0,0};
	turner1.x = begin.x;
	turner1.y = end.y;//与end column
	turner2.x = end.x;
	turner2.y = begin.y;
	//printf("turner %d", turner1);
	//printf("turner2 %d", turner2);
	bool a =horizon(begin,turner1) && column(turner1, end) && (map[turner1.x][turner1.y] == 0);//拐角两个条件，拐角为零一个条件
	bool b = column (begin,turner2) &&(horizon(turner2, end)) && (map[turner2.x][turner2.y] == 0);
	if (a||b)
	{
		return true;
	}
	else
	{
		return false;
	}

}
bool two_turner(POINT begin, POINT end)
{
	for (int i = 0; i <= MAP_SIZE+1; i++)
	{
		POINT point1;
		point1.x = i;
		point1.y = begin.y;
		//begin.x = i;
		if (one_turner(point1, end)&&column(point1,begin)&&map[point1.x][point1.y]==0) {//最后一个条件是防止在相邻处发生转弯但column，horizon无法识别的过程
			return true;
		}//一个一条线空的，一个转弯空的
	}
	for (int i = 0; i <= MAP_SIZE+1; i++)
	{
		POINT point2;
		point2.y = i;
		point2.x = begin.x;
		//begin.y = i;
		if (one_turner(point2, end)&&horizon(point2,begin) && map[point2.x][point2.y] == 0) {//最后一个条件是防止在相邻处发生转弯但column，horizon无法识别的过程
			return true;
		}
	}//一个一条线空的，一个转弯空的
	return false;

}
bool clear(POINT begin,POINT end)
{
	if (!same(begin,end))
	{
		return false;
	}
	if (begin.x == end.x&&begin.y == end.y)
	{
		return false;
	}
	bool isOk = horizon(begin, end)|column(begin,end);
	if (!isOk)
	{
		isOk = one_turner(begin, end);//判断一个转角是否可行
	}
	if (!isOk)
	{
		isOk = two_turner(begin, end);//判断两个转角是否可行
	}
	
	
	return isOk;
}

int main()
{
	HWND hwnd = GetForegroundWindow();
	ShowWindow(hwnd, SW_HIDE);
	//system("pause");
	//首先创建图形窗口
	initgraph(WIN_SIZE,WIN_SIZE,SHOWCONSOLE);//会闪退
 //   std::cout << "连连看\n";
	GameInit();
//	show();
	BeginBatchDraw();
	//GameDraw();
		//return 0;
	while (true) {
		GameDraw();
		FlushBatchDraw();
		MouseControl();
		
	

		//Sleep(200);//防止声音不断播放
		click = 0; play_sound = 0;
		}
		
}



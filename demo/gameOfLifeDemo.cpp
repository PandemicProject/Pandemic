#include "graphics.h"
#include <windows.h>
#include <time.h>
#include <stdio.h>
#define High 80 //数组大小
#define Width 80
#define Hight 10 * High //画面大小
#define Widths 10 * Width

int cells[High][Width]; // 所有位置细胞生1或死0

void my_playsound();       //播放游戏中音乐
void gotoxy(int x, int y); //清屏函数，时间比system（“pause”）短
void startup();            //初始化
void show();               //展示
void updateInput();        //更新下一次的细胞状态
int Input_yonghu();        //用户输入迭代次数

int main()
{
    initgraph(Hight, Widths); //建立画布

    int r = 100;

    startup();     // 数据初始化
    int i = 0;     //限定迭代次数
    while (i <= r) //  游戏循环执行
    {
        show();        // 显示画面
        updateInput(); //细胞更新
        i++;
    }
    getch();
    closegraph(); //退出画布
}
void gotoxy(int x, int y) //类似于清屏函数
{
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos;
    pos.X = x;
    pos.Y = y;
    SetConsoleCursorPosition(handle, pos);
}
void startup() // 数据初始化
{
    int i, j;
    for (i = 0; i < High; i++)
    { // 初始化

        cells[i][1] = 1;
        cells[i][Width - 2] = 1;
        for (j = 0; j < Width; j++)
        {
            cells[1][j] = 1;
            cells[High - 2][j] = 1;
        }
    }
}
void updateInput() // 细胞更新
{
    int NewCells[High][Width]; // 下一帧的细胞情况
    int NeibourNumber;         //统计邻居的个数
    int i, j;
    for (i = 1; i < High - 1; i++)
    {
        for (j = 1; j < Width - 1; j++)
        {
            NeibourNumber = cells[i - 1][j - 1] + cells[i - 1][j] + cells[i - 1][j + 1] + cells[i][j - 1] + cells[i][j + 1] + cells[i + 1][j - 1] + cells[i + 1][j] + cells[i + 1][j + 1];
            if (NeibourNumber == 3)
                NewCells[i][j] = 1;
            else if (NeibourNumber == 2)
                NewCells[i][j] = cells[i][j];
            else
                NewCells[i][j] = 0;
        }
    }

    for (i = 1; i < High - 1; i++)
        for (j = 1; j < Width - 1; j++)
            cells[i][j] = NewCells[i][j];
}
void show() // 显示画面
{
    gotoxy(0, 0); // 清屏
    int i, j;
    for (i = 0; i < High; i++)
    {
        for (j = 0; j < Width; j++)
        {
            if (cells[i][j] == 1)
            {
                setfillcolor(RED);
                bar(10 * i, 10 * j, 10 * (i + 1), 10 * (j + 1));
            }
            else
            {
                setfillcolor(0x55FFFF);
                bar(10 * i, 10 * j, 10 * (i + 1), 10 * (j + 1));
            }
        }
    }
    Sleep(00);
}

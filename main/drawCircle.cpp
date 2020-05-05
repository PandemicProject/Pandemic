#include <random>
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include "graphics.h"
using namespace std;

default_random_engine e;

class Person
{
public:
    Person()
    {
        condition = (cnt++) % 3;
    }
    int GetCondition()
    {
        return condition;
    }
    void SetPosition(int _x, int _y)
    {
        position[0] = _x;
        position[1] = _y;
    }
    int *GetPos()
    {
        return position;
    }

    static int cnt;

private:
    int position[2];
    int condition;
};

int Person::cnt = 0;

Person pool[1000];

void SetPerson(Person arr[], int len = 1000)
{
    normal_distribution<double> distribution(300, 100);
    int _x, _y;

    for (int i = 0; i < len; i++)
    {
        _x = lround(distribution(e));
        _y = lround(distribution(e));
        arr[i].SetPosition(_x, _y);
    }
}

void PrintPerson()
{
    //cleardevice();
    for (auto &x : pool)
    {
        int _cond = x.GetCondition();
        switch (_cond)
        {
        case 0:
            setfillcolor(RED);
            setcolor(RED);
            break;

        case 1:
            setfillcolor(WHITE);
            setcolor(WHITE);
            break;

        case 2:
            setfillcolor(BLUE);
            setcolor(BLUE);
            break;
        }
        fillellipse(*(x.GetPos()), *(x.GetPos() + 1), 3, 3);
    }
}

int main()
{

    initgraph(600, 600); // 初始化640 * 480大小的窗口
    //line(1, 1, 100, 100);
    /* setfillcolor(RED);
    setcolor(RED);
    fillellipse(100, 100, 5, 5);
    getch(); */
    SetPerson(pool);
    PrintPerson();
    getch();
    closegraph(); //关闭图形环境
    return 0;
}

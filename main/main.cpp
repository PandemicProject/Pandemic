#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include <ctime>
#include <string.h>
#include "graphics.h"
#include "sys_edit.h"

using namespace std;

#define WINDOW 600
double moveWill = 1; //should be average + noise
int day = 0;
int healthy = 990;
int exposed = 5;
int infected = 5;
int dead = 0;
const int hospitalBedPerThousand = 6;
int hospitalResponse = 10;
int bed = 50;
int bedConsumption = 10;
int hosPosition = 610;
const int population = 1000;
double deathRate = 0.05; //死亡率
double broadRate = 0.7;  // infected by whether people wear mask
const int threshold = 5;
const int square = 800;
const int life = 80;
int cell[80][80] = {0};
const int buffsize = 100;

default_random_engine e;

typedef struct
{
    int position[2]; //(x, y)
    int condition;   //0: healthy, 1: exposed, 2: infected 3: dead
    int age;
    double deathRate;
    int lurkDayCnt; //潜伏天数
    int lurkThreshhold;
    int infectedDayCnt;
    int quarantine; //0: in quarantine, 1: not in quarantine
    void (*Move)(int *);
} Person;

typedef struct
{
    int bedCnt;
} Hospital;

Person pool[1000];

void InitHospital(int cnt)
{
    Hospital hospital = {cnt};
    bed += cnt;
}

void UpdateHospital()
{
    setcolor(WHITE);
    setfillcolor(WHITE);
    rectangle(0, 610, 600, 630);
    int length;
    double ratio = (double)bedConsumption / bed;
    if (ratio > 1)
    {
        length = 600;
    }
    else
    {
        length = lround(600 * ratio);
    }
    bar(0, 610, length, 630);
}

void Initialize()
{
    int i, j;
    for (i = 0; i < life; i++)
    {
        cell[i][0] = 1;
        cell[i][life - 1] = 1;

        for (j = 0; j < life; j++)
        {
            cell[0][j] = 1;
            cell[life - 1][j] = 1;
        }
    }
}

void Update()
{
    int neighbour = 0, i, j;
    int tmp[80][80];
    for (i = 0; i < life - 1; i++)
    {
        for (j = 0; j < life - 1; j++)
        {
            neighbour = cell[i - 1][j - 1] + cell[i - 1][j] + cell[i - 1][j + 1] + cell[i][j - 1] + cell[i][j + 1] + cell[i + 1][j - 1] + cell[i + 1][j] + cell[i + 1][j + 1];
            if (neighbour == 3)
            {
                tmp[i][j] = 1;
            }
            else if (neighbour == 2)
            {
                tmp[i][j] = cell[i][j];
            }
            else
            {
                tmp[i][j] = 0;
            }
        }
    }

    for (i = 1; i < life - 1; i++)
    {
        for (j = 1; j < life - 1; j++)
        {
            cell[i][j] = tmp[i][j];
        }
    }
}

void DisplayGame()
{
    cleardevice();
    int i, j;
    for (i = 0; i < life; i++)
    {
        for (j = 0; j < life; j++)
        {
            if (cell[i][j] == 1)
            {
                setcolor(WHITE);
                setfillcolor(WHITE);
                bar(i * 10, j * 10, (i + 1) * 10, (j + 1) * 10);
            }
            else
            {
                setcolor(BLACK);
                setfillcolor(BLACK);
                bar(i * 10, j * 10, (i + 1) * 10, (j + 1) * 10);
            }
        }
    }
}

void GameOfLife()
{
    int alpha = 0x00; //透明度
    PIMAGE des = newimage();
    PIMAGE src = newimage();
    getimage(src, "pic2.png");

    Initialize();
    DisplayGame();
    Sleep(500);

    for (; is_run(); delay_fps(60))
    {
        Update();
        DisplayGame();
        if (kbhit())
        {
            char flag = getch();                          //don't know why not adding this leads to termination of the program
            for (; alpha <= 0x8a; delay_fps(60), alpha++) //渐暗
            {
                getimage(des, 0, 0, getwidth(), getheight());
                putimage_alphablend(des, src, 0, 0, alpha, 0, 0);
                putimage(0, 0, des);
            }
            break;
        }
    }
    cleardevice();
    xyprintf(0, 0, "Against \n Pandemic");
    Sleep(300);
    delimage(des);
    delimage(src);
}

/* make sure people do not move out of the rectangle */
bool Check(int x, int y)
{
    if (x > (WINDOW - 5) || y > (WINDOW - 5))
    {
        return false;
    }
    else
    {
        return true;
    }
}

void _Move(int *arr)
{
    uniform_int_distribution<int> uniform(-20, 20);
    int dx = 0, dy = 0;

    do
    {
        dx = uniform(e) * moveWill;
        dy = uniform(e) * moveWill;
    } while (!Check(*arr + dx, *(arr + 1) + dy));

    *arr += dx;
    *(arr + 1) += dy;
    /* if (*arr > 590)
    {
        *arr -= 2 * dx;
    }
    if (*(arr + 1) > 590)
    {
        *arr -= 2 * dy;
    } */
    return;
}

void InitPerson(int len = 1000)
{
    normal_distribution<double> normal(300, 100);
    uniform_int_distribution<int> uniform(0, population - 1);
    uniform_int_distribution<int> lurkDay(2, 14);
    int _x, _y;

    for (int i = 0; i < len; i++)
    {
        //initialize people's position
        do
        {
            _x = lround(normal(e));
            _y = lround(normal(e));
        } while (!Check(_x, _y));

        if (_x > WINDOW)
        {
            _x = WINDOW;
        }

        if (_y > WINDOW)
        {
            _y = WINDOW;
        }

        pool[i].position[0] = _x;
        pool[i].position[1] = _y;
        pool[i].condition = 0;
        pool[i].lurkDayCnt = 0;
        pool[i].infectedDayCnt = 0;
        pool[i].Move = _Move;
        pool[i].lurkThreshhold = lurkDay(e);
    }

    //initialize infected and exposed people
    int initCount;
    initCount = infected;
    while (initCount--)
    {
        int index = uniform(e);
        if (pool[index].condition != 2)
        {
            pool[index].condition = 2;
        }
        else
        {
            initCount++;
        }
    }

    initCount = exposed;
    while (initCount--)
    {
        int index = uniform(e);
        if (pool[index].condition != 2 && pool[index].condition != 1)
        {
            pool[index].condition = 1;
        }
        else
        {
            initCount++;
        }
    }
}

void PrintPerson()
{
    setcolor(WHITE);
    rectangle(0, 0, 600, 600);
    for (auto &person : pool)
    {
        int _cond = person.condition;
        switch (_cond)
        {
        case 0:
            setfillcolor(WHITE);
            setcolor(WHITE);
            break;

        case 1:
            setfillcolor(GREEN);
            setcolor(GREEN);
            break;

        case 2:
            setfillcolor(RED);
            setcolor(RED);
            break;

        case 3:
            setfillcolor(TRANSPARENT);
            setcolor(TRANSPARENT);
            break;
        }
        fillellipse(*(person.position), *(person.position + 1), 2, 2);
    }
}

void Contact()
{
    double distance;
    int i, j, tmp;
    uniform_int_distribution<int> uniform(0, 9);

    for (i = 0; i < population - 1; i++)
    {
        if (pool[i].condition == 3) //死亡者不参与移动
        {
            continue;
        }
        for (j = i + 1; j < population; j++)
        {
            double distance = sqrt(pow((pool[i].position[0] - pool[j].position[0]), 2) + pow((pool[i].position[1] - pool[j].position[1]), 2)); //distance between two peopel
            if (distance <= threshold)
            {
                if (pool[i].condition == 0)
                {
                    if (pool[j].condition == 1 || pool[j].condition == 2)
                    {
                        tmp = uniform(e);
                        if (tmp <= 10 * broadRate)
                        {
                            pool[i].condition = 1;
                            exposed++;
                            healthy--;
                        }
                    }
                }
                else if (pool[j].condition == 0)
                {
                    if (pool[i].condition == 1 || pool[i].condition == 2)
                    {
                        tmp = uniform(e);
                        if (tmp <= 10 * broadRate)
                        {
                            pool[j].condition = 1;
                            exposed++;
                            healthy--;
                        }
                    }
                }
            }
        }
    }
    return;
}

void UpdateLurk(Person *person)
{
    person->lurkDayCnt++;
    if (person->lurkDayCnt == person->lurkThreshhold)
    {
        person->condition = 2;
        person->lurkDayCnt = 0;
        person->infectedDayCnt = 0;
        exposed--;
        infected++;
    }
}

void UpdateDeath(Person *person)
{
    /* suppose deathRate = 0.1
    use unifrom distribution to achieve that */
    uniform_int_distribution<int> uniform(0, 99);
    int i = uniform(e);
    if (i < 100 * deathRate)
    {
        person->condition = 3;
        dead++;
        infected--;
    }
}

void PrintText()
{
    char daystr[10] = {'\0'};
    char numberOfHealthy[10] = {'\0'};
    char numberOfExposed[10] = {'\0'};
    char numberOfInfected[10] = {'\0'};
    char numberOfDead[10] = {'\0'};
    char numberOfBed[10] = {'\0'};
    char outputTotalDay[35] = "Day ";
    char outputHealthy[20] = "# Healthy: ";
    char outputExposed[20] = "# Exposed: ";
    char outputInfected[20] = "# Infected: ";
    char outputDead[20] = "# Dead: ";
    char outputBed[30] = "# Hospital Bed: ";

    itoa(day, daystr, 10);
    itoa(healthy, numberOfHealthy, 10);
    itoa(exposed, numberOfExposed, 10);
    itoa(infected, numberOfInfected, 10);
    itoa(dead, numberOfDead, 10);
    itoa(bed, numberOfBed, 10);

    strcat(outputTotalDay, daystr);
    strcat(outputHealthy, numberOfHealthy);
    strcat(outputExposed, numberOfExposed);
    strcat(outputInfected, numberOfInfected);
    strcat(outputDead, numberOfDead);
    strcat(outputBed, numberOfBed);

    setcolor(WHITE);
    outtextxy(610, 10, outputTotalDay);
    outtextxy(610, 30, outputHealthy);
    outtextxy(610, 50, outputExposed);
    outtextxy(610, 70, outputInfected);
    outtextxy(610, 90, outputDead);
    outtextxy(610, 110, outputBed);
}

void FinalDisplay()
{
    Sleep(1000);
    cleardevice();
    /* char ptr[5], outputRate[20] = "Death Rate: ";
    double rate = (double)dead / population;
    gcvt(rate, 2, ptr);
    strcat(outputRate, ptr); */
    setcolor(WHITE);
    outtextxy(400, 300, "OVER");
}

int Over()
{
    if ((infected + dead) >= population * 0.6)
    {
        return 0;
    }
    else
    {
        //判断win
        return 1;
    }
}

void NewDay()
{
    for (auto &person : pool)
    {
        int tmp = person.condition;
        if (tmp != 3)
        {
            person.Move(person.position);
            if (tmp == 1) //潜伏期
            {
                UpdateLurk(&person); //更新潜伏情况
            }
            else if (tmp == 2) //出现症状
            {
                UpdateDeath(&person);
            }
        }
    }
    bedConsumption += 5;
}

void Draw()
{
    cleardevice();
    PrintPerson();
    UpdateHospital();
    PrintText();
}

void Text(char *s, int len)
{
    s[len - 1] = '\0';
    s[len - 2] = '\0';
}

void ConsoleInput()
{
    sys_edit box;
    box.create(true); //multiple line
    /* edit box parameter */
    box.move(610, 130);    //position
    box.size(160, 80);     //size
    box.setbgcolor(BLACK); //background color
    box.setcolor(WHITE);   //text color
    box.setfont(16, 0, "Fira Code");
    box.visable(true);
    box.setfocus(); //光标闪烁

    int flagPress = 0; //标记是否按下
    char buff[100] = {'\0'};
    int buffLength = 0, height = 0;
    for (; is_run(); delay_fps(60))
    {
        if (keystate(key_enter)) //检测到按下回车键就获取输入内容
        {
            if (!flagPress)
            {
                box.gettext(buffsize, buff); //获取输入框内容，保存在buff中
                buffLength = strlen(buff);
                Text(buff, buffLength);
                if (buffLength)
                {
                    if (!strcmp(buff, "exit"))
                    {
                        break;
                    }
                    else
                    {
                        if (!strcmp(buff, "build"))
                        {
                            InitHospital(100);
                        }
                        Sleep(500);
                        box.settext("");
                    }
                }
            }
            flagPress = 1;
        }
        else
        {
            flagPress = 0;
        }
    }
}

int main()
{
    initgraph(800, 800, 0); // initialize 800 * 800 window
    setrendermode(RENDER_MANUAL);
    /* GameOfLife();
    Sleep(300); */
    getch();
    InitPerson();
    PrintPerson();
    getch();

    while (Over())
    {
        //ConsoleInput();
        day++;
        NewDay();
        Contact();
        Draw();
        Sleep(200);
    }
    FinalDisplay();

    getch();
    closegraph();
    return 0;
}

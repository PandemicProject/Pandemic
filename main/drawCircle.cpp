#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include <ctime>
#include <string.h>
#include "graphics.h"
using namespace std;

#define WINDOW 600
const double moveWill = 1; //should be average + noise
int day = 0;
int healthy = 990;
int exposed = 5;
int infected = 5;
int dead = 0;
const int hospitalBed = 100;
const int population = 1000;

default_random_engine e;

typedef struct
{
    int position[2]; //(x, y)
    int condition;   //0: healthy, 1: exposed, 2: infected
    int age;
    int lurkDayCnt; //潜伏天数
    int lurkThreshhold;
    void (*Move)(int *);
} Person;

Person pool[1000];

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

    /* do
    {

    } while (!Check(*arr, *(arr + 1))); */
    dx = uniform(e) * moveWill;
    dy = uniform(e) * moveWill;
    *arr += dx;
    *(arr + 1) += dy;
    if (*arr > 590)
    {
        *arr -= 2 * dx;
    }
    if (*(arr + 1) > 590)
    {
        *arr -= 2 * dy;
    }
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
    cleardevice();
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
        }
        fillellipse(*(person.position), *(person.position + 1), 2, 2);
    }
}

void ChangeCondition()
{
    double threshhold = 5, distance;
    int i, j;

    for (i = 0; i < population - 1; i++)
    {
        for (j = i + 1; j < population; j++)
        {
            double distance = sqrt(pow((pool[i].position[0] - pool[j].position[0]), 2) + pow((pool[i].position[1] - pool[j].position[1]), 2)); //distance between two peopel
            if (distance <= threshhold)
            {
                if (pool[i].condition == 0)
                {
                    if (pool[j].condition == 1 || pool[j].condition == 2)
                    {
                        pool[i].condition = 1;
                        exposed++;
                        healthy--;
                    }
                }
                else if (pool[j].condition == 0)
                {
                    if (pool[i].condition == 1 || pool[i].condition == 2)
                    {
                        pool[j].condition = 1;
                        exposed++;
                        healthy--;
                    }
                }
            }
        }
    }
    return;
}

void UpdataLurk(Person *person)
{
    person->lurkDayCnt++;
    if (person->lurkDayCnt == person->lurkThreshhold)
    {
        person->condition = 2;
        person->lurkDayCnt = 0;
        exposed--;
        infected++;
    }
}

void PrintText()
{
    char daystr[10] = {'\0'};
    char numberOfHealthy[10] = {'\0'};
    char numberOfExposed[10] = {'\0'};
    char numberOfInfected[10] = {'\0'};
    char outputTotalDay[35] = {'\0'};
    char outputHealthy[20] = {'\0'};
    char outputExposed[20] = {'\0'};
    char outputInfected[20] = {'\0'};

    strcpy(outputTotalDay, "Days of pandemic: ");
    strcpy(outputHealthy, "# Healthy: ");
    strcpy(outputExposed, "# Exposed: ");
    strcpy(outputInfected, "# Infected: ");

    itoa(day, daystr, 10);
    itoa(healthy, numberOfHealthy, 10);
    itoa(exposed, numberOfExposed, 10);
    itoa(infected, numberOfInfected, 10);

    strcat(outputTotalDay, daystr);
    strcat(outputHealthy, numberOfHealthy);
    strcat(outputExposed, numberOfExposed);
    strcat(outputInfected, numberOfInfected);

    setcolor(WHITE);
    outtextxy(610, 10, outputTotalDay);
    outtextxy(610, 30, outputHealthy);
    outtextxy(610, 50, outputExposed);
    outtextxy(610, 70, outputInfected);
}

bool Over()
{
    if (infected == population)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int main()
{
    initgraph(800, 800); // initialize 800 * 800 window
    InitPerson();
    PrintPerson();
    getch();

    while (!Over())
    {
        day++;
        for (auto &person : pool)
        {
            person.Move(person.position);
            if (person.condition == 1)
            {
                UpdataLurk(&person);
            }
        }
        ChangeCondition();
        Sleep(300);
        PrintPerson();
        PrintText();
    }

    getch();
    closegraph();
    return 0;
}

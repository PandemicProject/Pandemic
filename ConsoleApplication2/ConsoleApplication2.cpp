#include <iostream>
#include <random>
#include <math.h>
#include <ctime>
#include <graphics.h>
#include "game.h"
#include "interaction.h"
#include "person.h"
#include "graph.h"
using namespace std;

double moveWill = 1; //PARAMETER
int day = 0;
int population = 2000;
int healthy = 1990; 
int exposed = 5; //潜伏期
int infected = 5; //出现症状
int dead = 0;

int hospitalResponse = 5; //PARAMETER
int bedTotal = 50;
int bedConsumption = 0;
int costPerBed = 50; //PARAMETER
double deathRate = 0.05; //死亡率 PARAMETER
double broadRate = 0.8;  // infected by whether people wear mask PARAMETER
int threshold = 5; //PARAMETER
int money = 100000;//PARAMETER
int moneyPerPerson = 2; //PARAMETER
int costPerBedPerDay = 10; //PARAMETER
int medicalStuff = 200;
int maskConsumptionMedical = 2;
int maskConsumptionOrdinary = 0;
int mask = 200000;
Person pool[2000];

bool Over();
void InitParam();


int main()
{
	int c, flag = 0xff;
	initgraph(800, 800, 0);
	setrendermode(RENDER_MANUAL); 
	//GameOfLife();
	//Sleep(300);

	while (flag)
	{
		Sleep(1000);
		InitParam();
		InitPerson();
		cleardevice();
		PrintPerson();
 		getch();

		while (Over())
		{
			if (kbhit())
			{
				c = getch();
				flag = Console();
				if (flag != 2)
				{
					break;
				}
				flushkey();
			}
			day++;
			NewDay();
			Contact();
			Draw();
			Sleep(300);
		}
		if (flag == 2)
		{
			FinalDisplay();
		}
	}
	getch();
	closegraph();
	return 0;
}

bool Over()
{
	if ((infected + dead) >= population * 0.7)
	{
		return false;
	}
	else
	{
		//判断win
		return true;
	}
}

void InitParam()
{
	moveWill = 1;
	day = 0;
	healthy = 1990;
	exposed = 5;
	infected = 5;
	dead = 0;
	hospitalResponse = 5;
	bedTotal = 50;
	bedConsumption = 0;
	costPerBed = 30;
	deathRate = 0.05;
	broadRate = 0.8;
	threshold = 5;
	money = 100000;
	moneyPerPerson = 2;
	costPerBedPerDay = 5;
	medicalStuff = 200;
	maskConsumptionMedical = 2;
	maskConsumptionOrdinary = 0;
	mask = 200000;
}
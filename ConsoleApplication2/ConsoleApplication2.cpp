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

double moveWill = 1; //PARAMETER //NOTE: 无论如何输入command, moveWill cannot be zero!!!
int day = 0;
int population = 2000;
int healthy = 1990; 
int exposed = 5; //潜伏期
int infected = 5; //出现症状
int dead = 0;
int quarantine = 0;

int hospitalResponse = 5; //PARAMETER
int bedTotal = 50;
int bedConsumption = 0;
int costPerBed = 50; //PARAMETER
double broadRate = 0.8;  // infected by whether people wear mask PARAMETER
int threshold = 5; //PARAMETER
int money = 100000;//PARAMETER
int moneyPerPerson = 2; //PARAMETER
int costPerBedPerDay = 10; //PARAMETER
int medicalStuff = 200; //PARAMETER
int maskConsumptionMedical = 2; //PARAMETER
int maskConsumptionOrdinary = 0; //PARAMETER
int maskProduction = 2; //PARAMETER
int mask = 10000; //PARAMETER
int vaccineReverseCnt = 210; //PARAMETER
int medicineReverseCnt = 120; //PARAMETER
bool quarantineCommandOn = false;
bool medicineLock = false; //whethe the medicine has been developed

Person pool[2000];

int Over();
void InitParam();

int main()
{
	int c, flag = 2;
	initgraph(950, 800, 0);
	setrendermode(RENDER_MANUAL); 
	//GameOfLife();

	while (flag)
	{
		Sleep(1000);
		InitParam();
		InitPerson();
		cleardevice();
		PrintPerson();
		PrintText();
		CommandLine();
		DrawHospital();
 		getch();

		while (!Over())
		{
			if (kbhit())
			{
				c = getch();
				flag = Console();
				if (flag != 2) //2: continue game, 1: new game,  0: exit game
				{
					break;
				}
				flushkey();
			}
			NewDay();
			Draw();
			Sleep(300);
		}
		if (flag == 2)
		{
			FinalDisplay(Over());
		}
	}
	getch();
	closegraph();
	return 0;
}

int Over()
{
	if (dead >= population * 0.5 || money < -500) //lose
	{
		return 1;
	}
	else if (!vaccineReverseCnt || (!exposed && !infected))//疫苗研发 或 没有潜伏和感染人群 win
	{
		return 2;
	}
	else
	{
		return 0; //游戏继续
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
	broadRate = 0.8;
	threshold = 5;
	money = 100000;
	moneyPerPerson = 2;
	costPerBedPerDay = 5;
	medicalStuff = 200;
	maskConsumptionMedical = 2;
	maskConsumptionOrdinary = 0;
	maskProduction = 2;
	mask = 10000;
	quarantineCommandOn = false;
	quarantine = 0;
	vaccineReverseCnt = 210;
	medicineReverseCnt = 120;
	medicineLock = false;
}
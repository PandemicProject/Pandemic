#include <iostream>
#include <random>
#include <math.h>
#include <ctime>
#include <graphics.h>
#include "opening.h"
#include "interaction.h"
#include "person.h"
#include "graph.h"
#include "game.h"
using namespace std;

int day = 0;
int vaccineReverseCnt = 210; //PARAMETER
int medicineReverseCnt = 120; //PARAMETER

int population = 2000;
int healthy = 1990; 
int exposed = 5; //潜伏期
int infected = 5; //出现症状
int dead = 0;
int quarantine = 0;
int medicalStuff = 200; //PARAMETER
int threshold = 5; //PARAMETER

int money = 100000;//PARAMETER
int costPerBed = 40; //PARAMETER
int costPerBedPerDay = 10; //PARAMETER
int moneyPerPerson = 2; //PARAMETER

int hospitalResponse = 5; //PARAMETER
int bedTotal = 50;
int bedConsumption = 0;

int maskConsumptionMedical = 2; //PARAMETER
int maskConsumptionOrdinary = 0; //PARAMETER
int maskProduction = 2; //PARAMETER
int mask = 10000; //PARAMETER

double broadRate = 0.8;  //PARAMETER
double moveWill = 1; //PARAMETER 

bool quarantineCommandOn = false;
bool medicineLock = false; //whether the medicine has been developed
bool flagLowerRate = false; //whether death rate has been lowered due to medicine
Person pool[2000];

int main()
{
	int c, flag = 2;
	initgraph(950, 700, 0);
	setrendermode(RENDER_MANUAL); 
	GameOfLife();
	getch();

	while (flag)
	{
		Sleep(1000);
		InitParam();
		InitPerson();
		Draw();
 		getch();

		while (!Over())
		{
			NewDay();
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
			Draw();
			Sleep(600);
		}
		if (flag == 2)
		{
			FinalDisplay(Over());
		}
	}
	closegraph();
	return 0;
}
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
int healthy = 990;
int exposed = 5;
int infected = 5;
int dead = 0;

int hospitalResponse = 5; //PARAMETER
int bedTotal = 50;
int bedConsumption = 0;
int costPerBed = 50; //PARAMETER
int population = 1000;
double deathRate = 0.05; //死亡率 PARAMETER
double broadRate = 0.8;  // infected by whether people wear mask PARAMETER
int threshold = 5; //PARAMETER
int money = 1000000; //PARAMETER

Person pool[2000];

int Over()
{
	if ((infected + dead) >= population * 0.7)
	{
		return 0;
	}
	else
	{
		//判断win
		return 1;
	}
}
int main()
{
	int c;
	initgraph(800, 800, 0); // initialize 800 * 800 window
	setrendermode(RENDER_MANUAL);
	GameOfLife();
	Sleep(300);
	getch();
	cleardevice();
	InitPerson();
	PrintPerson();
	getch();

	while (Over())
	{
		if (kbhit())
		{
			c = getch();
			Console();
			flushkey();
		}
		day++;
		NewDay();
		Contact();
		Draw();
		Sleep(300);
	}
	FinalDisplay();

	getch();
	closegraph();
	return 0;
}

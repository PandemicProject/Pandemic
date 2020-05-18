#include <graphics.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "person.h"
using namespace std;
extern int day;
extern int healthy;
extern int exposed;
extern int infected;
extern int dead;
extern int bedTotal;
extern int bedConsumption;
extern int population;
extern Person pool[2000];

void Draw()
{
	cleardevice();
	PrintPerson();
	DrawHospital();
	PrintText();
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

void PrintText()
{
	char daystr[10] = { '\0' };
	char numberOfHealthy[10] = { '\0' };
	char numberOfExposed[10] = { '\0' };
	char numberOfInfected[10] = { '\0' };
	char numberOfDead[10] = { '\0' };
	char numberOfBed[10] = { '\0' };
	char outputTotalDay[35] = "Day ";
	char outputHealthy[20] = "# Healthy: ";
	char outputExposed[20] = "# Exposed: ";
	char outputInfected[20] = "# Infected: ";
	char outputDead[20] = "# Dead: ";
	char outputBed[30] = "# Hospital Bed: ";

	_itoa(day, daystr, 10);
	_itoa(healthy, numberOfHealthy, 10);
	_itoa(exposed, numberOfExposed, 10);
	_itoa(infected, numberOfInfected, 10);
	_itoa(dead, numberOfDead, 10);
	_itoa(bedTotal - bedConsumption, numberOfBed, 10);

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
	float deathRate = (float)dead / population;
	deathRate *= 100;
	char tmp[5], _tmp[2] = "%", output[20] = "Death Rate: ";
	sprintf(tmp, "%.2f", deathRate);
	strcat(output, tmp);
	strcat(output, _tmp);
	setcolor(WHITE);
	outtextxy(400, 300, "OVER");
	outtextxy(400, 320, output);
}

void DrawHospital()
{
	setcolor(WHITE);
	setfillcolor(WHITE);
	rectangle(0, 610, 600, 630);
	int length;
	double ratio = (double)bedConsumption / bedTotal;
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
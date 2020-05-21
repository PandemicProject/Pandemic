#include <graphics.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "person.h"
using namespace std;
extern int day, healthy, exposed, infected, dead, bedTotal, bedConsumption, money, mask, population, quarantine;
extern Person pool[2000];
extern bool medicineLock;
extern int vaccineReverseCnt, medicineReverseCnt;

void Draw()
{
	cleardevice();
	PrintPerson();
	DrawHospital();
	DrawQuarantineZone();
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
			break;
		}
		fillellipse(person.position[0], person.position[1], 2, 2);
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
	char moneyLeft[10] = { '\0' };
	char maskLeft[10] = { '\0' };
	char numberOfQuarantine[10] = { '\0' };
	char vaccineDay[10] = { '\0' };
	char medicineDay[10] = { '\0' };

	char outputTotalDay[35] = "Day ";
	char outputHealthy[20] = "# Healthy: ";
	char outputExposed[20] = "# Exposed: ";
	char outputInfected[20] = "# Infected: ";
	char outputDead[20] = "# Dead: ";
	char outputBed[30] = "# Hospital Bed: ";
	char outputMoney[30] = "# Money Left: ";
	char outputMask[30] = "# Mask Left: ";
	char outputQuarantine[30] = "# Quarantine: ";
	char outputVaccine[50] = "# Days required for vaccine: ";
	char outputMedicine[50] = "# Days required for medicine ";

	_itoa(day, daystr, 10);
	_itoa(healthy, numberOfHealthy, 10);
	_itoa(exposed, numberOfExposed, 10);
	_itoa(infected, numberOfInfected, 10);
	_itoa(dead, numberOfDead, 10);
	_itoa(bedTotal - bedConsumption, numberOfBed, 10);
	_itoa(money, moneyLeft, 10);
	_itoa(mask, maskLeft, 10);
	_itoa(quarantine, numberOfQuarantine, 10);
	_itoa(vaccineReverseCnt, vaccineDay, 10);
	_itoa(medicineReverseCnt, medicineDay, 10);

	strcat(outputTotalDay, daystr);
	strcat(outputHealthy, numberOfHealthy);
	strcat(outputExposed, numberOfExposed);
	strcat(outputInfected, numberOfInfected);
	strcat(outputDead, numberOfDead);
	strcat(outputBed, numberOfBed);
	strcat(outputMoney, moneyLeft);
	strcat(outputMask, maskLeft);
	strcat(outputQuarantine, numberOfQuarantine);
	strcat(outputVaccine, vaccineDay);
	strcat(outputMedicine, medicineDay);

	setcolor(WHITE);
	setbkcolor(TRANSPARENT);
	outtextxy(610, 10, outputTotalDay);
	outtextxy(610, 30, outputHealthy);
	outtextxy(610, 50, outputExposed);
	outtextxy(610, 70, outputInfected);
	outtextxy(610, 90, outputDead);
	outtextxy(610, 110, outputBed);
	outtextxy(610, 130, outputMoney);
	outtextxy(610, 150, outputMask);
	outtextxy(610, 170, outputQuarantine);
	outtextxy(610, 190, outputVaccine);
	if (medicineLock)
	{
		setcolor(GREEN);
	}
	outtextxy(610, 210, outputMedicine);
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
	outtextxy(370, 300, "OVER");
	outtextxy(370, 320, output);
	outtextxy(370, 340, "Press any key to continue.");
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

void DrawQuarantineZone()
{
	setcolor(WHITE);
	rectangle(0, 640, 600, 800);
}
#include <graphics.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "person.h"

extern int healthy, exposed, infected, dead, bedTotal, bedConsumption, money, mask, population, quarantine;
extern Person pool[2000];
extern bool medicineLock;
extern int day, vaccineReverseCnt, medicineReverseCnt;

void Draw()
{
	cleardevice();
	PrintPerson();
	PrintText();
	DrawHospital();
	CommandLine();
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

	char outputTotalDay[20] = "Day ";
	char outputHealthy[25] = "Healthy: ";
	char outputExposed[25] = "Exposed: ";
	char outputInfected[25] = "Infected: ";
	char outputDead[25] = "Dead: ";
	char outputBed[35] = "Vacant Hospital Bed: ";
	char outputMoney[35] = "Money Left: ";
	char outputMask[35] = "Mask Left: ";
	char outputQuarantine[30] = "Quarantine: ";
	char outputVaccine[40] = "Days required for vaccine: ";
	char outputMedicine[40] = "Days required for medicine ";

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
	setfont(-15, 0, "Times New Roman");
	outtextxy(610, 10, outputTotalDay);
	outtextxy(610, 30, outputHealthy);
	outtextxy(610, 50, outputExposed);
	outtextxy(610, 70, outputInfected);
	outtextxy(610, 90, outputDead);
	outtextxy(610, 110, outputQuarantine);
	outtextxy(610, 130, outputBed);
	if (!money)
	{
		setcolor(RED);
	}
	outtextxy(610, 150, outputMoney);
	setcolor(WHITE);
	if (!mask)
	{
		setcolor(RED);
	}
	outtextxy(610, 170, outputMask);
	setcolor(WHITE);
	if (!vaccineReverseCnt)
	{
		setcolor(GREEN);
	}
	outtextxy(610, 190, outputVaccine);
	setcolor(WHITE);
	if (medicineLock)
	{
		setcolor(GREEN);
	}
	outtextxy(610, 210, outputMedicine);
}

// quit,counterQuarantine, newgame, exitgame };

void CommandLine()
{
	setfont(-15, 0, "Times New Roman");
	outtextxy(610, 250, "COMMAND LIST: ");
	outtextxy(610, 270, "build [number]: build a hospital with [number] beds");
	outtextxy(610, 290, "research [number1] [number2]: spend [number1] and ");
	outtextxy(665, 310, "[number2] coins to accelerate medicine and ");
	outtextxy(665, 330, "vaccine development respectively");
	outtextxy(610, 350, "work: ask people to go back to work");
	outtextxy(610, 370, "rm work: ask people not to go to work");
	outtextxy(610, 390, "distance: carry out social distancing");
	outtextxy(610, 410, "mask: ask people to wear masks");
	outtextxy(610, 430, "rm mask: ask people not to wear masks");
	outtextxy(610, 450, "quarantine: keep infected people in quarantine");
	outtextxy(610, 470, "rm quarantine: withdraw the quarantine measure");
	outtextxy(610, 490, "quit: stop inputting commands");
	outtextxy(610, 510, "new: start a new game");
	outtextxy(610, 530, "exit: quit the game");
	outtextxy(610, 560, "Press ANY key to input commands");
}

void FinalDisplay(int x)
{
	Sleep(1000);
	cleardevice();
	float deathRate = (float)dead / population;
	deathRate *= 100;
	char tmp[10], _tmp[2] = "%", output[30] = "Death Rate: ";
	sprintf(tmp, "%.2f", deathRate);
	strcat(output, tmp);
	strcat(output, _tmp);
	setcolor(WHITE);
	setfont(-30, 0, "Times New Roman");
	if (x == 1)
	{
		outtextxy(370, 300, "OVER");
	}
	else if (x == 2)
	{
		outtextxy(370, 300, "YOU WIN!");
	}
	setfont(-20, 0, "Times New Roman");
	outtextxy(370, 350, output);
	Sleep(2000);
}

void DrawHospital()
{
	setcolor(WHITE);
	setfillcolor(WHITE);
	rectangle(0, 620, 600, 650);
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
	bar(0, 620, length, 650);
}

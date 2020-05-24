#include"game.h"
#include"person.h"

extern int population, healthy, exposed, infected, dead, quarantine, medicalStuff, threshold;
extern int money, costPerBed, moneyPerPerson, costPerBedPerDay;
extern int mask, maskConsumptionMedical, maskConsumptionOrdinary, maskProduction;
extern int bedTotal, bedConsumption, hospitalResponse;
extern int day, vaccineReverseCnt, medicineReverseCnt;
extern double moveWill, broadRate;
extern bool quarantineCommandOn, medicineLock, flagLowerRate;
extern Person pool[2000];

int Over()
{
	if (dead + infected * 0.6 >= population * 0.5 || money < -500) //lose
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
	broadRate = 0.8;
	day = 0;
	healthy = 1990;
	exposed = 5;
	infected = 5;
	dead = 0;
	quarantine = 0;
	medicalStuff = 200;
	hospitalResponse = 5;
	bedTotal = 50;
	bedConsumption = 0;
	costPerBed = 40;
	threshold = 5;
	money = 100000;
	moneyPerPerson = 2;
	costPerBedPerDay = 10;
	maskConsumptionMedical = 2;
	maskConsumptionOrdinary = 0;
	maskProduction = 2;
	mask = 10000;
	vaccineReverseCnt = 210;
	medicineReverseCnt = 120;
	quarantineCommandOn = false;
	medicineLock = false;
	flagLowerRate = false;
}

void NewDay()
{
	day++;
	vaccineReverseCnt--;

	if (!medicineLock)
	{
		medicineReverseCnt--;
		if (!medicineReverseCnt)
		{
			medicineLock = true;
		}
	}

	if (mask < medicalStuff + population - dead)
	{
		if (mask < 0)
		{
			mask = 0;
		}
		int shortage = medicalStuff + population - dead - mask;
		broadRate = (shortage * 0.8 + mask * broadRate) / (medicalStuff + population - dead);
		if (broadRate > 0.8)
		{
			broadRate = 0.8;
		}
	}

	int tmp = 0;
	bool flag = false;

	for (auto &person : pool)
	{
		tmp = person.condition;

		if (tmp == 3)
		{
			continue;
		}
		else
		{
			if (medicineLock && !flagLowerRate)
			{
				flagLowerRate = true;
				person.deathRate /= 5;
			}
			flag = person.quarantine == 1;
			person.Move(person.position, flag);

			if (tmp == 1)
			{
				UpdateLurk(&person);
			}

			if (tmp == 2)
			{
				UpdateInfected(&person);
				UpdateInHospital(&person);
				HospitalReception(&person);
				Quarantine(quarantineCommandOn, &person);
				UpdateDeathAndRecovery(&person);
			}
		}
	}

	money += (population - dead - quarantine) * moneyPerPerson; //隔离的人不带来经济效益
	money -= bedTotal * costPerBedPerDay;
	mask -= medicalStuff * maskConsumptionMedical;
	mask -= (population - dead) * maskConsumptionOrdinary;
	mask += (population - dead - quarantine) * maskProduction;

	Contact();
}
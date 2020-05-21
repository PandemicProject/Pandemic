#include "person.h"
#include <random>
#include <graphics.h>
using namespace std;
default_random_engine e;

//position in quarantine zone
uniform_int_distribution<int> xPos(0, 600);
uniform_int_distribution<int> yPos(640, 800);

//degree of cooperation (stay in quarantine)
uniform_int_distribution<int> cooperation(0, 9);

//die or recover
uniform_int_distribution<int> state(0, 9);

//move
uniform_int_distribution<int> step(-20, 20);

//init
normal_distribution<double> normal(300, 100);
uniform_int_distribution<int> cond(0, 1999);
uniform_int_distribution<int> lurkDay(2, 14);
uniform_int_distribution<int> disturbance(-5, 5); //随机扰动

const int WINDOW = 600;
bool flagLowerRate = false;
extern double moveWill; //should be average + noise
extern int day, vaccineReverseCnt, medicineReverseCnt;
extern int healthy, exposed, infected, dead, population, quarantine;
extern double deathRate, broadRate;
extern int threshold;
extern int hospitalResponse, bedTotal, bedConsumption;
extern int money, moneyPerPerson, costPerBedPerDay;
extern int mask, medicalStuff, maskConsumptionMedical, maskConsumptionOrdinary, maskProduction;
extern bool quarantineCommandOn, medicineLock;
extern Person pool[2000];

//判断坐标是否超出边界
bool Check(int x, int y)
{
	return (x > (WINDOW - 5) || y > (WINDOW - 5));
}

void _Move(int *arr, int flag)
{
	//判断是否死亡或隔离
	if (flag)
	{
		return;
	}

	int dx = 0, dy = 0;
	
	do
	{
		dx = lround(step(e) * moveWill);
		dy = lround(step(e) * moveWill);
	} while (Check(*arr + dx, *(arr + 1) + dy));

	*arr += dx;
	*(arr + 1) += dy;
	return;
}

void InitPerson()
{
	int _x = 0, _y = 0;

	for (int i = 0; i < 2000; i++)
	{
		//initialize people's position
		do
		{
			_x = lround(normal(e));
			_y = lround(normal(e));
		} while (Check(_x, _y));

		Person p = { {_x, _y}, 0, 0.2, 0, lurkDay(e), 0, 0, 0, 0, 0, 10 + disturbance(e), _Move }; //PARAMETER: deathRate, recoverThreshold
		pool[i] = p;
	}

	//initialize infected and exposed people
	int initCount;
	initCount = infected;

	while (initCount--)
	{
		int index = cond(e);
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
		int index = cond(e);
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

/*change the condition to "exposed" 
if a healthy person physically contacts an exposed or an infected person closely*/
void Contact()
{
	double distance;
	int i, j, tmp;
	uniform_int_distribution<int> uniform(0, 9);

	for (i = 0; i < population - 1; i++)
	{
		//不考虑死亡/隔离的人
		if (pool[i].condition == 3 || pool[i].quarantine == 1)
		{
			continue;
		}
		for (j = i + 1; j < population; j++)
		{
			distance = sqrt(pow((pool[i].position[0] - pool[j].position[0]), 2) + pow((pool[i].position[1] - pool[j].position[1]), 2)); //distance between two people
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

//from exposed to infected
void UpdateLurk(Person *person)
{
	person->lurkDayCnt++;
	if (person->lurkDayCnt == person->lurkThreshold)
	{
		person->condition = 2;
		person->lurkDayCnt = 0;
		person->infectedDayCnt = 0;
		exposed--;
		infected++;
	}
}

//some people recover and are discharged from the hospital
void UpdateInHospital(Person *person)
{
	if (person->inHospital)
	{
		person->hospitalReceptionCnt++;
	}
}

void UpdateInfected(Person *person)
{
	person->infectedDayCnt++;
}

//of all infected people
void UpdateDeathAndRecovery(Person *person)
{
	int flag;
	uniform_int_distribution<int> uniform(0, 999);
	flag = uniform(e);
	if (!person->inHospital)
	{
		if (person->infectedDayCnt >= person->recoverThreshold)
		{
			if (flag < 1000 * person->deathRate) //die
			{
				person->condition = 3;
				dead++;
				infected--;
			}
			else //recover
			{
				person->condition = 0;
				person->infectedDayCnt = 0;
				healthy++;
				infected--;

				//康复后不再隔离
				if (person->quarantine)
				{
					person->quarantine = 0; 
					quarantine--;
				}
			}
		}
	}
	else
	{
		if (person->hospitalReceptionCnt >= person->outOfHospitalThreshold)
		{
			if (flag < 1000 * person->deathRate)
			{
				person->condition = 3;
				dead++;
				infected--;
			}
			else
			{
				person->condition = 0;
				person->inHospital = 0;
				person->quarantine = 0;
				person->infectedDayCnt = 0;
				person->hospitalReceptionCnt = 0;
				quarantine--;
				healthy++;
				infected--;
			}
			bedConsumption--;
		}
	}
}

//infected people are recepted into the hospital
void HospitalReception(Person *p)
{
	int flag;
	
	if ((p->infectedDayCnt >= hospitalResponse && bedConsumption < bedTotal) && !p->inHospital) //入院条件
	{
		flag = state(e);
		if (flag < 2) //重症
		{
			p->outOfHospitalThreshold = 25;
			p->deathRate *= 2; //PARAMETER
		}
		else //轻症
		{
			p->outOfHospitalThreshold = 14;
		}

		bedConsumption++;
		if (!p->quarantine) //exclude those who are already kept in quarantine
		{
			quarantine++;
			p->quarantine = 1;
		}
		p->inHospital = 1;
		p->deathRate /= 5; //PARAMETER
	}
}

//update condition for a new day
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

	if (mask < medicalStuff + population - dead) //缺失值 equals to # not wearing masks -> broadRate == 0.8 for these people (n) and remains the same for others 
	{
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

void Quarantine(bool commandOn, Person *person)
{
	if (!commandOn) //解除隔离
	{

		if (person->quarantine && !person->inHospital)
		{
			person->quarantine = 0;
			quarantine--;
		}
	}
	else
	{
		int flag = 0;
		
		if (person->quarantine)
		{
			return;
		}

		if (!person->inHospital) //infected but not recepted into hospital
		{
			flag = cooperation(e);
			//90% of these people will stay in quarantine under the "quarantine" command
			//PARAMETER
			if (flag < 9)
			{
				person->quarantine = 1;
				quarantine++;
			}
		}
	}
}
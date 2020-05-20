#include "person.h"
#include <random>
#include <graphics.h>
using namespace std;
default_random_engine e;

//position in quarantine zone
uniform_int_distribution<int> xPos(0, 600);
uniform_int_distribution<int> yPos(640, 680);

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

const int WINDOW = 600;
extern double moveWill; //should be average + noise
extern int healthy, exposed, infected, dead, population;
extern double deathRate, broadRate;
extern int threshold;
extern int hospitalResponse, bedTotal, bedConsumption;
extern int money, moneyPerPerson, costPerBedPerDay;
extern int mask, medicalStuff, maskConsumptionMedical, maskConsumptionOrdinary;
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
	int dx, dy;
	
	do
	{
		dx = lround(step(e) * moveWill);
		dy = lround(step(e) * moveWill);
	} while (Check(*arr + dx, *(arr + 1) + dy));

	*arr += dx;
	*(arr + 1) += dy;
	/* if (*arr > 590)
	{
		*arr -= 2 * dx;
	}
	if (*(arr + 1) > 590)
	{
		*arr -= 2 * dy;
	} */
	return;
}

void InitPerson()
{
	int _x, _y;

	for (int i = 0; i < 2000; i++)
	{
		//initialize people's position
		do
		{
			_x = lround(normal(e));
			_y = lround(normal(e));
		} while (Check(_x, _y));

		if (_x > WINDOW)
		{
			_x = WINDOW - 5;
		}

		if (_y > WINDOW)
		{
			_y = WINDOW - 5;
		}

		Person p = { {_x, _y}, 0, 0.05, 0, lurkDay(e), 0, 0, 0, 0, 0, 10, _Move };
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
	if (person->condition == 3)
	{
		return;
	}
	else
	{
		if (person->inHospital)
		{
			person->hospitalReceptionCnt++;
			if (person->hospitalReceptionCnt >= person->outOfHospitalThreshold) //康复出院
			{
				bedConsumption--;
				person->quarantine = 0;
				person->condition = 0;
				person->inHospital = 0;
			}
		}
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
			if (flag < 1000 * person->deathRate)
			{
				person->condition = 3;
				dead++;
				infected--;
			}
			else
			{
				person->condition = 0;
				healthy++;
				infected--;

				//康复后不再隔离
				if (person->quarantine)
				{
					person->quarantine = 0; 
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
				person->quarantine = 0;
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
	
	if (p->infectedDayCnt >= hospitalResponse && bedConsumption < bedTotal && !p->inHospital) //入院条件
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
		p->quarantine = 1;
		p->inHospital = 1;
		p->deathRate /= 5; //PARAMETER
	}
}

//update condition for a new day
void NewDay()
{
	for (auto &person : pool)
	{
		int tmp = person.condition;
		if (tmp == 3)
		{
			continue;
		}
		else
		{
			int flag = person.condition == 3 || person.quarantine == 1;
			person.Move(person.position, flag);
			if (tmp == 1)
			{
				UpdateLurk(&person); //更新潜伏情况
			}
			else if (tmp == 2)
			{
				UpdateInfected(&person);
				HospitalReception(&person);
				UpdateDeathAndRecovery(&person);
				UpdateInHospital(&person);
			}
		}
	}
	money += (healthy + exposed) * moneyPerPerson;
	money -= bedTotal * costPerBedPerDay;
	mask -= medicalStuff * maskConsumptionMedical;
	mask -= (population - dead) * maskConsumptionOrdinary;
}

void StayInQuarantine()
{
	int flag;
	for (auto &p : pool)
	{
		if (p.condition == 2 && !p.inHospital) //infected but not recepted into hospital
		{
			flag = cooperation(e);
			//90% of these people will stay in quarantine under the "quarantine" command
			//PARAMETER
			if (flag < 9)
			{
				p.quarantine = 1;
			}
		}

		//放入隔离区
		if (!p.quarantine)
		{
			return;
		}
		else
		{
			p.position[0] = xPos(e);
			p.position[1] = yPos(e);
		}
	}
}
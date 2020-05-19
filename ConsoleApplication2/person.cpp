#include "person.h"
#include <random>
#include <graphics.h>
using namespace std;
default_random_engine e;
const int WINDOW = 600;
extern double moveWill; //should be average + noise
extern int population,healthy, exposed, infected, dead;
extern double deathRate, broadRate;
extern int threshold;
extern int hospitalResponse;
extern int bedTotal, bedConsumption;
extern int money, moneyPerPerson, costPerBedPerDay;
extern Person pool[2000];

bool Check(int x, int y)
{
	if (x > (WINDOW - 5) || y > (WINDOW - 5))
	{
		return false;
	}
	else
	{
		return true;
	}
}

void _Move(int *arr)
{
	uniform_int_distribution<int> uniform(-20, 20);
	int dx = 0, dy = 0;

	do
	{
		dx = lround(uniform(e) * moveWill);
		dy = lround(uniform(e) * moveWill);
	} while (!Check(*arr + dx, *(arr + 1) + dy));

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
	normal_distribution<double> normal(300, 100);
	uniform_int_distribution<int> uniform(0, population - 1);
	uniform_int_distribution<int> lurkDay(2, 14);
	int _x, _y;

	for (int i = 0; i < 2000;i++)
	{
		//initialize people's position
		do
		{
			_x = lround(normal(e));
			_y = lround(normal(e));
		} while (!Check(_x, _y));

		if (_x > WINDOW)
		{
			_x = WINDOW;
		}

		if (_y > WINDOW)
		{
			_y = WINDOW;
		}

		Person p = { {_x, _y}, 0, 0.05, 0, lurkDay(e), 0, 0, 10, 0, 7, _Move };
		pool[i] = p;
	}

	//initialize infected and exposed people
	int initCount;
	initCount = infected;
	while (initCount--)
	{
		int index = uniform(e);
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
		int index = uniform(e);
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
			double distance = sqrt(pow((pool[i].position[0] - pool[j].position[0]), 2) + pow((pool[i].position[1] - pool[j].position[1]), 2)); //distance between two people
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

void UpdateInHospital(Person *person)
{
	if (person->condition == 3)
	{
		return;
	}
	else
	{
		if (person->quarantine)
		{
			person->hospitalReceptionCnt++;
			if (person->hospitalReceptionCnt >= person->outOfHospitalThreshold) //康复出院
			{
				bedConsumption--;
				person->quarantine = 0;
				person->condition = 0;
			}
		}
	}
}

void UpdateInfected(Person *person)
{
	person->infectedDayCnt++;
}

void UpdateDeathAndRecovery(Person *person)
{
	int flag;
	uniform_int_distribution<int> uniform(0, 999);
	flag = uniform(e);
	if (!person->quarantine)
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

void HospitalReception(Person *p)
{
	int flag;
	uniform_int_distribution<int> state(0, 9);
	if (p->infectedDayCnt >= hospitalResponse && bedConsumption < bedTotal && p->quarantine == 0)
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
		p->deathRate /= 5; //PARAMETER
	}
}

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
			person.Move(person.position);
			if (tmp == 1) //潜伏期
			{
				UpdateLurk(&person); //更新潜伏情况
			}
			else if (tmp == 2) //出现症状
			{
				UpdateInfected(&person);
				HospitalReception(&person);
				UpdateDeathAndRecovery(&person);
			}
		}
	}
	money += (healthy + exposed) * moneyPerPerson;
	money -= bedTotal * costPerBedPerDay;
}

void StayInQuarantine()
{
	uniform_int_distribution<int> willingness(0, 9);
	int flag;
	for (auto p : pool)
	{
		if (p.condition == 2) 
		{
			flag = willingness(e);
			//90% of infected people stay in quarantine
			//PARAMETER
			if (flag < 9)
			{
				p.quarantine = 1;
			}
		}
	}
}
#ifndef PERSON_H
#define PERSON_H

typedef struct
{
	int position[2]; //(x, y)
	int condition;   //0: healthy, 1: exposed, 2: infected 3: dead
	//int age;
	double deathRate; //死亡率
	int lurkDayCnt;   //潜伏天数
	int lurkThreshold;
	int infectedDayCnt;
	int hospitalReceptionCnt;    //住院时长
	int outOfHospitalThreshold; //出院
	int quarantine;              //1 in quarantine, 0: not in quarantine
	int recoverThreshold;
	void(*Move)(int *);
} Person;

bool Check(int x, int y);
void _Move(int *arr);
void InitPerson();
void Contact();
void StayInQuarantine();
void UpdateLurk(Person *person);
void UpdateInHospital(Person *person);
void UpdateInfected(Person *person);
void UpdateDeathAndRecovery(Person *person);
void HospitalReception(Person *person);
void NewDay();
#endif


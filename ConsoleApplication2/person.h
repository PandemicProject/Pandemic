#ifndef PERSON_H
#define PERSON_H

typedef struct
{
	int position[2]; //(x, y)
	int condition;   //0: healthy, 1: exposed, 2: infected 3: dead
	//int age;
	double deathRate; //������
	int lurkDayCnt;   //Ǳ������
	int lurkThreshold;
	int infectedDayCnt;
	int hospitalReceptionCnt;    //סԺʱ��
	int outOfHospitalThreshold; //��Ժ
	int quarantine;              //1 in quarantine, 0: not in quarantine
	int inHospital;//1 in hospital, 0 not in hospital
	int recoverThreshold;
	void(*Move)(int *, int);
} Person;

bool Check(int x, int y);
void _Move(int *arr, int flag);
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


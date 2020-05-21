#include<graphics.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<math.h>
#include<sys_edit.h>
#include"interaction.h"
#include"higiene.h"
#include"person.h"
//TODO: build & research + sum
//use vector to store the data

const int commandCnt = 12;
char build[] = "build", quarantine[] = "quarantine", work[] = "work", counterWork[] = "rm work", research[] = "research",
quit[] = "quit", mask[] = "mask", distance[] = "distance", counterMask[] = "rm mask", counterQuarantine[] = "rm quarantine",
newgame[] = "new game", exitgame[] = "exit game";
char *commandList[12] = { build, quarantine, work, counterWork, research, quit, mask, distance, counterMask, counterQuarantine, newgame, exitgame };
const int buffsize = 100;

//make sure these commands are only executed once
bool maskFlag = false, distanceFlag = false, quarantineFlag = false, workFlag = true, hospitalFlag = false, researchFlag = false;

extern double broadRate, moveWill;
extern int money, costPerBed, moneyPerPerson, maskConsumptionOrdinary, maskProduction;
extern int vaccineReverseCnt, medicineReverseCnt;
extern bool quarantineCommandOn, medicineLock;

int Min(int a, int b)
{
	return a < b ? a : b;
}

//最小编辑距离，处理用户拼写错误
void EditDistance(char *input, char**re)
{
	int dp[15][15];
	int flag = 0, now = 0xff, i, j, k;

	for (k = 0; k < commandCnt; k++)
	{
		char *target = commandList[k];
		//insert a 0 at the beginning
		char in[15] = { '\0' }, command[15] = { '\0' };
		in[0] = '0', command[0] = '0';
		strcat(in, input);
		strcat(command, target);
		int m = strlen(in);
		int n = strlen(command);

		for (i = 0; i < m; i++)
		{
			dp[i][0] = i;
		}

		for (i = 0; i < n; i++)
		{
			dp[0][i] = i;
		}

		for (i = 1; i < m; i++)
		{
			for (j = 1; j < n; j++)
			{
				flag = 1 - (in[i] == command[j]);
				dp[i][j] = Min(dp[i - 1][j] + 1, Min(dp[i][j - 1] + 1, dp[i - 1][j - 1] + flag));
			}
		}

		if (dp[m - 1][n - 1] <= now)
		{
			*re = target;
			now = dp[m - 1][n - 1];
		}
	}
}

//handle \n, \t and number
bool Text(char *s, int &len)
{
	s[len - 1] = '\0';
	s[len - 2] = '\0';
	len -= 2;
	int i;
	
	bool flag = true;
	//判断是否为数字
	for (i = 0; i < len; i++)
	{
		if (!isdigit(s[i]))
		{
			flag = false;
		}
		else
		{
			continue;
		}
	}
	return flag;
}

//command
int Console()
{
	sys_edit box;
	box.create(true); 
	box.move(610, 300);    //position
	box.size(170, 120);    //size
	box.setbgcolor(BLACK); //background color
	box.setcolor(WHITE);   //text color
	box.setfont(16, 0, "Fira Code");
	box.visable(true); 
	box.setfocus(); //光标闪烁

	bool flagPress = false; //标记是否按下
	bool flagDigit;
	char buff[100] = { '\0' };
	int buffLength = 0, height = 0, i;
	for (; is_run(); delay_fps(60))
	{
		if (keystate(key_enter)) //检测到按下回车键就获取输入内容
		{
			if (!flagPress)
			{
				try
				{
					box.gettext(buffsize, buff); //获取输入框内容，保存在buff中
					
					for (i = 0; i < 98; i++)
					{
						if (buff[i] < 0) //detect Mandarine input 
						{
							throw false;
						}
					}
					
					buffLength = strlen(buff);
					flagDigit = Text(buff, buffLength);

					if (!buffLength) //detect blank input
					{
						throw 0;
					}

					if (!flagDigit)
						{
							if (!strcmp(buff, "quit"))
							{
								Sleep(500);
								return 2;
							}
							else if (!strcmp(buff, "build")) //build hospital
							{
								Sleep(1500);
								hospitalFlag = true;
								box.settext("Input the number of hospital beds: ");
							}
							else if (!strcmp(buff, "quarantine"))
							{
								Sleep(1500);
								if (!quarantineCommandOn)
								{
									quarantineCommandOn = true;
									box.settext("Infected people start to be kept in quarantine.");
								}
								else
								{
									box.settext("Invalid command. Infected people are already kept in quarantine.");
								}
							}
							else if (!strcmp(buff, "rm quarantine"))
							{
								Sleep(1500);
								if (quarantineCommandOn)
								{
									quarantineCommandOn = false;
									box.settext("The quarantine measure is withdrawn.");
								}
								else
								{
									box.settext("Invalid command. The quarantine measure is not initiated.");
								}
							}
							else if (!strcmp(buff, "work")) //back to work
							{
								Sleep(1500);
								if (!workFlag)
								{
									moneyPerPerson *= 2; //PARAMETER
									moveWill += 0.2; //PARAMETER
									workFlag = true;
									box.settext("People are getting back to work");
								}
								else
								{
									box.settext("Invalid command. People are already back to work.");
								}
							}
							else if (!strcmp(buff, "rm work"))
							{
								Sleep(1500);
								if (workFlag)
								{
									moneyPerPerson /= 2;//PARAMETER
									moveWill -= 0.2;//PARAMETER
									maskProduction = 1; //PARAMETER
									workFlag = false;
									box.settext("People stop working.");
								}
								else
								{
									box.settext("Invalid command. People are not working.");
								}
							}
							else if (!strcmp(buff, "mask")) //wear mask
							{
								Sleep(1500);
								if (!maskFlag)
								{
									maskFlag = true;
									broadRate /= 2; //PARAMETER
									maskConsumptionOrdinary = 1;
									box.settext("People begin to wear masks.");
								}
								else
								{
									box.settext("Invalid command. People are alreay wearing masks.");
								}
							}
							else if (!strcmp(buff, "rm mask")) //remove mask
							{
								Sleep(1500);
								if (!maskFlag)
								{
									box.settext("People are not wearing masks.");
								}
								else
								{
									maskFlag = false;
									broadRate = 2;
									maskConsumptionOrdinary = 0;
									box.settext("People stop wearing masks");
								}
							}
							else if (!strcmp(buff, "distance")) //call for social distancing
							{
								Sleep(1500);
								if (!distanceFlag)
								{
									distanceFlag = true;
									moveWill /= 3; //PARAMETER
									box.settext("People begin to carry out social distancing.");
								}
								else
								{
									box.settext("Invalid command. People are already carrying out social distancing.");
								}
							}
							else if (!strcmp(buff, "research")) //fund research
							{
								//not sure if the user can fund research for more than once
								Sleep(1500);
								researchFlag = true;
								box.settext("Input the amount of funding: ");
								//if (money > 5000)
								//{
								//	money -= 5000; //PARAMETER
								//	box.settext("Research teams are racing to develop medication and vaccine.");
								//}
								//else
								//{
								//	box.settext("Research appropriation failed. Not enough money.");
								//}
							}
							else if (!strcmp(buff, "new game")) //重新开始游戏
							{
								maskFlag = false;
								distanceFlag = false;
								quarantineFlag = false;
								workFlag = false;
								hospitalFlag = false;
								box.destory();
								return 1;
							}
							else if (!strcmp(buff, "exit game")) //退出游戏
							{
								box.destory();
								return 0;
							}
							else 
							{
								char *tmp = buff;
								EditDistance(buff,&tmp);
								char warning[50] = "Ambiguous command. Do you mean ";
								strcat(warning, tmp);
								strcat(warning, "?");
								Sleep(300);
								box.settext(warning);
							} //spelling mistake
						}
					else //number
						{
							Sleep(1500);
							if (hospitalFlag) //之前输入了build
							{
								int num = atoi(buff);
								bool constructionFlag = num * costPerBed < money; //判断是否有足够金额

								if (constructionFlag)
								{
									InitHospital(num);
									money -= (num * costPerBed);
									box.settext("Hospital construction successful.");
								}
								else
								{
									box.settext("Hospital Construction failed. Not enough money.");
								}
								hospitalFlag = false;
							}
							else
							{
								box.settext("Undefined command. Please input \"build\" first if you want to add hospital beds.");
							}

							if (researchFlag)
							{
								int num = atoi(buff);
								bool fundFlag = num < money;

								if (fundFlag)
								{
									money -= num;
									if (!medicineLock)
									{
										int tailorMedicine = lround(sqrt(num)) / 2;
										int tailorVaccine = lround(sqrt(num)) / 2;
										vaccineReverseCnt -= tailorVaccine;
										medicineReverseCnt -= tailorMedicine;
										box.settext("Medicine and vaccine development has been accelerated.");
									}
									else
									{
										int tailorVaccine = lround(sqrt(num));
										vaccineReverseCnt -= tailorVaccine;
										box.settext("Medicine has been developed. Vaccine development has been accelerated.");
									}
									
								}
								else
								{
									box.settext("Research development acceleration failed. Not enough money.");
								}

								researchFlag = false;
							}
						}
					Sleep(1500);
					box.settext("");
				}
				catch (int)
				{
					box.settext("Blank command.");
					Sleep(1000);
					box.settext("");
				}
				catch (bool)
				{
					box.settext("The game does not support Mandarine.");
					Sleep(1000);
					box.settext("");
				}

				flagPress = 1;
			}
			else
			{
				flagPress = 0;
			}
		}	
	}
}
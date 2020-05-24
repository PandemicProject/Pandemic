#include<graphics.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<math.h>
#include<sys_edit.h>
#include"interaction.h"

const int commandCnt = 12;
char build[] = "build", quarantine[] = "quarantine", work[] = "work", counterWork[] = "rm work", research[] = "research",
quit[] = "quit", mask[] = "mask", distance[] = "distance", counterMask[] = "rm mask", counterQuarantine[] = "rm quarantine",
newgame[] = "new", exitgame[] = "exit";
char *commandList[12] = { build, quarantine, work, counterWork, research, quit, mask, distance, counterMask, counterQuarantine, newgame, exitgame };
const int buffsize = 120;

//make sure these commands are only executed once
bool maskFlag = false, distanceFlag = false, quarantineFlag = false, workFlag = true;

extern double broadRate, moveWill;
extern int money, bedTotal, costPerBed, moneyPerPerson, maskConsumptionOrdinary, maskProduction;
extern int vaccineReverseCnt, medicineReverseCnt;
extern bool quarantineCommandOn, medicineLock;

int Min(int a, int b)
{
	return a < b ? a : b;
}

//处理用户拼写错误
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

int Segment(char *input, char(*command)[15], const int len)
{
	int cnt = 0;
	const char split[] = " ";
	char *token;
	token = strtok(input, split);
	while (token != NULL)
	{
		strcpy(command[cnt++], token);
		token = strtok(NULL, split);
	}
	return cnt;
}

void DelSpace(char *s, char *dest)
{
	int i, j, len = strlen(s), cnt = 0;

	//delete /r and /n
	s[len - 1] = { '\0' };
	s[len - 2] = { '\0' };

	//delete spaces at the end of the input string
	for (i = strlen(s) - 1; i >= 0; i--)
	{
		if (!isalpha(s[i]) && !isdigit(s[i]))
		{
			s[i] = '\0';
		}
		else
		{
			break;
		}
	}

	//delete spaces at the beginning
	while (*s == 32)
	{
		s++;
	}

	//delete redundant spaces in the middle of the input for later segmentation
	for (i = 0; i < strlen(s); i++)
	{
		if (isalpha(s[i]))
		{
			if (isupper(s[i]))
			{
				s[i] = tolower(s[i]); //转小写
			}
		}

		if (s[i] == 32 && s[i + 1] == 32)
		{
			continue;
		}

		dest[cnt++] = s[i];
	}
}

bool IsDigit(char *s)
{
	int i;
	for (i = 0; i < strlen(s); i++)
	{
		if (!isdigit(s[i]))
		{
			return false;
		}
	}
	return true;
}

int Console()
{
	sys_edit box;
	box.create(true); 
	box.move(610, 560);    //position
	box.size(300, 90);    //size
	box.setbgcolor(BLACK); //background color
	box.setcolor(WHITE);   //text color
	box.setfont(16, 0, "Times New Roman");
	box.visable(true); 
	box.setfocus(); //光标

	bool flagPress = false; //标记是否按下
	char buff[120] = { '\0' }; //缓存区
	char command[5][15] = { '\0' }; //命令
	int buffLength = 0, height = 0, i, cnt = 0;
	for (; is_run(); delay_fps(60))
	{
		if (keystate(key_enter)) //检测到按下回车键就获取输入内容
		{
			if (!flagPress)
			{
				box.gettext(buffsize, buff); //获取输入框内容，保存在buff中
				try
				{
					for (i = 0; i < 98; i++)
					{
						if (buff[i] < 0) //中文输入
						{
							throw false;
						}
					}

					char input[120] = { '\0' };
					DelSpace(buff, input);
					buffLength = strlen(input);

					if (!buffLength) //空白输入
					{
						throw 0;
					}

					cnt = Segment(input, command, strlen(input));
					if (!strcmp(command[0], "quit"))
					{
						try
						{
							if (cnt != 1)
							{
								throw false;
							}
							Sleep(500);
							return 2;
						}
						catch (bool)
						{
							Sleep(500);
							box.settext("Invalid command. No parameter needed.");
						}
					}
					else if (!strcmp(command[0], "build"))
					{
						try
						{
							if (cnt != 2)
							{
								throw false;
							}

							if (!IsDigit(command[1]))
							{
								throw false;
							}

							if (strlen(command[1]) > 1 && command[1][0] == '0')
							{
								throw 0;
							}


							int num = atoi(command[1]);

							if (num < 0)
							{
								throw false;
							}

							bool constructionFlag = num * costPerBed < money; //判断是否有足够金额
							Sleep(1000);
							if (constructionFlag)
							{
								bedTotal += num;
								money -= (num * costPerBed);
								box.settext("Hospital construction successful.");
							}
							else
							{
								throw 'f';
							}
						}
						catch (int)
						{
							Sleep(500);
							box.settext("The number begins with a zero.");
						}
						catch (bool)
						{
							Sleep(500);
							box.settext("Invalid input. Format: build [number]. e.g. build 100");
						}
						catch (char)
						{
							box.settext("Hospital Construction failed. Not enough money.");
						}
					}
					else if (!strcmp(command[0], "research"))
					{
						try
						{
							if (cnt != 3)
							{
								throw false;
							}
							//非数字
							if (!IsDigit(command[1]) || !IsDigit(command[2]))
							{
								throw false;
							}
							//首位为0
							if ((strlen(command[1]) > 1 && command[1][0] == '0') || (strlen(command[2]) > 1 && command[2][0] == '0'))
							{
								throw 0;
							}
							

							int num1 = atoi(command[1]); //medicine
							int num2 = atoi(command[2]); //vaccine

							if (num1 < 0 || num2 < 0)
							{
								throw false;
							}

							if (medicineLock && num1 > 0)
							{
								throw 1.0;
							}

							int total = num1 + num2;
							bool fundFlag = total < money;
							Sleep(1000);
							if (fundFlag)
							{
								money -= total;
								if (!medicineLock)
								{
									int tailorMedicine = lround(sqrt(num1 / 2));
									int tailorVaccine = lround(sqrt(num2) / 2);
									vaccineReverseCnt -= tailorVaccine;
									medicineReverseCnt -= tailorMedicine;
									if (vaccineReverseCnt < 0)
									{
										vaccineReverseCnt = 0;
									}
									if (medicineReverseCnt < 0)
									{
										medicineReverseCnt = 0;
										medicineLock = true;
									}
									box.settext("Medicine and vaccine development has been accelerated.");
								}
								else
								{
									int tailorVaccine = lround(sqrt(num2) / 2);
									vaccineReverseCnt -= tailorVaccine;
									if (vaccineReverseCnt < 0)
									{
										vaccineReverseCnt = 0;
									}
									box.settext("Medicine has been developed. Vaccine development has been accelerated.");
								}
							}
							else
							{
								throw 'f';
							}
						}
						catch (int)
						{
							Sleep(500);
							box.settext("The number begins with a zero.");
						}
						catch (bool)
						{
							Sleep(500);
							box.settext("Invalid input. Format: research [number] [number]. e.g. research 3000 0.");
						}
						catch (char)
						{
							box.settext("Research development acceleration failed. Not enough money.");
						}
						catch (double)
						{
							box.settext("Medicine has been developed. No need for funding.");
						}
					}
					else if (!strcmp(command[0], "work"))
					{
						try
						{
							if (cnt != 1)
							{
								throw false;
							}

							Sleep(1000);
							if (!workFlag)
							{
								moneyPerPerson *= 2; //PARAMETER
								moveWill += 0.2; //PARAMETER
								if (moveWill > 1)
								{
									moveWill = 1;
								}
								workFlag = true;
								box.settext("People are getting back to work");
							}
							else
							{
								throw 0;
							}
						}
						catch (bool)
						{
							Sleep(500);
							box.settext("Invalid command. No parameter needed.");
						}
						catch (int)
						{
							box.settext("Invalid command. People are already back to work.");
						}
					}
					else if (!strcmp(command[0], "quarantine"))
					{
						try
						{
							if (cnt != 1)
							{
								throw false;
							}

							Sleep(1000);
							if (!quarantineCommandOn)
							{
								quarantineCommandOn = true;
								box.settext("Infected people start to be kept in quarantine.");
							}
							else
							{
								throw 0;
							}
						}
						catch (bool)
						{
							Sleep(500);
							box.settext("Invalid command. No parameter needed.");
						}
						catch (int)
						{
							box.settext("Invalid command. Infected people are already kept in quarantine.");
						}
					}
					else if (!strcmp(command[0], "mask"))
					{
						try
						{
							if (cnt != 1)
							{
								throw false;
							}

							Sleep(1000);
							if (!maskFlag)
							{
								maskFlag = true;
								broadRate /= 2; //PARAMETER
								maskConsumptionOrdinary = 1;
								box.settext("People begin to wear masks.");
							}
							else
							{
								throw 0;
							}
						}
						catch (bool)
						{
							Sleep(500);
							box.settext("Invalid command. No parameter needed.");
						}
						catch (int)
						{
							box.settext("Invalid command. People are alreay wearing masks.");
						}
					}
					else if (!strcmp(command[0], "distance"))
					{
						try
						{
							if (cnt != 1)
							{
								throw false;
							}

							Sleep(1000);
							if (!distanceFlag)
							{
								distanceFlag = true;
								moveWill /= 3; //PARAMETER
								box.settext("People begin to carry out social distancing.");
							}
							else
							{
								throw 0;
							}
						}
						catch (bool)
						{
							Sleep(500);
							box.settext("Invalid command. No parameter needed.");
						}
						catch (int)
						{
							box.settext("Invalid command. People are already carrying out social distancing.");
						}
					}
					else if (!strcmp(command[0], "rm"))
					{
						try
						{
							if (cnt != 2)
							{
								throw false;
							}

							if (!strcmp(command[1], "work"))
							{
								Sleep(1000);
								if (workFlag)
								{
									moneyPerPerson = 1;//PARAMETER
									moveWill -= 0.2;//PARAMETER
									if (moveWill < 0)
									{
										moveWill = 0.1;
									}
									maskProduction = 0; //PARAMETER
									workFlag = false;
									box.settext("People stop working.");
								}
								else
								{
									throw 0;
								}
							}
							else if (!strcmp(command[1], "mask"))
							{
								Sleep(1000);
								if (!maskFlag)
								{
									throw 'f';
								}
								else
								{
									maskFlag = false;
									broadRate *= 2;
									if (broadRate >= 0.8)
									{
										broadRate = 0.8;
									}
									maskConsumptionOrdinary = 0;
									box.settext("People stop wearing masks");
								}
							}
							else if (!strcmp(command[1], "quarantine"))
							{
								Sleep(1500);
								if (!quarantineCommandOn)
								{
									quarantineCommandOn = true;
									box.settext("Infected people start to be kept in quarantine.");
								}
								else
								{
									throw 1.0;
								}
							}
						}
						catch (bool)
						{
							Sleep(500);
							box.settext("\"rm\" needs to be followed by the command you want to remove.");
						}
						catch (int)
						{
							box.settext("Invalid command. People are not working.");
						}
						catch (char)
						{
							box.settext("People are not wearing masks.");
						}
						catch (double)
						{
							box.settext("Invalid command. Infected people are already kept in quarantine.");
						}
					}
					else if (!strcmp(command[0], "new"))
					{
						try
						{
							if (cnt != 1)
							{
								throw false;
							}

							maskFlag = false;
							distanceFlag = false;
							quarantineFlag = false;
							workFlag = true; 
							box.destory();
							return 1;
						}
						catch (bool)
						{
							Sleep(500);
							box.settext("Invalid restarting command.");
						}
					}
					else if (!strcmp(command[0], "exit"))
					{
						try
						{
							if (cnt != 1)
							{
								throw false;
							}

							box.destory();
							return 0;
						}
						catch (bool)
						{
							Sleep(500);
							box.settext("Invalid exiting command.");
						}
					}
					else
					{
						char *tmp = input;
						EditDistance(input, &tmp);
						char warning[50] = "Ambiguous command. Do you mean \" ";
						strcat(warning, tmp);
						strcat(warning, "\"?");
						Sleep(500);
						box.settext(warning);
						Sleep(500);
					}
				}
				catch (bool)
				{
					Sleep(1000);
					box.settext("The game does not support Chinese input.");
				}
				catch (int)
				{
					Sleep(1000);
					box.settext("Blank input.");
				}
				flagPress = 1;
				Sleep(1000);
				box.settext("");
			}
			else
			{
				flagPress = 0;
			}
		}	
	}
}
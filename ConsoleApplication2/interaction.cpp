#include<graphics.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<sys_edit.h>
#include"interaction.h"
#include"higiene.h"
#include"person.h"

const int commandCnt = 7;
char build[] = "build", quarantine[] = "quarantine", work[] = "work", research[] = "research",
quit[] = "quit", mask[] = "mask", distance[] = "distance";
char *commandList[7] = { build, quarantine, work, research, quit, mask, distance };
const int buffsize = 100;

//make sure these commands are only executed once
bool maskFlag = false;
bool distanceFlag = false;
bool quarantineFlag = false;
bool workFlag = false;

extern double broadRate, moveWill;
extern int money, costPerBed;

int Min(int a, int b)
{
	return a < b ? a : b;
}

//最小编辑距离，处理用户拼写错误
char *EditDistance(char *input)
{
	int dp[15][15];
	int flag = 0, now = 0xff, i, j, k;

	char *re = (char*)malloc(10);

	for (k = 0; k < commandCnt; k++)
	{
		char *target = commandList[k];
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
			re = target;
			now = dp[m - 1][n - 1];
		}
	}
	return re;
}

//handle \n, \t and number
bool Text(char *s, int &len)
{
	s[len - 1] = '\0';
	s[len - 2] = '\0';
	len -= 2;
	int i;
	bool flag = false;
	//判断是否为数字
	for (i = 0; i < len; i++)
	{
		if (isdigit(s[i]))
		{
			flag = true;
		}
	}
	if (flag)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//处理输入和输出
void Console()
{
	sys_edit box;
	box.create(true); //multiple line
	/* edit box parameter */
	box.move(610, 130);    //position
	box.size(160, 100);    //size
	box.setbgcolor(BLACK); //background color
	box.setcolor(WHITE);   //text color
	box.setfont(16, 0, "Fira Code");
	box.visable(true);
	box.setfocus(); //光标闪烁

	bool flagPress = false; //标记是否按下
	bool flagDigit;
	char buff[100] = { '\0' };
	int buffLength = 0, height = 0;
	for (; is_run(); delay_fps(60))
	{
		if (keystate(key_enter)) //检测到按下回车键就获取输入内容
		{
			if (!flagPress)
			{
				box.gettext(buffsize, buff); //获取输入框内容，保存在buff中
				buffLength = strlen(buff);
				flagDigit = Text(buff, buffLength);

				if (buffLength)
				{
					if (!flagDigit)
					{
						if (!strcmp(buff, "quit"))
						{
							Sleep(500);
							break;
						}
						else if (!strcmp(buff, "build"))
						{
							Sleep(1500);
							box.settext("Input the number of hospital beds: ");
						}
						else if (!strcmp(buff, "quarantine"))
						{
							Sleep(1500);
							if (!quarantineFlag)
							{
								StayInQuarantine();
								box.settext("Infected people start to be kept in quarantine.");
								//添加函数
							}
							else
							{
								box.settext("Invalid command. Infected people are already kept in quarantine.");
							}
						}
						else if (!strcmp(buff, "work"))
						{
							Sleep(1500);
							if (!workFlag)
							{
								//添加函数
								box.settext("People are getting back to work");
							}
							else
							{
								box.settext("Invalid command. People are already back to work.");
							}
						}
						else if (!strcmp(buff, "mask"))
						{
							Sleep(1500);
							if (!maskFlag)
							{
								maskFlag = true;
								broadRate /= 2; //PARAMETER
								box.settext("People begin to wear masks.");
							}
							else
							{
								box.settext("Invalid command. People are alreay wearing masks.");
							}
						}
						else if (!strcmp(buff, "distance"))
						{
							Sleep(1500);
							if (!distanceFlag)
							{
								distanceFlag = true;
								moveWill /= 5; //PARAMETER
								box.settext("People begin to carry out social distancing.");
							}
							else
							{
								box.settext("Invalid command. People are already carrying out social distancing.");
							}
						}
						else if (!strcmp(buff, "research"))
						{
							//not sure if the user can fund research for more than once
							Sleep(1500);
							money -= 5000; //PARAMETER
							if (money > 5000)
							{
								money -= 5000;
								box.settext("Research teams are racing to develop medication and vaccine.");
							}
							else
							{
								box.settext("Research appropriation failed. Not enough money.");
							}
						}
						else
						{
							char *tmp = EditDistance(buff);
							char warning[50] = "Ambiguous command. Do you mean ";
							strcat(warning, tmp);
							strcat(warning, "?");
							Sleep(300);
							box.settext(warning);
							free(tmp);
						}
					}
					else
					{
						Sleep(1500);
						int num = atoi(buff);
						bool hospitalFlag = true;

						if (num * costPerBed > money)
						{
							hospitalFlag = false;
						}

						if (hospitalFlag)
						{
							InitHospital(num);
							box.settext("Hospital construction successful.");
						}
						else
						{
							box.settext("Hospital Construction failed. Not enough money.");
						}
					}
					Sleep(1500);
					box.settext("");
				}
			}

			flagPress = 1;
		}
		else
		{
			flagPress = 0;
		}
	}
}
#include <graphics.h>
#include "opening.h"

const int square = 600;
const int marginWidth = 175;
const int marginHeight = 50;
const int life = 60;
int cell[60][60] = { 0 };

void Initialize()
{
	int i, j;
	for (i = 0; i < life; i++)
	{
		cell[i][0] = 1;
		cell[i][life - 1] = 1;

		for (j = 0; j < life; j++)
		{
			cell[0][j] = 1;
			cell[life - 1][j] = 1;
		}
	}
}

void Update()
{
	int neighbour = 0, i, j;
	int tmp[60][60];
	for (i = 0; i < life - 1; i++)
	{
		for (j = 0; j < life - 1; j++)
		{
			neighbour = cell[i - 1][j - 1] + cell[i - 1][j] + cell[i - 1][j + 1] + cell[i][j - 1] + cell[i][j + 1] + cell[i + 1][j - 1] + cell[i + 1][j] + cell[i + 1][j + 1];
			if (neighbour == 3)
			{
				tmp[i][j] = 1;
			}
			else if (neighbour == 2)
			{
				tmp[i][j] = cell[i][j];
			}
			else
			{
				tmp[i][j] = 0;
			}
		}
	}

	for (i = 1; i < life - 1; i++)
	{
		for (j = 1; j < life - 1; j++)
		{
			cell[i][j] = tmp[i][j];
		}
	}
}

void DisplayGame()
{
	cleardevice();
	int i, j;
	for (i = 0; i < life; i++)
	{
		for (j = 0; j < life; j++)
		{
			if (cell[i][j] == 1)
			{
				setcolor(WHITE);
				setfillcolor(WHITE);
			}
			else
			{
				setcolor(BLACK);
				setfillcolor(BLACK);
			}
			bar(marginWidth + i * 10, marginHeight + j * 10, marginWidth + (i + 1) * 10, marginHeight + (j + 1) * 10);
		}
	}
}

void GameOfLife()
{
	int alpha = 0x00; //͸����
	PIMAGE des = newimage();
	PIMAGE src = newimage();
	getimage(src, "pic2.png");

	Initialize();
	DisplayGame();
	Sleep(500);

	for (; is_run(); delay_fps(60))
	{
		Update();
		DisplayGame();
		if (kbhit())
		{
			char flag = getch();                          //don't know why not adding this leads to termination of the program
			for (; alpha <= 0x8a; delay_fps(60), alpha++) //����
			{
				getimage(des, 0, 0, getwidth(), getheight());
				putimage_alphablend(des, src, 0, 0, alpha, 0, 0);
				putimage(0, 0, des);
			}
			break;
		}
	}
	cleardevice();
	setfont(-30, 0, "Times New Roman");
	outtextxy(350, 300, "Against \n Pandemic"); //NEED REFINEMENT
	setfont(-20, 0, "Times New Roman");
	outtextxy(340, 350, "Press any key to enter the game");
	Sleep(500);
	delimage(des);
	delimage(src);
}
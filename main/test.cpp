#include <bits/stdc++.h>
using namespace std;

typedef struct
{
    int condition = 0;
} Person;

Person pool[10];

void ChangeCondition()
{
    int i;
    for (i = 0; i < 10; i++)
    {
        pool[i].condition++;
    }
    //return;
}

int main()
{
    ChangeCondition();
    for (int i = 0; i < 10; i++)
    {
        cout << pool[i].condition << endl;
    }
    return 0;
}
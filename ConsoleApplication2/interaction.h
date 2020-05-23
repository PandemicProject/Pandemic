#ifndef INTERACTION_H
#define INTERACTION_H

void EditDistance(char *input, char **re);
int Min(int a, int b);
//void Text(char *s, int &len);
bool IsDigit(char *s);
void DelSpace(char *s, char *dest);
int Segment(char *input, char(*command)[15], const int len);
int Console(); //new game: return 1, exit game: return 0, 2: nothing happens

#endif


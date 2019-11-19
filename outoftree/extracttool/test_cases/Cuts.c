#include <stdio.h>

int addOneX(int x)
{
  return x + 1;
}

int addOneY(int y)
{
  return y + 1;
}

int addTwoX(int x)
{
  return x + 2;
}

int addTwoY(int y)
{
  return y + 2;
}

int addThreeX(int x)
{
  return x + 3;
}

int addThreeY(int y)
{
  return y + 3;
}

int main()
{
  int x,y;
  x = 5;
  y = addOneX(x);
}

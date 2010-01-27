#include<stdlib.h>
#include<stdio.h>

#define FACT_NUM 10

#if 1
unsigned int fact(unsigned int param)
{

  if (param == 1) return 1;
  if (param == 2) return 2;
  return param * fact (param - 1);
}

int main()
{
  printf("Factorial of %d is %d\n", FACT_NUM, fact(FACT_NUM));
  return 0;
}
#else
int mul(int param1, int param2)
{
  return param1*param2;
}

int main()
{
    int i = 1, result = 1;
    for(i = 1; i <= 10; i++)
        result = mul(result, i);
    printf("Factorial of 10 is %d\n", result);
    return 0;
}
#endif

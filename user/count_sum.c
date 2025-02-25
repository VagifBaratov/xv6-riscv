#include "user/user.h"

int
main(int argc, char *argv[])
{
  char buff[1];
  char strNum[10];
  int nums[2];
  int argI = 0;
  int n;
  int i = 0;
  
  do 
  {
    if (argI >= 2)
    {
      fprintf(2, "Ошибка: некорректный формат\n");
      exit(1);
    }

    n = read(0, buff, 1);
    if(n < 0){
      fprintf(2, "Ошибка чтения\n");
      exit(1);
    }
    if (buff[0] == ' ' || buff[0] == '\n')
    {
      nums[argI] = atoi(strNum);
      argI++;
        
      for(int j = 0; j < i; j++)
        strNum[j] = ' ';
      i = 0;
    }
    else if (i >= 10)
    {
      fprintf(2, "Ошибка: слишком длинная строка\n");
      exit(1);
    }
    else 
    {
      strNum[i] = buff[0];
      i++;
    }
  } while(buff[0] != '\n');
  
  if (argI < 2)
  {
    fprintf(2, "Ошибка: некорректный формат\n");
    exit(1);
  }
    int sum = add(nums[0], nums[1]);
  printf("|%d %d|", nums[0], nums[1]);
  printf("%d", sum);
  exit(0);
}
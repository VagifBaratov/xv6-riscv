#include "user/user.h"

int
main(int argc, char *argv[])
{
  char buff;
  char strNum[11];
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

    n = read(0, &buff, 1);
    if(n <= 0){
      fprintf(2, "Ошибка чтения\n");
      exit(1);
    }

    if (buff == ' ' || buff == '\n')
    {
      strNum[i] = '\0';
      nums[argI] = atoi(strNum);
      argI++;
      i = 0;
    }
    else if (i > 10)
    {
      fprintf(2, "Ошибка: слишком большие числа\n");
      exit(1);
    }
    else if (buff > '9' || buff < '0') {
      fprintf(2, "Ошибка: некорректный формат\n");
      exit(1); 
    }
    else 
    {
      strNum[i] = buff;
      i++;
    }
  } while(buff != '\n');
  
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
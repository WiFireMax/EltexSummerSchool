#include <stdio.h>
int main(void)
{
 float x = 5.0;
 printf("x = %f, ", x);
 float y = 6.0;
 printf("y = %f\n", y);
 float *xp = &y; //вместо 5 надо 6, то есть указатель на y
 float *yp = &y;
 printf("Результат: %f\n", *xp + *yp);
}

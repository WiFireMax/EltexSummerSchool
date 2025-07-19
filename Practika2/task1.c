#include <stdio.h>
#define N 3

int main()
{
    int value = 1;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            printf("%d ", value++);
        }
        printf("\n");
    }
    return 0;
}

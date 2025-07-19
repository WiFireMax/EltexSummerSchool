#include <stdio.h>
#define N 3

int main()
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (i + j < N - 1)
                printf("0 ");
            else
                printf("1 ");
        }
        printf("\n");
    }
    return 0;
}

#include <stdio.h>

int main()
{
    int arr[10];
    for(int i = 0; i < 10; i++)
        arr[i] = i + 1;

    int *ptr = arr;
    int *end = arr + 10;
    while (ptr < end)
    {
        printf("%d ", *ptr);
        ptr++;
    }
    printf("\n");
    return 0;
}

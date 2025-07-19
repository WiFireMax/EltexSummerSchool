#include <stdio.h>
#define N 5

int main()
{
    int res[N][N];
    int value = 1;
    int top = 0, down = N - 1, left = 0, right = N - 1;

    while (top <= down && left <= right)
    {
        for (int j = left; j <= right; j++)
            res[top][j] = value++;
        top++;
        for (int i = top; i <= down; i++)
            res[i][right] = value++;
        right--;
        if (top <= down)
        {
            for (int j = right; j >= left; j--)
                res[down][j] = value++;
            down--;
        }
        if (left <= right)
        {
            for (int i = down; i >= top; i--)
                res[i][left] = value++;
            left++;
        }
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", res[i][j]);
        }
        printf("\n");
    }
    return 0;
}

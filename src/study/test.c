#include <stdio.h>

extern int add(int, int);

int sub(int a, int b)
{
    int c = a - b;
    return c;
}

int main()
{
    int i = 5;
    int j = 10;
    int result = add(i, j);

    printf("%d + %d = %d\n", i, j, result);
}
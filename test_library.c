#include <stdio.h>
int fibonacci(int number)
{
    fprintf(stderr, "%s:%d\n", __func__, number);
    switch (number)
    {
    case 0:
        return 1;
    case 1:
        return 1;
    default:
    {
        int a = fibonacci(number - 2);
        int b = fibonacci(number - 1);
        return a + b;
    }
    };
}

#include <stdio.h>
#include "collatz.h"

int collatz_conjecture(int input) 
{
    if (input % 2 == 0) {
        return input / 2;
    } else {
        return 3 * input + 1;
    }
}

int test_collatz_convergence(int input, int max_iter, int *steps)
{
    int cnt = 0;
    while (input != 1 && cnt < max_iter)
    {
        steps[cnt] = input;
        input = collatz_conjecture(input);
        cnt++;
    }
    if (input == 1)
    {
        return cnt;
    }
    return 0;
}
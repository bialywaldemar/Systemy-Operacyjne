#include "collatz.h"
#include <stdio.h>

int main()
{
    int x = 17;
    int max_it = 1500;
    int steps[max_it];
    int res = test_collatz_convergence(x, max_it, steps);

    printf("Collatz sequence for %d:\n", x);
    for (int i = 0; i < res; i++) 
    {
        printf("%d ", steps[i]);
    }
    printf("\nTotal steps: %d\n", res);
}
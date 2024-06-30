#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include "bibl1.h"




/*napisz biblioteke ladowana dynamicznie przez program zawierajaca funkcje:

1) zliczajaca sume n elementow tablicy tab:
int sumuj(int *tab, int n);

2) liczaca srednia n elementow tablicy tab
double srednia(int *tab, int n);

*/

int sumuj(int *tab, int n) {
    int suma = 0;
    for (int i = 0; i < n; i++) {
        suma += tab[i];
    }
    return suma;
}

double srednia(int *tab, int n) {
    if (n == 0) return 0;

    int sum = sumuj(tab, n);
    return (double)sum / n;
}

#include <stdio.h>
int add(int, int);
int subtract(int, int);
double func2(double, double);

int main() {
    int a = add(2, 100);
    int b = subtract(2, 1);
    double c = func2(2.0, 1.0);

    return a;
}
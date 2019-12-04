#include <math.h>
#define TRUE 1
#define FALSE 0

// Precondition
// (and (<= 4 x1 159/25) (<= 4 x2 159/25) (<= 4 x3 159/25) (<= 4 x4 159/25) (<= 4 x5 159/25) (<= 4 x6 159/25))

double ex0(double x1, double x2, double x3, double x4, double x5, double x6) {
	return (((((x2 * x5) + (x3 * x6)) - (x2 * x3)) - (x5 * x6)) + (x1 * (((((-x1 + x2) + x3) - x4) + x5) + x6)));
}


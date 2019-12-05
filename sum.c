#include <math.h>
#include <stdio.h>
#define TRUE 1
#define FALSE 0

// Preconditions
// (and (<= 1 x0 2) (<= 1 x1 2) (<= 1 x2 2))

double ex0(double x0, double x1, double x2) {
	double p0 = ((x0 + x1) - x2);
	double p1 = ((x1 + x2) - x0);
	double p2 = ((x2 + x0) - x1);
	return ((p0 + p1) + p2);
}

int main() {
	FILE *fp;
	fp = fopen("sum_posit.txt", "w+");
	double x0, x1, x2;
	for (x0 = 1.; x0 <= 2.; x0 += 0.1)	
		for (x1 = 1.; x1 <= 2.; x1 += 0.1)	
			for (x2 = 1.; x2 <= 2.; x2 += 0.1)
				fprintf(fp, "%.20g\n", ex0(x0, x1, x2));
	fclose(fp);
}
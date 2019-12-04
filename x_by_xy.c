#include <math.h>
#include <stdio.h>
#define TRUE 1
#define FALSE 0

// Preconditions
// (and (<= 1 x 4) (<= 1 y 4))

double ex0(double x, double y) {
	return (x / (x + y));
}

int main() {
	FILE *fp;
	fp = fopen("x_by_xy_posit.txt", "w+");
	double x;
	double y;
	for (x = 1.; x <= 4.; x += 0.1)
		for (y = 1.; y <= 4.; y += 0.1)
			fprintf(fp, "%.20g\n", ex0(x, y));
	fclose(fp);
}
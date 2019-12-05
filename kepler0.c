#include <math.h>
#include <stdio.h>
#define TRUE 1
#define FALSE 0

// Precondition
// (and (<= 4 x1 159/25) (<= 4 x2 159/25) (<= 4 x3 159/25) (<= 4 x4 159/25) (<= 4 x5 159/25) (<= 4 x6 159/25))

double ex0(double x1, double x2, double x3, double x4, double x5, double x6) {
	return (((((x2 * x5) + (x3 * x6)) - (x2 * x3)) - (x5 * x6)) + (x1 * (((((-x1 + x2) + x3) - x4) + x5) + x6)));
}

int main() {
	FILE *fp;
	fp = fopen("kepler0_posit.txt", "w+");
	double x1, x2, x3, x4, x5, x6;
	for (x1 = 4.; x1 <= 159./25.; x1 += 0.3)
		for (x2 = 4.; x2 <= 159./25.; x2 += 0.3)
			for (x3 = 4.; x3 <= 159./25.; x3 += 0.3)	
				for (x4 = 4.; x4 <= 159./25.; x4 += 0.3)
					for (x5 = 4.; x5 <= 159./25.; x5 += 0.3)
						for (x6 = 4.; x6 <= 159./25.; x6 += 0.3)
							fprintf(fp, "%.20g\n", ex0(x1, x2, x3, x4, x5, x6));
	fclose(fp);
}
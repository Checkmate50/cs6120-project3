#include <math.h>
#include <stdio.h>
#define TRUE 1
#define FALSE 0

// Precondition:
// (and (<= 4 x1 3969/625) (<= 4 x2 3969/625) (<= 4 x3 3969/625) (<= 4 x4 3969/625) (<= 4 x5 3969/625) (<= 4 x6 3969/625))

double ex0(double x1, double x2, double x3, double x4, double x5, double x6) {
	return ((((((-x2 * x3) - (x1 * x4)) + (x2 * x5)) + (x3 * x6)) - (x5 * x6)) + (x1 * (((((-x1 + x2) + x3) - x4) + x5) + x6)));
}

int main() {
	FILE *fp;
	fp = fopen("delta4_posit.txt", "w+");
	double x1, x2, x3, x4, x5, x6;
	for (x1 = 4.; x1 <= 3969./625.; x1 += 0.3)
		for (x2 = 4.; x2 <= 3969./625.; x2 += 0.3)
			for (x3 = 4.; x3 <= 3969./625.; x3 += 0.3)	
				for (x4 = 4.; x4 <= 3969./625.; x4 += 0.3)
					for (x5 = 4.; x5 <= 3969./625.; x5 += 0.3)
						for (x6 = 4.; x6 <= 3969./625.; x6 += 0.3)
							fprintf(fp, "%.20g\n", ex0(x1, x2, x3, x4, x5, x6));
	fclose(fp);
}
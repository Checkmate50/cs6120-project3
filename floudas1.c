#include <math.h>
#define TRUE 1
#define FALSE 0

// Precondition:
// (and (<= 0 x1 6) (<= 0 x2 6) (<= 1 x3 5) (<= 0 x4 6) (<= 0 x5 6) (<= 0 x6 10) 
// (>= (- (+ (* (- x3 3) (- x3 3)) x4) 4) 0) (>= (- (+ (* (- x5 3) (- x5 3)) x6) 4) 0) 
// (>= (+ (- 2 x1) (* 3 x2)) 0) (>= (- (+ 2 x1) x2) 0) (>= (- (- 6 x1) x2) 0) (>= (- (+ x1 x2) 2) 0))

double ex0(double x1, double x2, double x3, double x4, double x5, double x6) {
	return ((((((-25.0 * ((x1 - 2.0) * (x1 - 2.0))) - ((x2 - 2.0) * (x2 - 2.0))) - ((x3 - 1.0) * (x3 - 1.0))) - ((x4 - 4.0) * (x4 - 4.0))) - ((x5 - 1.0) * (x5 - 1.0))) - ((x6 - 4.0) * (x6 - 4.0)));
}


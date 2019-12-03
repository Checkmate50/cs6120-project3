#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "posit.h"
#include "posit_ops.h"
#include "posit_convert.h"

float accuracy(float answer, double correct_answer) {
  return 1 - abs((correct_answer - answer) / correct_answer);
}

float summation_benchmark(int limit) {
   float x = 0;
   float increment = 1;
   for(int i = 0; i < limit; i++) {
     x = x + increment;  
   }

   double correct_answer = limit * (limit + 1) / 2;
   return accuracy(x, correct_answer);
}

bool within_epsilon(double a, double b) {
  // See: https://en.wikipedia.org/wiki/Machine_epsilon
  double float_epsilon = pow(2, -24);
  return abs(a - b) < float_epsilon; 
}

/*
void detect_posit_pass(double result, double float_answer, double posit_answer) {
  if(within_epsilon(result, float_answer)) {
    printf("Absence of LLVM Pass Detected: floats were used\n");
  }
  else {
    if(within_epsilon(result, posit_answer)) {
      printf("LLVM Pass Detected: posits were used\n");
    }
    else {
      printf("Unable to confirm/deny posit pass: result doesn't match expected result for either floats or posits.\n");
    }
  }
}
*/

typedef struct small_res {double *add; double *sub; double *mul; double *div;} small_res;

small_res small_pointer_benchmark(int n, double small) {
  small_res res = {malloc(sizeof(double)), malloc(sizeof(double)), malloc(sizeof(double)), malloc(sizeof(double))};
  *res.add = 0.0l;
  *res.sub = 0.0l;
  *res.mul = 1.0l;
  *res.div = 1.0l;

  for(int i = 0; i < n; i++) {
    *res.add = *res.add + small;
    *res.sub = *res.sub - small;
    *res.mul = *res.mul * small;
    *res.div = *res.div / small;
  }

  return res;
}

void print_assert_equal(double result, double expected) {
  if(result == expected) {
    printf("SUCCESS\n");
  }
  else {
    printf("FAILURE\n");
  }
}

void pointer_ops_sanity_check(int n, double small, small_res expected) {

  small_res res = small_pointer_benchmark(n, small);

  printf("pointer sanity check for addition... ");
  print_assert_equal(*res.add, *expected.add);

  printf("pointer sanity check for subtraction... ");
  print_assert_equal(*res.sub, *expected.sub);

  printf("pointer sanity check for multiplication... ");
  print_assert_equal(*res.mul, *expected.mul);

  printf("pointer sanity check for division... ");
  print_assert_equal(*res.div, *expected.div);
}

int main() {
  /*
  int n = 10000;
  printf("accuracy of summation up to %d = %f\n", n, summation_benchmark(n));
  */

  int n = 6; 
  double small = 0.132718f;
  small_res res = small_pointer_benchmark(n, small);

  printf("results of small_pointer_benchmark(%d, %f)\n:", n, small);
  printf("add = %f\nsub = %f\nmul = %f\ndiv = %f\n", *res.add, *res.sub, *res.mul, *res.div);
}


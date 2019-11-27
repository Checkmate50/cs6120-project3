#include <stdlib.h>
#include <stdio.h>

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

int main() {
  int n = 10000;
  printf("accuracy of summation up to %d = %f", n, summation_benchmark(n));
}

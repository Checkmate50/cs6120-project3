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

void pointer_ops_sanity_check(int n,
                          double float_add_answer, double float_sub_answer, 
                          double float_mul_answer, double float_div_answer,
                          double posit_add_answer, double posit_sub_answer, 
                          double posit_mul_answer, double posit_div_answer) {
  double small = 0.01f;

  float *add_ptr_result = &(float){small},
        *sub_ptr_result = &(float){small},
        *mul_ptr_result = &(float){small}, 
        *div_ptr_result = &(float){small};

  for(int i = 0; i < n; i++) {
    *add_ptr_result = *add_ptr_result + small;
    *sub_ptr_result = *sub_ptr_result - small;
    *mul_ptr_result = *mul_ptr_result * small;
    *div_ptr_result = *div_ptr_result / small;
  }

  printf("checking if repeated addition pointer op correctly executed over either floats or posits...\n");
  detect_posit_pass(*add_ptr_result, float_add_answer, posit_add_answer);

  printf("checking if repeated subtraction pointer op correctly executed over either floats or posits...\n");
  detect_posit_pass(*sub_ptr_result, float_sub_answer, posit_sub_answer);

  printf("checking if repeated multiplication pointer op correctly executed over either floats or posits...\n");
  detect_posit_pass(*mul_ptr_result, float_mul_answer, posit_mul_answer);

  printf("checking if repeated division pointer op correctly executed over either floats or posits...\n");
  detect_posit_pass(*div_ptr_result, float_div_answer, posit_div_answer);

}

int main() {
  int n = 10000;
  printf("accuracy of summation up to %d = %f", n, summation_benchmark(n));
}

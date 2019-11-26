#include <stdio.h>

#include "posit_convert.h"
#include "posit_ops.h"

void print_posit32_binary(posit32_t p) {
  for(int i = 31; i >= 0; i--) {
    printf("%d", !!(p.v& (1 << i)));
  }
  printf("\n");
}

int main() {
  float float_val = 1;
  posit32_t p = convertFloatToP32(float_val);
  printf("\nposit representation of %f: ", float_val);
  print_posit32_binary(p);
  
  float x = -1, y = 100;
  printf("Posit add test:\n");
  float z = convertP32ToFloat(p32_add(convertFloatToP32(x), convertFloatToP32(y)));
  printf("%f + %f = %f\n", x, y, z);
}


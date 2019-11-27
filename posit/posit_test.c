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
  
  float x, y, z;
  x = -1, y = 100;
  printf("Posit add test:\n");
  z = convertP32ToFloat(p32_add(convertFloatToP32(x), convertFloatToP32(y)));
  printf("%f + %f = %f\n", x, y, z);

  x = 100, y = -100;
  printf("Posit sub test:\n");
  z = convertP32ToFloat(p32_sub(convertFloatToP32(x), convertFloatToP32(y)));
  printf("%f - %f = %f\n", x, y, z);

  x = 100, y = -50;
  printf("Posit mul test:\n");
  z = convertP32ToFloat(p32_mul(convertFloatToP32(x), convertFloatToP32(y)));
  printf("%f * %f = %f\n", x, y, z);

  x = 100, y = -4;
  printf("Posit div test:\n");
  z = convertP32ToFloat(p32_div(convertFloatToP32(x), convertFloatToP32(y)));
  printf("%f / %f = %f\n", x, y, z);
}

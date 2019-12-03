#include <stdio.h>
#include "posit/posit_ops.h"
#include "posit/posit_convert.h"
int main() {
  float num = 124.15151;
  float num2 = 123.212334;
  posit32_t l = convertFloatToP32(num);
  posit32_t r = convertFloatToP32(num2);
  posit32_t res = p32_add(p32_add(p32_add(p32_add(l, l), l), l), r);
  printf("%.12g\n", convertP32ToFloat(res));
  printf("%.12g\n", num + num + num + num + num2);
  return 0;
}

#include <stdio.h>
int temp(int x, int y) {
  return x - y;
}

int main() {
  int num = 1;
  int num2 = 2;
  printf("%i\n", temp(num, num2));
  printf("%i\n", num + num2);
  return 0;
}

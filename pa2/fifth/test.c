#include <stdlib.h> 
#include <stdio.h>

int main() {
 unsigned a = 72;
 unsigned b = 128;
 int r1 = (a & a-1) ? 0: 1;
 int r2 = (b & b-1) ? 0: 1;
 printf("%d,%d", r1, r2);
}
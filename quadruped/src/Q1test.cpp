#include <stdio.h>
#include <stdlib.h>

#include "include/quadruped.h"

int main(int argc, char** argv) {
  double a = 15.0;
  if (argc > 1) {
    a = atof(argv[1]);
  }
  Q1::Quadruped Q();
  printf("%.2f => %.2f\n",a, Q1::NormalizeAngle(a));
  
}

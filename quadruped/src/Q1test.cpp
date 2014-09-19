#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/quadruped.h"


int main(int argc, char** argv) {
  Q1::SpringGG sgg;
  sgg.SetFootPosition(0, 150, 150);
  sgg.SetFootPosition(1, -150, 150);
  sgg.SetFootPosition(2, -150, -150);
  sgg.SetFootPosition(3, 150, -150);
  sgg.SetCoMPosition(0, 0);
  sgg.ZeroForces();
  sgg.SetFootPosition(1, -100, 150);
  sgg.CalculateForces();
  sgg.CalculateForces();
  sgg.PrintForces();
  return 0;
}

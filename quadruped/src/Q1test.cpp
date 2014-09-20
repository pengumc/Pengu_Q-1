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
  sgg.SetCoMPosition(10, 0);
  sgg.CalculateForces();
  sgg.PrintForces();
  printf("com inside 0 = %i\n", sgg.CoMInside(0, 0.0));
  printf("com inside 1 = %i\n", sgg.CoMInside(1, 0.0));
  printf("com inside 2 = %i\n", sgg.CoMInside(2, 0.0));
  printf("com inside 3 = %i\n", sgg.CoMInside(3, 0.0));
  printf("leg = %i\n", sgg.GetLegWithHighestForce(0.0));
  return 0;
}

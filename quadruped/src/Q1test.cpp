#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/quadruped.h"


int main(int argc, char** argv) {
  Q1::SpringGG sgg;
  double D = 110;
  sgg.SetFootPosition(0, D, D+20);
  sgg.SetFootPosition(1, -D, D+20);
  sgg.SetFootPosition(2, -D, -D-20);
  sgg.SetFootPosition(3, D, -D-20);
  sgg.SetCoMPosition(0, 0);
  sgg.ZeroForces();
  sgg.SetCoMPosition(0, 20);
  // sgg.SetFootPosition(2, -D, 30);
  sgg.CalculateForces();
  sgg.PrintForces();
  printf("com inside 0 = %i\n", sgg.CoMInside(0, 0.0));
  printf("com inside 1 = %i\n", sgg.CoMInside(1, 0.0));
  printf("com inside 2 = %i\n", sgg.CoMInside(2, 0.0));
  printf("com inside 3 = %i\n", sgg.CoMInside(3, 0.0));
  printf("leg = %i\n", sgg.GetLegWithHighestForce(M_PI/2.0));
  double v[3];
  sgg.GetDeltaVector(2, M_PI/2.0, 130, v);
  printf("v = %.0f, %.0f, %.0f\n", v[0], v[1], v[2]);
  return 0;
}

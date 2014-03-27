#include <stdio.h>
#include <stdlib.h>

#include "include/quadruped.h"

int main(int argc, char** argv) {
  double a = 15.0;
  if (argc > 1) {
    a = atof(argv[1]);
  }
  Q1::Quadruped* aap = new Q1::Quadruped();
  
  for (int i = 0; i < Q1::HMatrix::kMagic16; ++i) {
    printf("%.2f\n", ar[i]);
  }
  
}

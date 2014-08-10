#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/quadruped.h"


int main(int argc, char** argv) {
  //Q1::Quadruped* aap = new Q1::Quadruped();
  //ROGG::GaitGenerator* aap = new ROGG::GaitGenerator();
  ROGG::GaitGenerator aap;
  printf("%d\n", &aap);

  return 0;
}

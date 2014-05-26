#include <stdio.h>
#include <stdlib.h>

#include "include/quadruped.h"

int main(int argc, char** argv) {
  Q1::Quadruped* aap = new Q1::Quadruped();
  delete aap;

  return 0;
}

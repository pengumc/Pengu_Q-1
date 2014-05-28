#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/quadruped.h"


int main(int argc, char** argv) {
  Q1::Quadruped* aap = new Q1::Quadruped();
  printf("connect: %d\n", aap->ConnectDevice(0x16c0, 0x05df));

  const double* angles = aap->GetDeviceAngles();
  if (angles) {
    for (int i = 0; i < 12; ++i) {
      printf("%.2f\n", angles[i]);
    }
  }
  
  delete aap;



  return 0;
}

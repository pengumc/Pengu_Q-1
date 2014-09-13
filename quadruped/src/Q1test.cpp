#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/quadruped.h"


int main(int argc, char** argv) {
  Q1::UsbCom dev;
  
  double pulsewidths[12];
  for (int i = 0; i<12; ++i) {
    pulsewidths[i] = atof(argv[1]);
  }
  
  int c = dev.Connect(0x16c0, 0x05df);
  printf("connect: %i\n", c);
  
  c = dev.WriteServoPulsewidths(pulsewidths);
  printf("write: %i\n", c);
  
  
  return 0;
}

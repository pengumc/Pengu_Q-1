#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/quadruped.h"


int main(int argc, char** argv) {
  Q1::UsbCom dev;
  int c = dev.Connect(0x16c0, 0x05df);
  printf("connect: %i", c);
  
  return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/quadruped.h"


int main(int argc, char** argv) {
  Q1::Quadruped q;
  int r = q.ConnectDevice(0x16c0, 0x05df);
  printf("conntect: %i\n", r);
  
  r = q.SyncFromDevice();
  printf("sync from dev: %i\n", r);
  return 0;
}

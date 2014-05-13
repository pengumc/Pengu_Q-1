#include <stdio.h>
#include <stdlib.h>

#include "include/quadruped.h"

int main(int argc, char** argv) {
  //~ double a = 15.0;
  //~ if (argc > 1) {
    //~ a = atof(argv[1]);
  //~ }
  Q1::Quadruped* aap = new Q1::Quadruped();
  delete aap;
  
  //create 3 pivots
  Q1::Pivot p1 = Q1::Pivot(NULL);
  p1.SetPosition(0,0,0);
  p1.set_abs_max_angle(3.14);
  Q1::Pivot p2 = Q1::Pivot(p1.H_framep());
  p2.set_abs_max_angle(3.14);
  p2.SetPosition(1,0,0);
  p2.set_offset_angle(-1.57);
  Q1::Pivot foot = Q1::Pivot(p2.H_framep());
  foot.SetPosition(1,0,0);
  //create 1 IK
  Q1::InvKinematic ik = Q1::InvKinematic(3);
  ik.SetPivot(0, &p1);
  ik.SetPivot(1, &p2);
  ik.SetPivot(2, &foot);
  // do stuff
  printf("angle p1: %.2f\n", p1.angle());
  const double* rel = foot.GetRelativeHMatrixArray();
  printf("pos foot:   %.2f, %.2f, %.2f\n", 
    rel[Q1::HMatrix::kX],
    rel[Q1::HMatrix::kY],
    rel[Q1::HMatrix::kZ]);
  
  double target[3] = {
      1.9,
      -0.0,
      0.0};
  ik.SetTargetPos(target[0], target[1], target[2]);
  printf("target pos: %.2f, %.2f, %.2f\n", target[0], target[1], target[2]); 
  printf("---\n");
  for (int i = 0; i < 3; ++i) {
    printf("step result: %f\n", ik.Step());
    printf("pivot angles:\n%f\n%f\n\n",p1.angle(), p2.angle());
    if (ik.flag() != 0) {
      printf("==> flag = %d\n", ik.flag());
      break;
    }
  }
}

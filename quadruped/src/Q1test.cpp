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
  
  //create 2 pivots
  Q1::Pivot p1 = Q1::Pivot(NULL);
  p1.SetPosition(0,0,0);
  Q1::Pivot foot = Q1::Pivot(p1.H_framep());
  foot.SetPosition(10,0,0);
  //create 1 IK
  Q1::InvKinematic ik = Q1::InvKinematic(2);
  ik.SetPivot(0, &p1);
  ik.SetPivot(1, &foot);
  // do stuff
  printf("angle p1: %.2f\n", p1.angle());
  printf("pos foot: %.2f, %.2f, %.2f\n", 
    foot.H_framep()->GetX(),
    foot.H_framep()->GetY(),
    foot.H_framep()->GetZ());
  
  double target[3] = {
      std::cos(0.1)*10.0,
      std::sin(0.1)*10.0,
      0.0};
  ik.SetTargetPos(target[0], target[1], target[2]);
  printf("target pos: %.2f, %.2f, %.2f\n", target[0], target[1], target[2]); 
  printf("---\n");
  for (int i = 0; i < 3; ++i) {
    printf("step result: %f\n", ik.Step());
    printf("pivot angles:\n%f\n\n",p1.angle());
  }
}

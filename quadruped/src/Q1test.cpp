#include <stdio.h>
#include <stdlib.h>

#include "include/quadruped.h"

int main(int argc, char** argv) {
  Q1::Quadruped* aap = new Q1::Quadruped();
  delete aap;

  // create 3 pivots
  Q1::Pivot p1 = Q1::Pivot(NULL);
  p1.SetPosition(0, 0, 0);
  p1.set_abs_max_angle(M_PI);
  Q1::Pivot p2 = Q1::Pivot(p1.H_framep());
  p2.set_abs_max_angle(M_PI);
  p2.SetPosition(1, 0, 0);
  p2.set_offset_angle(-1.57);
  Q1::Pivot foot = Q1::Pivot(p2.H_framep());
  foot.SetPosition(1, 0, 0);
  // create 1 IK
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

  FILE* f = fopen("ikdata.csv", "w");
  fprintf(f, "targetx, targety, targetz, iterations, angle1, angle2\n");
  int r;
  for (double x = -2.0; x < 2.1; x += 0.1) {
    for (double y = -2.0; y < 2.1; y += 0.1) {
      ik.SetTargetPos(x, y, 0.0);
      r = ik.Iterate(300);
      if (ik.flag() != 0) r = -ik.flag();
      const double* H = foot.GetRelativeHMatrixArray();
      if (1) {
        fprintf(f, "%f, %f, %f, %d, %f, %f\n", 
                x, y, 0.0, r, p1.angle(), p2.angle());
      }
    }
  }
  fclose(f);
  //~ ik.SetTargetPos(target[0], target[1], target[2]);
  //~ printf("target pos: %.2f, %.2f, %.2f\n", target[0], target[1], target[2]);
  //~ printf("---\n");
  //~ printf(" steps taken: %d\n", ik.Iterate(100));
  //~ const double* rel2= foot.GetRelativeHMatrixArray();
  //~ printf(" pos foot:  %.2f, %.2f, %.2f\n",
    //~ rel2[Q1::HMatrix::kX],
    //~ rel2[Q1::HMatrix::kY],
    //~ rel2[Q1::HMatrix::kZ]);
  //~ printf(" pivot angles:\n %f\n %f\n", p1.angle(), p2.angle());
  //~ printf(" flag = %d\n", ik.flag());
  //~ 
  
}

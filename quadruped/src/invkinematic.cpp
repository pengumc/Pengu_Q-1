/**
 * @file invkinematic.cpp
 * Inverse kinematic engine
 */

#include "include/invkinematic.h"

namespace Q1 {

// ------------------------------------------------------------------Constructor
/** @brief constructor
 * 
 * @param pivot_count number of pivots in the chain, the last one is considered
 * the endpoint.
 */
InvKinematic::InvKinematic(int pivot_count) {
  pivot_count_ = pivot_count;
  jacobianT_ = static_cast<double*>(calloc(pivot_count_ * 3,
                                     sizeof(double)));
  pivots_ = static_cast<Pivot**>(calloc(pivot_count_, sizeof(Pivot*)));
  flag_ = NOTHING_WRONG;
  target_[0] = 0.0;
  target_[1] = 0.0;
  target_[2] = 0.0;
}

// -------------------------------------------------------------------Destructor
/** @brief destructor*/
InvKinematic::~InvKinematic() {
  free(jacobianT_);
  free(pivots_);
}

// ---------------------------------------------------------------------SetPivot
/** @brief provide a pointer to a pivot in the chain*/
void InvKinematic::SetPivot(int pivot_index, Pivot* pivot) {
  if (pivot_index < pivot_count_) {
    pivots_[pivot_index] = pivot;
  }
}

// ------------------------------------------------------------ConstructJacobian
/** @brief updates the stored jacobian for the current values in the pivots*/
void InvKinematic::ConstructJacobian() {
  // ds_i / dtheta_j = v_j x (s_i - p_j)
  //
  //         (u2*v3 - u3*v2)
  // u x v = (u3*v1 - u1*v3)
  //         (u1*v2 - u2*v1)
  const int end = pivot_count_ - 1;  // don't include the end effector
  double unity_z[3] = {0.0, 0.0, 1.0};
  double v_j[3];
  for (int j = 0; j < end; ++j) {
    HMatrix diff = pivots_[end]->GetSpecificH(pivots_[j]->H_framep());
    if (diff.parent() == NULL) {
      // pivot is not in chain to end-effector. error in setup
      flag_ = NOT_ALL_PIVOT_IN_CHAIN;
      return;
    }
    pivots_[j]->H_framep()->RotateVector(unity_z, v_j);
    const double v_diff[3] = {diff.GetX(), diff.GetY(), diff.GetZ()};
    CrossProduct3(v_j, v_diff, &jacobianT_[j*3]);
  }
}

// --------------------------------------------------------------------SetTarget
/** @brief set the target end position
 * 
 * target is relative to origin
 */
void InvKinematic::SetTargetPos(double x, double y, double z) {
  target_[0] = x;
  target_[1] = y;
  target_[2] = z;
}

// -------------------------------------------------------------------------Step
/** @brief do a single iteration*/
void InvKinematic::Step() {
  ConstructJacobian();
  // jacobianT_ stored as:
  // x y z
  // x y z
  // x y z
  // x y z
  
  // normal jacobian should be
  // x x x x
  // y y y y 
  // z z z z
  
  // calculate J * transpose(J)
  const int end = pivot_count_ - 1;
  double JJT[end*3];  // k rows x 3 columns
  int r = 0;
  // m = col in transposed jacobian
  // n = row in transposed jacobian
  for (m = 0; m < 3; ++m) {
    for (n = 0; n < end; ++n) {
      JJT[m, n] = 0;
      for (trow = 0; trow < end; ++trow) {
        // JJT[m, n] += J[m,trow] * JT[trow,n];
        JJT[3*m + n] += jacobianT_[3*trow + m] * jacobianT_[3*trow + n];
      }
    }
  }
  
  // calculate e = columnvector(e_x, e_y, e_z)
  double e[3] = {
    target_[0] - pivots_[pivot_count_]->GetX(),
    target_[1] - pivots_[pivot_count_]->GetY(),
    target_[2] - pivots_[pivot_count_]->GetZ(),
  };
  
  // calc JT * e
  for row in jt
    jte[row] = jt[row, col1] * ex + jt[row, col2] * ey + jt[row, col3] * ez
  
  double JTe[end];
    for (int i; i < end; ++i) {
      JTe[i] = jacobianT_[3*i] * e[0] + jacobianT_[3*i + 1] * e[1] +
               jacobianT_[3*i + 2] * e[2];
    }
  }
  
  // calc JJTe
  double JJTe[3]
  for (int m = 0; m < 3; ++m) {
    JJTe[m] = 0;
    for (int n = 0; n < end; ++n) {
      JJTE[m] += jacobianT_[3*n + m] * JTe[n];
    }
  }
  
  // calc alpha = (e * JJTe) / (JJTe * JJTe)
  double alpha = (e[0]*JJTe[0] + e[1] * JJTe[1] + e[2] * JJTe[2]) /
                  (JJTe[0] * JJTe[0] + JJTe[1] * JJTe[1] + JJTe[2] * JJTe[2]);
  // change angles by alpha * JTe
  for (int i = 0; i < end; ++i) {
    pivots_[i]->ChangeAngle(alpha * JTe[i]);
  }
}

}  // namespace Q1

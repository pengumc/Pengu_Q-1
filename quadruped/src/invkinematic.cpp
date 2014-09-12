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
 * the endpoint. pivot_count minimum: 2
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
  max_allowed_error_ = 0.005;
}

// -------------------------------------------------------------------Destructor
/** @brief destructor*/
InvKinematic::~InvKinematic() {
  free(jacobianT_);
  free(pivots_);
}

// ----------------------------------------------------------------flag accessor
/** @brief returns the current flag*/
InvKinematic::IKFlags InvKinematic::flag() {
  return flag_;
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
  HMatrix H_0_s = HMatrix(
    pivots_[pivot_count_ - 1]->GetRelativeHMatrixArray());
  HMatrix H_0_pj;
  //  printf(" transposed jacobian:\n");
  for (int j = 0; j < end; ++j) {
    H_0_pj = HMatrix(pivots_[j]->GetRelativeHMatrixArray());
    H_0_pj.RotateVector(unity_z, v_j);
    const double v_diff[3] = {
      H_0_s.GetX() - H_0_pj.GetX(),
      H_0_s.GetY() - H_0_pj.GetY(),
      H_0_s.GetZ() - H_0_pj.GetZ()};
    CrossProduct3(v_j, v_diff, &jacobianT_[j*3]);
    //  printf(" %.4f, %.4f, %.4f\n", jacobianT_[j*3], jacobianT_[j*3+1],
      //  jacobianT_[j*3+2]);
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

// -----------------------------------------------------------SetMaxAllowedError
/** @brief set the maximum allowable error */
void InvKinematic::SetMaxAllowedError(double max_allowed_error) {
  max_allowed_error_ = std::abs(max_allowed_error);
}

// -------------------------------------------------------------------------Step
/** @brief do a single iteration, returns the new error distance*/
double InvKinematic::Step() {
  ConstructJacobian();
  const int end = pivot_count_ - 1;  // index of last pivot in chain
  HMatrix H_0_s = HMatrix(pivots_[pivot_count_ - 1]->GetRelativeHMatrixArray());
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

  //  double JJT[end*3];  // k rows x 3 columns
  //  int r = 0;
  //  for (m = 0; m < 3; ++m) {
    //  for (n = 0; n < end; ++n) {
      //  JJT[m, n] = 0;
      //  for (trow = 0; trow < end; ++trow) {
        //  JJT[3*m + n] += jacobianT_[3*trow + m] * jacobianT_[3*trow + n];
      //  }
    //  }
  //  }

  // calculate e = columnvector(e_x, e_y, e_z)
  // printf("ik H_0_s: %f, %f, %f\n", H_0_s.GetX(), H_0_s.GetY(), H_0_s.GetZ());
  double e[3] = {
    target_[0] - H_0_s.GetX(),
    target_[1] - H_0_s.GetY(),
    target_[2] - H_0_s.GetZ(),
  };
  //  printf(" error\n %f\n %f\n %f\n", e[0], e[1], e[2]);

  // calc JT * e
  double JTe[end];  // column vector
  //  printf(" JTe:\n");
  for (int i = 0; i < end; ++i) {
    JTe[i] = jacobianT_[3*i] * e[0] + jacobianT_[3*i + 1] * e[1] +
             jacobianT_[3*i + 2] * e[2];
    //  printf(" %f\n", JTe[i]);
  }


  // calc J*JT*e
  double JJTe[3];
  //  printf(" JJTe:\n");
  for (int m = 0; m < 3; ++m) {
    JJTe[m] = 0.0;
    for (int n = 0; n < end; ++n) {
      JJTe[m] += jacobianT_[3*n + m] * JTe[n];
    }
    //  printf(" %f\n", JJTe[m]);
  }

  // alpha = (e * JJTe) / (JJTe * JJTe)
  double divisor = (JJTe[0] * JJTe[0] + JJTe[1] * JJTe[1] + JJTe[2] * JJTe[2]);
  double alpha;
  if (divisor == 0.0) {
    alpha = 0.0;
    //  printf(" alpha = 0.0 (divisor was 0)\n");
  } else {
    alpha = (e[0]*JJTe[0] + e[1] * JJTe[1] + e[2] * JJTe[2]) / divisor;
    //  printf(" alpha: %f\n", alpha);
  }

  // change angles by alpha * JTe
  for (int i = 0; i < end; ++i) {
    bool success = pivots_[i]->ChangeAngle(alpha * JTe[i]);
    if (!success) {
      flag_ = NEW_ANGLE_OUT_OF_REACH;
    }
  }

  // recalc e
  H_0_s = HMatrix(pivots_[pivot_count_ - 1]->GetRelativeHMatrixArray());
  e[0] = target_[0] - H_0_s.GetX();
  e[1] = target_[1] - H_0_s.GetY();
  e[2] = target_[2] - H_0_s.GetZ();
  return std::sqrt(std::pow(e[0], 2) + std::pow(e[1], 2) + std::pow(e[2], 2));
}

// ----------------------------------------------------------------------Iterate
/** @brief perform steps until error <= max_allowed_error.
 *
 * The validity of the result can be checked with \ref flag()
 * @param max_steps the maximun amount of step to do before giving up
 * @return the number of steps taken
 */
int InvKinematic::Iterate(int max_steps) {
  double error;
  flag_ = NOTHING_WRONG;
  const int end = pivot_count_ -1;
  // store original angles and limits of the pivots
  double original_angles[end];
  double original_max[end];
  for (int i = 0; i < end; ++i) {
    original_angles[i] = pivots_[i]->angle();
    original_max[i] = pivots_[i]->abs_max_angle();
    pivots_[i]->set_abs_max_angle(M_PI);
    // setting abs_max_angle clears the current angle, so let's restore it:
    pivots_[i]->set_angle(original_angles[i]);
  }
  for (int i = 0; i < max_steps; ++i) {
    error = Step();
    if (flag_ != NOTHING_WRONG) {
      // restore original values
      for (int j = 0; j < end; ++j) {
        pivots_[j]->set_abs_max_angle(original_max[j]);
        pivots_[j]->set_angle(original_angles[j]);
      }
      return i;
    }
    if (error <= max_allowed_error_) {
      // success. now let's see if those angles actually available
      int success = 0;
      double angle;
      for (int j = 0; j < end; ++j) {
        angle = pivots_[j]->angle();
        pivots_[j]->set_abs_max_angle(original_max[j]);
        // this has cleared the angle, so let's try setting it again:
        if (pivots_[j]->set_angle(angle)) {
          ++success;
        }
      }
      if (success < end) {
        for (int j = 0; j < end; ++j) {
          pivots_[j]->set_angle(original_angles[j]);
          flag_ = NEW_ANGLE_OUT_OF_REACH;
        }
      }
      return i;
    }
  }
  flag_ = EXCEEDED_MAX_STEPS;
  // restore original values
  for (int j = 0; j < end; ++j) {
    pivots_[j]->set_abs_max_angle(original_max[j]);
    pivots_[j]->set_angle(original_angles[j]);
  }
  return(max_steps);
}
}  // namespace Q1

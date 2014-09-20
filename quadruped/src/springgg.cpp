/**
 * @file springgg.cpp
 * spring based gait generation
 */

#include "include/springgg.h"

namespace Q1 {

// ------------------------------------------------------------------Constructor
/** @brief constructor*/
SpringGG::SpringGG() {
  // changes here should also be reflected in SetBasePositions
  springs[0].Connect(&feet_[0], &feet_[1]);
  springs[1].Connect(&feet_[1], &feet_[2]);
  springs[2].Connect(&feet_[2], &feet_[3]);
  springs[3].Connect(&feet_[3], &feet_[0]);
  springs[4].Connect(&feet_[0], &feet_[2]);
  springs[5].Connect(&feet_[1], &feet_[3]);
  springs[6].Connect(&feet_[0], &com_);
  springs[7].Connect(&feet_[1], &com_);
  springs[8].Connect(&feet_[2], &com_);
  springs[9].Connect(&feet_[3], &com_);
}

// -------------------------------------------------------------------ZeroForces
/** @brief set all the current forces to zero by setting the equilibrium
 * distances of all springs to their current distances*/
void SpringGG::ZeroForces() {
  for (int i = 0; i < kSpringCount; ++i) {
    springs[i].ZeroForce();
  }
}

// --------------------------------------------------------------SetFootPosition
/** @brief set the position (xy) of a foot on the ground plane
 *
 * @param index foot index 0..3
 */
void SpringGG::SetFootPosition(int index, double x, double y) {
  feet_[index].SetPosition(x, y);
}

// ---------------------------------------------------------------SetCoMPosition
/** @brief set the position (xy) of the CoM on the ground plane*/
void SpringGG::SetCoMPosition(double x, double y) {
  com_.SetPosition(x, y);
}

// --------------------------------------------------------------CalculateForces
/** @brief evalute the springs and set the net forces on the points*/
void SpringGG::CalculateForces() {
  double dx, dy, F, Fx, Fy;
  feet_[0].ClearForce();
  feet_[1].ClearForce();
  feet_[2].ClearForce();
  feet_[3].ClearForce();
  com_.ClearForce();
  for (int i = 0; i < kSpringCount; ++i) {
    dx = springs[i].B()->x() - springs[i].A()->x();
    dy = springs[i].B()->y() - springs[i].A()->y();
    F = springs[i].CalculateForce();
    Fx = std::cos(std::atan2(dy, dx)) * F;
    Fy = std::sin(std::atan2(dy, dx)) * F;
    springs[i].A()->AddForce(-Fx, -Fy);
    springs[i].B()->AddForce(Fx, Fy);
  }
}

// --------------------------------------------------------------------IsInside
/** @brief check if point at x,y is inside the support pattern that allows
 * lifting of leg index
 */
bool SpringGG::IsInside(int index, double x, double y) {
  double v0[3], v1[3], v2[3], v3[3], v_p[3], v_com[3];
  SpringPoint* p1 = &feet_[(index+1)%4];
  SpringPoint* p2 = &feet_[(index+2)%4];
  SpringPoint* p3 = &feet_[(index+3)%4];

  v_p[0] = p2->x() - p1->x();
  v_p[1] = p2->y() - p1->y();
  v_com[0] = x - p1->x();
  v_com[1] = y - p1->y();
  CrossProduct3(v_p, v_com, v0);

  v_p[0] = p3->x() - p1->x();
  v_p[1] = p3->y() - p1->y();
  v_com[0] = x - p1->x();
  v_com[1] = y - p1->y();
  CrossProduct3(v_p, v_com, v1);

  v_p[0] = p1->x() - p2->x();
  v_p[1] = p1->y() - p2->y();
  v_com[0] = x - p2->x();
  v_com[1] = y - p2->y();
  CrossProduct3(v_p, v_com, v2);

  v_p[0] = p3->x() - p2->x();
  v_p[1] = p3->y() - p2->y();
  v_com[0] = x - p2->x();
  v_com[1] = y - p2->y();
  CrossProduct3(v_p, v_com, v3);

  return ((v0[2]*v1[2] < 0) && (v2[2]*v3[2] < 0));
}

// --------------------------------------------------------------------CoMInside
/** @brief check if CoM is inside support pattern (see \ref IsInside) with
 * some margin
 */
bool SpringGG::CoMInside(int index, double margin) {
  if (IsInside(index, com_.x() - margin, com_.y()) &&
      IsInside(index, com_.x() + margin, com_.y()) &&
      IsInside(index, com_.x(), com_.y() - margin) &&
      IsInside(index, com_.x(), com_.y() + margin)) {
    return true;
  } else {
    return false;
  }
}

// ------------------------------------------------------------------PrintForces
/** @brief dump net forces on feet and com to stdout */
void SpringGG::PrintForces() {
  printf("foot 0: (%.0f, %.0f)\n Fx = %f\n Fy = %f\n\n",
    feet_[0].x(), feet_[0].y(), feet_[0].Fx(), feet_[0].Fy());
  printf("foot 1: (%.0f, %.0f)\n Fx = %f\n Fy = %f\n\n",
    feet_[1].x(), feet_[1].y(), feet_[1].Fx(), feet_[1].Fy());
  printf("foot 2: (%.0f, %.0f)\n Fx = %f\n Fy = %f\n\n",
    feet_[2].x(), feet_[2].y(), feet_[2].Fx(), feet_[2].Fy());
  printf("foot 3: (%.0f, %.0f)\n Fx = %f\n Fy = %f\n\n",
    feet_[3].x(), feet_[3].y(), feet_[3].Fx(), feet_[3].Fy());
  printf("com: (%.0f, %.0f)\n Fx = %f\n Fy = %f\n\n",
    com_.x(), com_.y(), com_.Fx(), com_.Fy());
}

// -------------------------------------------------------GetLegWithHighestForce
/** @brief return the leg-index (0-based) for the foot that has the highest
 * positive net force in the given direction
 *
 * @return -1 if no foot has a positive force.
 */
int SpringGG::GetLegWithHighestForce(double direction_angle) {
  double F;
  double F_high = 0;
  int leg = -1;
  for (int i = 0; i < 4; ++i) {
    F = feet_[i].Fx() * std::cos(direction_angle) +
            feet_[i].Fy() * std::sin(direction_angle);
    if (F > F_high) {
      F_high = F;
      leg = i;
    }
  }
  return leg;
}



// ------------------------------------------------------------------constructor
/** @brief constructor */
SpringPoint::SpringPoint() {
  Fx_ = 0.0;
  Fy_ = 0.0;
  x_ = 0.0;
  y_ = 0.0;
}

// -------------------------------------------------------------------ClearForce
/** @brief set \ref Fx_ and \ref Fy_ to 0*/
void SpringPoint::ClearForce() {
  Fx_ = 0;
  Fy_ = 0;
}

// ---------------------------------------------------------------------AddForce
/** @brief add an x and y force to the net force on the point*/
void SpringPoint::AddForce(double x, double y) {
  Fx_ += x;
  Fy_ += y;
}

// ------------------------------------------------------------------SetPosition
/** @brief set xy position of point*/
void SpringPoint::SetPosition(double x, double y) {
  x_ = x;
  y_ = y;
}

// -------------------------------------------------------------------x accessor
/** @brief \ref x_ accessor*/
double SpringPoint::x() {
  return x_;
}

// -------------------------------------------------------------------y accessor
/** @brief \ref y_ accessor*/
double SpringPoint::y() {
  return y_;
}

// ------------------------------------------------------------------Fx accessor
/** @brief \ref Fx_ accessor*/
double SpringPoint::Fx() {
  return Fx_;
}

// ------------------------------------------------------------------Fy accessor
/** @brief \ref Fy_ accessor*/
double SpringPoint::Fy() {
  return Fy_;
}



// ------------------------------------------------------------------constructor
/** @brief constructor */
Spring::Spring() {
  A_ = NULL;
  B_ = NULL;
  K_ = 1.0;
  base_distance_ = 0.0;
}

// ----------------------------------------------------------------------Connect
/** @brief connect two SpringPoint to the spring*/
void Spring::Connect(SpringPoint* A, SpringPoint* B) {
  A_ = A;
  B_ = B;
}

// --------------------------------------------------------------------ZeroForce
/** @brief set \ref base_distance_ to the current distance */
void Spring::ZeroForce() {
  base_distance_ = std::sqrt(
    std::pow(B_->x() - A_->x(), 2) + std::pow(B_->y() - A_->y(), 2));
}

// -------------------------------------------------------------------A accessor
/** @brief \ref A_ accessor*/
SpringPoint* Spring::A() {
  return A_;
}

// -------------------------------------------------------------------B accessor
/** @brief \ref B_ accessor*/
SpringPoint* Spring::B() {
  return B_;
}

// -------------------------------------------------------------------K accessor
/** @brief \ref K_ accessor*/
double Spring::K() {
  return K_;
}

// ---------------------------------------------------------------CalculateForce
/** @brief return F = -K*(x - x0) */
double Spring::CalculateForce() {
  double d =
    std::sqrt(std::pow(B_->x() - A_->x(), 2) + std::pow(B_->y() - A_->y(), 2));
  return K_ * -1.0 * (d - base_distance_);
}

}  // namespace Q1

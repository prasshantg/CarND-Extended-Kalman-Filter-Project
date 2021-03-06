#include "kalman_filter.h"
#include "tools.h"
#include <iostream>

using namespace std;

using Eigen::MatrixXd;
using Eigen::VectorXd;

KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::Init(VectorXd &x_in, MatrixXd &P_in, MatrixXd &F_in,
                        MatrixXd &H_in, MatrixXd &R_in, MatrixXd &Q_in) {
  x_ = x_in;
  P_ = P_in;
  F_ = F_in;
  Q_ = Q_in;
  H_ = H_in;
  R_ = R_in;
}

void KalmanFilter::Predict() {
  x_ = F_ * x_;
  MatrixXd Ft = F_.transpose();
  P_ = F_ * P_ * Ft + Q_;
}

void KalmanFilter::Update(const VectorXd &z) {
  VectorXd z_pred = H_ * x_;
  VectorXd y = z - z_pred;
  MatrixXd Ht = H_.transpose();
  MatrixXd PHt = P_ * Ht;
  MatrixXd S = H_ * PHt + R_;
  MatrixXd Si = S.inverse();
  MatrixXd K = PHt * Si;

  //new estimate
  x_ = x_ + (K * y);
  long x_size = x_.size();
  MatrixXd I = MatrixXd::Identity(x_size, x_size);
  P_ = (I - K * H_) * P_;
}

void KalmanFilter::UpdateEKF(const VectorXd &z) {
  /**
   * update the state by using Extended Kalman Filter equations
   */
  float px = x_(0);
  float py = x_(1);
  float vx = x_(2);
  float vy = x_(3);

  if(fabs(px) < SMALL_FLOAT_VAL){
    px = 0.0001;
  }

  float rho = sqrt((px * px) + (py * py));

  if(fabs(rho) < 0.0001){
    rho = 0.0001;
  }

  float theta = atan2(py, px);
  float ro_dot = ((px * vx) + (py *vy)) / rho; 
  VectorXd z_pred = VectorXd(3);

  z_pred << rho, theta, ro_dot;

  VectorXd y_vector = z - z_pred;

  y_vector(1) = NormalizeAngle(y_vector(1));

  MatrixXd Ht = H_.transpose();
  MatrixXd PHt = P_ * Ht;
  MatrixXd S = H_ * PHt + R_;
  MatrixXd Si = S.inverse();
  MatrixXd K =  PHt * Si;  

  MatrixXd I;
  I = MatrixXd::Identity(x_.size(), x_.size());
  //new state
  x_ = x_ + (K * y_vector);
  P_ = (I - K * H_) * P_;  
}

float KalmanFilter::NormalizeAngle(float angle) {
  if(fabs(angle) > M_PI) {
    angle -= round(angle / (2. * M_PI)) * (2.* M_PI);
  }

  return angle;
}


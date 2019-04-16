#pragma once

#include "../../Core/Physics/Tick.h"

class Transpose {
  Matrix4f matrix;
public:
  void setIdentity() {
    matrix.setIdentity();
  }
  void scale(fVec3 scale) {
    Matrix4f with;
    with.at(0, 0) = scale.x;
    with.at(1, 1) = scale.y;
    with.at(2, 2) = scale.z;
    with.at(3, 3) = 1;
    matrix = with*matrix;
  }
  void transform(fVec3 transform) {
    Matrix4f with;
    with.setIdentity();
    with.at(0, 3) = transform.x;
    with.at(1, 3) = transform.y;
    with.at(2, 3) = transform.z;

    matrix = with*matrix;
  }

  void createLook(mVec3 from, sVec3 dir) {
    polar_vec3 p;
    p.fromCartesian(dir);

    float sinp = sin(p.phi);
    float cosp = cos(p.phi);
    float sint = sin(p.theta);
    float cost = cos(p.theta);

    setIdentity();
    transform(-from);
    Matrix4f with;
    float withf[16] = {
      sinp, -cosp, 0, 0,
      -cosp*cost, -sinp*cost, sint, 0,
      -cosp * sint, -sinp*sint, -cost, 0,
      0, 0, 0, 1};
    with.set(withf);
    matrix = with*matrix;
  }
  void project(float fov, float aspectRatio, float zFar, float zNear) {
    float yScale = 1/tan(fov / 2);
    float xScale = yScale / aspectRatio;
    Matrix4f with;
    float withf[16] = {
      xScale, 0, 0, 0,
      0, yScale, 0, 0,
      0, 0, -(zFar + zNear) / (zFar - zNear), -2 * zNear*zFar / (zFar - zNear),
      0, 0, -1, 0};
    with.set(withf);
    matrix = with * matrix;
  }
  void read(float* to) {
    for (int i = 0; i < 16; i++) {
      to[i] = matrix._vals[i];
    }
  }
  void read(double* to) {
    for (int i = 0; i < 16; i++) {
      to[i] = matrix._vals[i];
    }
  }
  void transpose() {
    matrix.transpose();
  }
  Transpose() {
    setIdentity();
  }
};
#pragma once

#include "../../Core/Terrain/WorldLoader.h"

class Transpose {
  Eigen::Matrix4f matrix;
public:
  void setIdentity() {
    matrix = Eigen::Matrix4f::Identity();
  }
  void scale(fVec3 scale) {
    Eigen::Matrix4f with;
    with <<
      scale.x, 0, 0, 0,
    0, scale.y, 0, 0,
    0, 0, scale.z, 0,
    0, 0, 0, 1;
    matrix = with*matrix;
  }
  void transform(fVec3 transform) {
    Eigen::Matrix4f with;
    with <<
      1, 0, 0, transform.x,
      0, 1, 0, transform.y,
      0, 0, 1, transform.z,
      0, 0, 0, 1;
    matrix = with*matrix;
  }
  void rotate(sVec3 around, float with) {
    Eigen::Matrix4f rotm = Eigen::Matrix4f::Identity();
    around.norm();
    Eigen::Vector3f arv;
    arv << around.x, around.y, around.z;
    rotm.block<3,3>(0,0) = Eigen::AngleAxisf(with, arv).matrix();
    matrix = rotm*matrix;
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
    Eigen::Matrix4f with;
    with <<
      sinp, -cosp, 0, 0,
      -cosp*cost, -sinp*cost, sint, 0,
      -cosp * sint, -sinp*sint, -cost, 0,
      0, 0, 0, 1;
    matrix = with*matrix;
    //scale({0.1,0.1,0.1});
  }
  void project(float fov, float aspectRatio, float zFar, float zNear) {
    float yScale = 1/tan(fov / 2);
    float xScale = yScale / aspectRatio;
    Eigen::Matrix4f pmat;
    pmat << xScale, 0, 0, 0,
      0, yScale, 0, 0,
      0, 0, -(zFar + zNear) / (zFar - zNear), -2 * zNear*zFar / (zFar - zNear),
      0, 0, -1, 0;
    matrix = pmat * matrix;
  }
  void read(float* to) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        to[i*4 + j] = matrix(j,i);
      }
    }
  }
  void read(double* to) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        to[i * 4 + j] = matrix(j, i);
      }
    }
  }
  Transpose() {
    setIdentity();
  }
};
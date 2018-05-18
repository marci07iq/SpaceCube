#pragma once

#include "../Definitions.h"
#define EIGEN_MPL2_ONLY //Only MPL2 or better files included
#include "../../NGin/Maths/Eigen/Eigen"

template <typename T, int R>
using ColVector = Eigen::Matrix<T, R, 1>;

template <typename T, int C>
using RowVector = Eigen::Matrix<T, 1, C>;

/* CRC-32C (iSCSI) polynomial in reversed bit order. */
#define POLY 0x82f63b78

/* CRC-32 (Ethernet, ZIP, etc.) polynomial in reversed bit order. */
/* #define POLY 0xedb88320 */

uint32_t crc32c(uint32_t crc, const unsigned char *buf, size_t len);

union quad_int {
  uint8_t b[16];
  int i[4];
};

double locationRandom(int seed, int x, int y);

class Perlin2D {
public:
  vector<vector<fVec2>> _vals;
  int _xl, _yl;
  int _xo, _yo;
  Perlin2D(int xl, int yl, int xo, int yo);
  void setSeed(int gridscale, int seed);
  double getAt(float x, float y);
};
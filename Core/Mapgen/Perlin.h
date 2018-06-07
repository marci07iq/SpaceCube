#pragma once

#include "../Definitions.h"

/* CRC-32C (iSCSI) polynomial in reversed bit order. */
#define POLY 0x82f63b78

/* CRC-32 (Ethernet, ZIP, etc.) polynomial in reversed bit order. */
/* #define POLY 0xedb88320 */

uint32_t crc32c(uint32_t crc, const unsigned char *buf, size_t len);

union hex_int {
  uint8_t b[24];
  int i[6];
};

float locationRandom(int seed, int type, int dim, int x, int y, int z);

float locationRandomF(int seed, int type, int dim, float x, float y, float z);

class Perlin2D {
public:
  vector<vector<fVec2>> _vals;
  int _xl, _yl;
  int _xo, _yo;
  Perlin2D(int xl, int yl, int xo, int yo);
  void setSeed(int gridscale, int seed);
  double getAt(float x, float y);
};
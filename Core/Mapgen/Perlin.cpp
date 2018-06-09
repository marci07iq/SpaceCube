#include "Perlin.h"

uint32_t bitShuffle(uint32_t what) {
  what ^= ((what & 0x55555555) << 1) + ((what & 0xaaaaaaaa) >> 1);
  what ^= ((what & 0x0f0f0f0f) << 4) + ((what & 0xf0f0f0f0) >> 4);
  what ^= ((what & 0x0000ffff) << 16) + ((what & 0xffff0000) >> 16);
  return what;
}

uint32_t crc32c(uint32_t crc, const unsigned char * buf, size_t len) {
  int k;

  crc = ~crc;
  while (len--) {
    crc ^= *buf++;
    for (k = 0; k < 8; k++)
      crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
  }
  return ~crc;
}

float linInterPol(float zero, float one, float at) {
  at = modulo(at, 1);
  return one * at + zero * (1-at);
}

float locationRandom(int seed, int type, int dim, int x, int y, int z) {
  hex_int comb;
  comb.i[0] = seed;
  comb.i[1] = type;
  comb.i[2] = dim;
  comb.i[3] = x;
  comb.i[4] = y;
  comb.i[5] = z;
  return crc32c(0, comb.b, 24) / 4294967296.0;
}

float locationRandomF(int seed, int type, int dim, float x, float y, float z) {
  return linInterPol(
    linInterPol(
      linInterPol(
        locationRandom(seed, type, dim, floor(x), floor(y), floor(z)),
        locationRandom(seed, type, dim, floor(x) + 1, floor(y), floor(z)),
        x
      ),
      linInterPol(
        locationRandom(seed, type, dim, floor(x), floor(y) + 1, floor(z)),
        locationRandom(seed, type, dim, floor(x) + 1, floor(y) + 1, floor(z)),
        x
      ),
      y
    ),
    linInterPol(
      linInterPol(
        locationRandom(seed, type, dim, floor(x), floor(y), floor(z) + 1),
        locationRandom(seed, type, dim, floor(x) + 1, floor(y), floor(z) + 1),
        x
      ),
      linInterPol(
        locationRandom(seed, type, dim, floor(x), floor(y) + 1, floor(z) + 1),
        locationRandom(seed, type, dim, floor(x) + 1, floor(y) + 1, floor(z) + 1),
      x
      ),
    y
    ),
  z);
}

Perlin2D::Perlin2D(int xl, int yl, int xo, int yo) {
  _vals.resize(xl + 1, vector<fVec2>(yl + 1));
  _xl = xl;
  _yl = yl;
  _xo = xo;
  _yo = yo;
}

void Perlin2D::setSeed(int gridscale, int seed) {
  for (int i = 0; i <= _xl; i++) {
    for (int j = 0; j <= _yl; j++) {
      float rval = locationRandom(seed, 0, 0, modulo(_xo + i, MAP_X / gridscale), modulo(_yo + j, MAP_Y / gridscale), gridscale)*TWO_PI;
      _vals[i][j] = fVec2(sin(rval), cos(rval));
    }
  }
}

double Perlin2D::getAt(float x, float y) {
  double posx = x;
  double posy = y;
  int posix = floor(posx);
  int posiy = floor(posy);
  return
    ((_vals[posix + 0][posiy + 0].x * (posx - posix - 0) + _vals[posix + 0][posiy + 0].y * (posy - posiy - 0)))*(1 - posx + posix)*(1 - posy + posiy) +
    ((_vals[posix + 0][posiy + 1].x * (posx - posix - 0) + _vals[posix + 0][posiy + 1].y * (posy - posiy - 1)))*(1 - posx + posix)*(0 + posy - posiy) +
    ((_vals[posix + 1][posiy + 0].x * (posx - posix - 1) + _vals[posix + 1][posiy + 0].y * (posy - posiy - 0)))*(0 + posx - posix)*(1 - posy + posiy) +
    ((_vals[posix + 1][posiy + 1].x * (posx - posix - 1) + _vals[posix + 1][posiy + 1].y * (posy - posiy - 1)))*(0 + posx - posix)*(0 + posy - posiy);
}

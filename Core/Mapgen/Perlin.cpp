#include "Perlin.h"

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

double locationRandom(int seed, int x, int y) {
  tri_int comb;
  comb.i[0] = seed;
  comb.i[1] = x;
  comb.i[2] = y;
  return crc32c(0, comb.b, 12) / 4294967296.0;
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
      _vals[i][j] = fVec2(
        2 * locationRandom(gridscale ^ seed, modulo(_xo + i, MAP_X / gridscale), modulo(_yo + j, MAP_Y / gridscale)) - 1,
        2 * locationRandom(gridscale ^ seed ^ 0xedb88320, modulo(_xo + i, MAP_X / gridscale), modulo(_yo + j, MAP_Y / gridscale)) - 1
      );
    }
  }
}

double Perlin2D::getAt(float x, float y) {
  double posx = x;
  double posy = y;
  int posix = posx;
  int posiy = posy;
  return
    ((_vals[posix + 0][posiy + 0].x * (posx - posix - 0) + _vals[posix + 0][posiy + 0].y * (posy - posiy - 0)))*(1 - posx + posix)*(1 - posy + posiy) +
    ((_vals[posix + 0][posiy + 1].x * (posx - posix - 0) + _vals[posix + 0][posiy + 1].y * (posy - posiy - 1)))*(1 - posx + posix)*(0 + posy - posiy) +
    ((_vals[posix + 1][posiy + 0].x * (posx - posix - 1) + _vals[posix + 1][posiy + 0].y * (posy - posiy - 0)))*(0 + posx - posix)*(1 - posy + posiy) +
    ((_vals[posix + 1][posiy + 1].x * (posx - posix - 1) + _vals[posix + 1][posiy + 1].y * (posy - posiy - 1)))*(0 + posx - posix)*(0 + posy - posiy);
}

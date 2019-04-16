#pragma once

#include "RayTrace.h"

struct floatCol {
  float r, g, b, a;
};

const uint32_t TEX_IMG_SIZE = 16;
const float TEXEL_SIZE = 0.0625f;
const uint32_t TEX_FILE_WIDTH = 16;
const float TEXTURE_SIZE = 0.0625f;

struct PhysCube {
  mVec3 nc;
  mVec3 pc;
  float bounce; //0: none, 1: 100%
  float drag; //0: none, 1: infinite
  float invMass; //1/kg
  mass_density_type_kgpermmm density;

  void offset(mVec3 with) {
    nc += with;
    pc += with;
  }

  bool intersect(PhysCube& other) {
    return band(vecSwitch(nc < other.nc, other.nc, nc) < vecSwitch(pc > other.pc, other.pc, pc));
  }
};

struct QuadFace { //Data to render
  fVec3 vbl, vtl, vtr, vbr; //Vertex coordinates
  vec2<float> tbl, ttl, ttr, tbr; //Texture coordinates
  float brightness;
  void calculateLight() {
    brightness = 0.8 + abs(dot<float>(light, crs(vbr - vbl, vtl - vbl).norm()) * 0.2);
  }
};

typedef uint8_t BlockNeeds;

struct StoredQuadFace {
  QuadFace base;
#ifdef M_CLIENT
  //Example: hollow tube from x- to y+ faces will be 9.
  //1: x-, 2: x+, 4: y-, 8: y+, 16: z-, 32: z+, 64: always
  //Binary storage when to enable.
  //Enabled if one is requested.
  BlockNeeds type;
#endif
  uint32_t faceID;
};

struct BlockModel {
  vector<StoredQuadFace> faces;
  list<PhysCube> cubes; //For physics
};


inline BlockNeeds rotateCW(BlockNeeds what) {
  return (what & 48) | ((what & 1) << 3) | ((what & 14) >> 1);
}

inline BlockNeeds rotateCCW(BlockNeeds what) {
  return (what & 48) | ((what & 8) >> 3) | ((what & 7) << 1);
}
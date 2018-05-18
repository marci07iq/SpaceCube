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
};

struct QuadFace { //Data to render
  fVec3 vbl, vtl, vtr, vbr; //Vertex coordinates
#ifdef M_CLIENT
  vec2<float> tbl, ttl, ttr, tbr; //Texture coordinates
  //float mbl, mtl, mtr, mbr; //Ripple
  floatCol recolor;
#endif
};

struct StoredQuadFace {
  QuadFace base;
  //1: x-, 2: x+, 4: y-, 8: y+, 16: z-, 32: z+, 64: always
  //Binary storage when to enable.
  //Enabled if one is requested.
  //Example: hollow tube from x- to y+ faces will be 9.
#ifdef M_CLIENT
  uint8_t type;
#endif
  uint32_t faceID;
};

struct BlockModel {
  vector<StoredQuadFace> faces;
  list<PhysCube> cubes; //For physics
};

typedef uint8_t BlockNeeds;
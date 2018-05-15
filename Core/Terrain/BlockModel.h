#pragma once

#include "../Definitions.h"

struct floatCol {
  float r, g, b, a;
};

const uint32_t TEX_IMG_SIZE = 16;
const float TEXEL_SIZE = 0.0625f;
const uint32_t TEX_FILE_WIDTH = 1;
const float TEXTURE_SIZE = 1;

struct QuadFace {
  fVec3 vbl, vtl, vtr, vbr; //Vertex coordinates
  vec2<float> tbl, ttl, ttr, tbr; //Texture coordinates
  floatCol recolor;
};

struct StoredQuadFace {
  QuadFace base;
  //1: x-, 2: x+, 4: y-, 8: y+, 16: z-, 32: z+, 64: always
  //Binary storage when to enable.
  //Enabled if one is requested.
  //Example: hollow tube from x- to y+ faces will be 9.
  uint8_t type;
  uint32_t texId;
};

struct BlockModel {
  vector<StoredQuadFace> faces;
};

typedef uint8_t BlockNeeds;
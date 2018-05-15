#pragma once

#include "../Definitions.h"

struct floatCol {
  float r, g, b, a;
};

struct QuadFace {
  fVec3 vbl, vtl, vtr, vbr; //Vertex coordinates
  fVec2 tbl, ttl, ttr, tbr; //Texture coordinates
  floatCol recolor; //
  
  //1: x-, 2: x+, 4: y-, 8: y+, 16: z-, 32: z+, 64: always
  //Binary storage when to enable.
  //Enabled if one is requested.
  //Example: hollow tube from x- to y+ faces will be 9.
  uint8_t type;
};

struct BlockModel {
  vector<QuadFace> faces;
};

typedef uint8_t BlockNeeds;
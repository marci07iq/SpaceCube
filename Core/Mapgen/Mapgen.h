#pragma once

#include "../Terrain/Fragment.h"
#include "Perlin.h"

class Mapgen {
public:
  Fragment* res;
  int dim;
  static list<pair<int, double>> noise_weights;

  Block& Mapgen::getBlock(iVec3 lplace);

  bool isIn(iVec3& what);

  void buildTree(iVec3 lbase);

  void generateFragment(int fx, int fy);
};
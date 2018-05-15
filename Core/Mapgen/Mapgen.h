#pragma once

#include "../Terrain/Fragment.h"
#include "Perlin.h"

class Mapgen {
public:
  Fragment* res;
  int dim;
  static list<pair<int, double>> noise_weights;

  void generateFragment(int fx, int fy);
};
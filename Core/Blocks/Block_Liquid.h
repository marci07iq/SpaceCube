#pragma once

#include "../Terrain/Block.h"

#ifdef M_CLIENT
void getConnectedModel(BlockPos b[7], BlockNeeds n, list<QuadFace>& addTo);
#endif
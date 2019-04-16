#pragma once

#include "../Terrain/Block.h"

void getRotatedPhysics(BlockPos[7], list<PhysCube>& l);

#ifdef M_CLIENT

BlockNeeds getStairNeeds(Block&);

void getRotatedModel(BlockPos b[7], BlockNeeds n, list<QuadFace>& addTo);

#endif
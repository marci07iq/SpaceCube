#pragma once

#include "../Terrain/Block.h"

#ifdef M_CLIENT

BlockNeeds getEmptyNeeds(Block&);
BlockNeeds getSolidNeeds(Block&);

void getStoredModel(BlockPos b[7], BlockNeeds n, list<QuadFace>& addTo);

vec2<float> getStoredTex(BlockPos& b, uint32_t texID);

#endif
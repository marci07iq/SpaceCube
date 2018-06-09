#pragma once

#include "../Terrain/Block.h"

void onDefaultBlockUpdate(BlockPos&);
void onDefaultBlockTick(BlockPos&);
void onDefaultBlockInteract(BlockPos&, Entity*, int, void*);
void onDefaultBlockLook(BlockPos&, Entity*, int, void*);
void onDefaultBlockBreak(BlockPos&, Entity*, int, void*);
void getStoredBlockPhysics(BlockPos[7], list<PhysCube>& l);

#ifdef M_CLIENT

BlockNeeds getEmptyNeeds(Block&);
BlockNeeds getSolidNeeds(Block&);

void getStoredModel(BlockPos b[7], BlockNeeds n, list<QuadFace>& addTo);

void getWoodModel(BlockPos b[7], BlockNeeds n, list<QuadFace>& addTo);

vec2<float> getStoredTex(BlockPos& b, uint32_t texID);

#endif

void actionDestoryNothing(BlockPos&);
void onUpdateNothing(BlockPos&);
void onUpdateNature(BlockPos&);
#pragma once

#include "Region.h"

extern shared_mutex _fragmentsLock;
extern map<world_col_t, Fragment*> _fragments;

void addFragment(Fragment * frag);

Fragment* findFragment(int fx, int fy, int dim);

Fragment* findLoadFragment(int fx, int fy, int dim);

ChunkCol* findChunkCol(int xc, int yc, int dim);

ChunkCol* findLoadChunkCol(int cx, int cy, int dim);

Chunk * findChunk(int xc, int yc, int zc, int dim);

BlockPos & getBlock(int xb, int yb, int zb, int dim, bool& success);

void setBlock(iVec3 location, int dim, Block to);

bool blockNeighbour(BlockPos & block, Directions dir, BlockPos & out);
#ifdef M_CLIENT
void setChunk(int cx, int cy, int cz, int dim, DataElement * data);
#endif

#ifdef M_SERVER

void initMapgen();
#endif
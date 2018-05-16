#pragma once

#include "Region.h"

extern shared_mutex _fragmentsLock;
extern map<world_col_t, Fragment*> _fragments;

void addFragment(Fragment* frag);

Fragment * findFragment(int xf, int yf, int dim);

ChunkCol * findChunkCol(int xc, int yc, int dim);

Chunk * findChunk(int xc, int yc, int zc, int dim);

Block & getBlock(int xb, int yb, int zb, int dim, bool& success);

void setBlock(iVec3 location, int dim, Block to);

#ifdef M_CLIENT
void setChunk(int xc, int yc, int zc, int dim, DataElement* data);
#endif

#ifdef M_SERVER
ChunkCol* findLoadColumn(int xc, int yc, int dim);
void initMapgen();
#endif